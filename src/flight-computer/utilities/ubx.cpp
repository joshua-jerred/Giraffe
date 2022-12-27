#include <unistd.h>
#include <iostream>  // DEBUG
#include "ubx.h"

#define STREAM_SIZE_MSB_REG 0xFD
#define STREAM_SIZE_LSB_REG 0xFE
#define STREAM_REG 0xFF

#define UBX_SYNC_CHAR_1 0xB5
#define UBX_SYNC_CHAR_2 0x62

#define MESSAGE_RETRY_TIME 0.2  // seconds
#define MESSAGE_RETRY_COUNT 5   // number of times to retry (up to a second)

/** @todo implement this*/
#define MAX_MESSAGE_SIZE 2000  // bytes, prevents over reading when there is an 
                               // error with message size

/**
 * @brief Construct a new UBXMessage, if the payload is not null, the checksum
 * is calculated and stored in ck_a and ck_b. If the payload is null, the
 * checksum is not calculated, sync1, sync2, ck_a, and ck_b are set to 0.
 * 
 * @param class_ID 
 * @param msg_ID 
 * @param length 
 * @param payload 
 */
ubx::UBXMessage::UBXMessage(uint8_t class_ID, uint8_t msg_ID, uint16_t length,
                            uint8_t *payload)
    : sync1(UBX_SYNC_CHAR_1),
      sync2(UBX_SYNC_CHAR_2),
      classID(class_ID),
      msgID(msg_ID),
      length(length),
      payload(payload) {
    if (!calculateChecksum()) {
        sync1 = 0;
        sync2 = 0;
        ck_a = 0;
        ck_b = 0;
    }
}

/**
 * @brief Calculates the checksum for the message
 * @details The checksum is calculated with the Message Class, Message ID,
 * Length bytes, and Payload. The checksum is stored in ck_a and ck_b.
 * 
 * @return true Checksum calculated successfully and stored in ck_a and ck_b
 * @return false Checksum could not be calculated (payload is nullptr)
 * @see 32.4 UBX Checksum of 'u-blox 8 / u-blox M8 Receiver description'
 */
bool ubx::UBXMessage::calculateChecksum() {
    if (payload == nullptr) {
        return false;
    }
    uint8_t *buffer = new uint8_t[length + 4];
    buffer[0] = classID;
    buffer[1] = msgID;
    buffer[2] = length & 0xFF; // LSB (little endian)
    buffer[3] = length >> 8;   // MSB (little endian)
    for (int i = 0; i < length; i++) {
        buffer[i + 4] = payload[i];
    }

    ck_a = 0;
    ck_b = 0;

    for (int i = 0; i < length + 4; i++) {
        ck_a += buffer[i];
        ck_b += ck_a;
    }
    delete[] buffer;
    return true;
}

int ubx::getStreamSize(I2C &i2c) {
    int msb = i2c.readByteFromReg(STREAM_SIZE_MSB_REG);
    int lsb = i2c.readByteFromReg(STREAM_SIZE_LSB_REG);
    int stream_size = (msb << 8) | lsb;
    if (stream_size > MAX_MESSAGE_SIZE) {
        stream_size = 0;
        std::cout << "MESSAGE SIZE ERROR" << std::endl;
    }
    return stream_size;
}

uint8_t *ubx::readUBX(I2C &i2c, const int stream_size) {
    uint8_t *buffer = new uint8_t[stream_size];
    i2c.readChunkFromReg(STREAM_REG, buffer, stream_size);
    return buffer;
}

ubx::UBXMessage ubx::readMessage(I2C &i2c, const uint8_t class_num,
                                 const uint8_t id_num) {
    int stream_size = getStreamSize(i2c);
    uint8_t *buffer = readUBX(i2c, stream_size);

    for (int i = 0; i < stream_size - 4; i++) {
        if (buffer[i] == UBX_SYNC_CHAR_1 && buffer[i + 1] == UBX_SYNC_CHAR_2) {
            if (buffer[i + 2] == class_num && buffer[i + 3] == id_num) {
                uint16_t length = (buffer[i + 5] << 8) | buffer[i + 4];
                uint8_t *payload = new uint8_t[length];
                for (int j = 0; j < length; j++) {
                    payload[j] = buffer[i + 6 + j];
                }
                uint8_t ck_a = buffer[i + 6 + length];
                uint8_t ck_b = buffer[i + 7 + length];
                UBXMessage message;
                message.sync1 = buffer[i];
                message.sync2 = buffer[i + 1];
                message.classID = buffer[i + 2];
                message.msgID = buffer[i + 3];
                message.length = length;
                message.payload = payload;
                message.ck_a = ck_a;
                message.ck_b = ck_b;
                delete[] buffer;
                return message;
            }
        }
    }
    delete[] buffer;
    return UBXMessage();  // Return an empty message (payload = nullptr) if no
                          // message was found
}

