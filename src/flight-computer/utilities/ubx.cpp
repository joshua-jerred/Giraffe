#include <unistd.h>
#include <iostream>  // DEBUG
#include "ubx.h"

#define STREAM_SIZE_MSB_REG 0xFD
#define STREAM_SIZE_LSB_REG 0xFE
#define STREAM_REG 0xFF

#define UBX_SYNC_CHAR_1 0xB5
#define UBX_SYNC_CHAR_2 0x62

#define UBX_ACK_NACK 0x00

#define MESSAGE_RETRY_TIME 20000  // microseconds (200ms)
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
    if (i2c.status() != I2C_STATUS::OK) {
        return -1;
    }
    int msb = i2c.readByteFromReg(STREAM_SIZE_MSB_REG);
    int lsb = i2c.readByteFromReg(STREAM_SIZE_LSB_REG);
    int stream_size = (msb << 8) | lsb;
    //if (stream_size > MAX_MESSAGE_SIZE) {
    //    stream_size = 0;
    //    std::cout << "MESSAGE SIZE ERROR" << std::endl;
    //}
    return stream_size;
}

uint8_t *ubx::readUBX(I2C &i2c, const int stream_size) {
    uint8_t *buffer = new uint8_t[stream_size];
    if (i2c.readChunkFromReg(STREAM_REG, buffer, stream_size) != stream_size) {
        delete[] buffer;
        return nullptr;
    }
    return buffer;
}