bool ubx::writeUBX(I2C &i2c, const ubx::UBXMessage &message) {
    // Create the buffer space. Fixed size is 8 bytes for the header
    // (and checksum) plus the payload size.
    uint8_t *buffer = new uint8_t[message.length + 8];

    buffer[0] = message.sync1;                  // sync char 1
    buffer[1] = message.sync2;                  // sync char 2
    buffer[2] = message.classID;                // class ID
    buffer[3] = message.msgID;                  // message ID
    buffer[4] = message.length & 0xFF;          // length LSB (little endian)
    buffer[5] = (message.length >> 8) & 0xFF;   // length MSB (little endian)
    for (int i = 0; i < message.length; i++) {  // payload
        buffer[6 + i] = message.payload[i];
    }
    buffer[6 + message.length] = message.ck_a;  // checksum A
    buffer[7 + message.length] = message.ck_b;  // checksum B
    int bytes_written = i2c.writeChunk(buffer, message.length + 8);
    delete[] buffer;

    return bytes_written ==
           message.length + 8;  // Return true if all bytes were written.
}

bool ubx::checkForAck(I2C &i2c, const uint8_t msg_class, const uint8_t msg_id) {
    UBXMessage ack;
    int timeout = 0;
    for (int i = 0; i < MESSAGE_RETRY_COUNT; i++) {
        ack = readMessage(i2c, 0x05, 0x01);
        if (ack.payload != nullptr) {
            continue;
        }
        if (ack.length !=
            2) {  // This is almost certainly a failure, but retry anyway
            continue;
        }
        // According to u-blox documentation, the first byte of the payload is
        // the class ID of the message being acknowledged, and the second byte
        // is the message ID.
        if (ack.payload[0] == msg_class && ack.payload[1] == msg_id) {
            return true;
        }
        usleep(MESSAGE_RETRY_TIME);
        timeout++;
    }
    return false;
}

/**
 * @brief Configure the I2C port to use UBX protocol for input and output
 * @details
 * UBX-CFG-PRT (0x06 0x00) Configure I/O Port (Specifically I2C)
 * Length: 20 bytes
 *
 * <portID U1><reserved1 U1><txReady X2><mode X4><reserved2 U1[4]>
 * <inProtoMask X2><outProtoMask X2><flags X2><reserved3 U1[2]>
 * 
 * Does not configure TXReady
 * 
 * @see 32.10.25.5 of u-blox 8 / u-blox M8 Receiver Description
*/
bool ubx::setProtocolDDC(I2C &i2c, bool extended_timeout) {
    uint8_t kClass_ID = 0x06; // CFG
    uint8_t kMessage_ID = 0x00; // PRT

    uint8_t i2c_address = i2c.getAddress();
    
    // 0b00000010 = extended timeout, 0x00 = normal timeout
    uint8_t extended_timeout_byte = extended_timeout ? 0b00000010 : 0x00; 
    
    // 32.10.25.5 of u-blox 8 / u-blox M8 Receiver Description
    uint8_t payload[20] = {
        0x00, // portID
        0x00, // reserved0
        0x00, 0x00, // txReady (disabled)
        (uint8_t)(i2c_address << 1), 0x00, 0x00, 0x00, // mode (I2C address, shifted left by 1, little endian)
        0x00, 0x00, 0x00, 0x00, // reserved1
        0x01, 0x00, // inProtoMask (UBX only)
        0x01, 0x00, // outProtoMask (UBX only)
        extended_timeout_byte, 0x00, // flags (extended timeout)
        0x00, 0x00 // reserved2
    };

    UBXMessage message(kClass_ID, kMessage_ID, 20, payload);

    // Send the message
    return writeUBX(i2c, message);
}

//bool ubx::setMessageRate(I2C &i2c, uint16_t )