ubx::UBXMessage ubx::readMessage(I2C &i2c, const uint8_t class_num,
                                 const uint8_t id_num) {
    int stream_size = getStreamSize(i2c);
    uint8_t *buffer = readUBX(i2c, stream_size);

    if (buffer == nullptr) {
        UBXMessage message;
        message.sync1 = 0;
        message.sync2 = 0;
        message.classID = 0;
        message.msgID = 0;
        message.length = 0;
        message.payload = nullptr;
        message.ck_a = 0;
        message.ck_b = 0;
        return message;
    }

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

ubx::ACK ubx::checkForAck(I2C &i2c, const uint8_t msg_class, const uint8_t msg_id) {
    static const uint8_t kAckClassID = 0x05;
    static const uint8_t kAckMsgID = 0x01;
    static const uint8_t kNackMsgID = 0x00;
    
    for (int i = 0; i < MESSAGE_RETRY_COUNT; i++) {
        int stream_size = getStreamSize(i2c);
        uint8_t *buffer = readUBX(i2c, stream_size);

        if (buffer == nullptr) {
            return ACK::READ_ERROR;
        }

        for (int i = 0; i < stream_size - 4; i++) {
            // Find a UBX message
            if (buffer[i] == UBX_SYNC_CHAR_1 &&
                buffer[i + 1] == UBX_SYNC_CHAR_2) {
                // Found a frame, next check if it's an ACK class message,
                // if not, skip over it by payload length + 2 bytes for the
                // checksum
                if (buffer[i + 2] != kAckClassID) {
                    //i += (buffer[i + 4] + (buffer[i + 5] << 8)) + 1;
                    continue;
                }
                // First check to make sure the payload length is 2 bytes
                if (buffer[i + 4] != 0x02 || buffer[i + 5] != 0x00) {
                    continue; // Try again
                }
                // Check if the ACK message is for the message we want
                if (buffer[i + 6] != msg_class || buffer[i + 7] != msg_id) {
                    continue; // Not the ACK we want, skip it.
                }
                // Check if the ACK message is an ACK or NACK
                if (buffer[i + 3] == kAckMsgID) {
                    delete[] buffer;
                    return ACK::ACK;
                } else if (buffer[i + 3] == kNackMsgID) {
                    delete[] buffer;
                    return ACK::NACK;
                } else {
                    continue; // Try again
                }
            }
        }
        delete[] buffer;
        usleep(MESSAGE_RETRY_TIME);
        std::cout << "Sleep" << std::endl;
    }
    return ACK::NONE;
}

/**
 * @brief Send a UBX-CFG-RST command to the receiver (Hardware Reset)
*/
bool ubx::sendResetCommand(I2C &i2c) {
    static const uint8_t  kMessageClass  = 0x06; // CFG
    static const uint8_t  kMessageId     = 0x04; // RST
    static const uint16_t kPayloadLength = 4;    // 4 bytes

    // Create the payload
    uint8_t payload[kPayloadLength] = 
        {
        0x00, 0x00, // navBbrMask (Hot Start)
        0x00, 0x00  // resetMode  (Hardware Reset)
        };
    ubx::UBXMessage message(kMessageClass, kMessageId, kPayloadLength, payload);
    int result = writeUBX(i2c, message);
    usleep(5000); // Wait for the reset to complete (500 ms)
    return result;
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
ubx::ACK ubx::setProtocolDDC(I2C &i2c, const bool extended_timeout) {
    static const uint8_t kMessageClass   = 0x06; // CFG
    static const uint8_t kMessageId      = 0x00; // PRT
    static const uint16_t kPayloadLength = 20;   // 20 bytes

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
    UBXMessage message(kMessageClass, kMessageId, kPayloadLength, payload);

    // Send the message
    if (!writeUBX(i2c, message)) {
        return ACK::WRITE_ERROR;
    }
    return checkForAck(i2c, kMessageClass, kMessageId);
}

/**
 * @brief CFG-MSG (0x06 0x01) Configure Message Rate
 * @details "Send rate is relative to the event a message is registered on. For 
 * example, if the rate of a navigation message is set to 2, the message is sent 
 * every second navigation solution."
 * @see 32.10.18.3
*/
ubx::ACK ubx::setMessageRate(I2C &i2c, uint8_t msg_class, uint8_t msg_id, uint8_t rate) {
    static const uint8_t kMessageClass   = 0x06; // CFG
    static const uint8_t kMessageId      = 0x01; // MSG
    static const uint16_t kPayloadLength = 3;    // 3 bytes

    uint8_t payload[3] = {msg_class, msg_id, rate};
    UBXMessage message(kMessageClass, kMessageId, kPayloadLength, payload);

    // Send the message
    if (!writeUBX(i2c, message)) {
        return ACK::WRITE_ERROR;
    }
    return checkForAck(i2c, kMessageClass, kMessageId);
}

ubx::ACK ubx::setMeasurementRate(I2C &i2c, const uint16_t rate_ms) {
    static const uint8_t kMessageClass   = 0x06; // CFG
    static const uint8_t kMessageId      = 0x08; // RATE
    static const uint16_t kPayloadLength = 6;    // 6 bytes

    uint8_t payload[6] = {
        (uint8_t)(rate_ms & 0xFF), (uint8_t)(rate_ms >> 8), // measRate (ms)
        0x01, 0x00, // navRate (cycles)
        0x00, 0x00  // timeRef (UTC)
    };
    UBXMessage message(kMessageClass, kMessageId, kPayloadLength, payload);

    // Send the message
    if (!writeUBX(i2c, message)) {
        return ACK::WRITE_ERROR;
    }
    return checkForAck(i2c, kMessageClass, kMessageId);
}

ubx::ACK ubx::setDynamicModel(I2C &i2c, const ubx::DYNAMIC_MODEL model) {
    static const uint8_t kMessageClass   = 0x06; // CFG
    static const uint8_t kMessageId      = 0x24; // NAV5
    static const uint16_t kPayloadLength = 36;   // 36 bytes

    uint8_t payload[kPayloadLength] = {
        0x01, 0x00, // mask (dynamic model only)
        (uint8_t)model, // Dynamic Model
        0x00, // fixMode
        0x00, 0x00, 0x00, 0x00, // fixedAlt
        0x00, 0x00, 0x00, 0x00, // fixedAltVar
        0x00, // minElev
        0x00, // drLimit (reserved)
        0x00, 0x00, // pDop
        0x00, 0x00, // tDop
        0x00, 0x00, // pAcc
        0x00, 0x00, // tAcc
        0x00, // staticHoldThresh
        0x00, // DGNSS timeout
        0x00, // cnoThreshNumSVs
        0x00, // cnoThresh
        0x00, 0x00, // reserved1
        0x00, 0x00, // staticHoldMax
        0x00, // UTC standard
        0x00, 0x00, 0x00, 0x00, 0x00 // reserved2
    };
    UBXMessage message(kMessageClass, kMessageId, kPayloadLength, payload);

    // Send the message
    if (!writeUBX(i2c, message)) {
        return ACK::WRITE_ERROR;
    }
    return checkForAck(i2c, kMessageClass, kMessageId);
}