#include <unistd.h>
#include <chrono>
#include <thread>
#include <iostream>  // DEBUG
#include <unordered_map>
#include <cstring>

#include "ubx.h"

#define STREAM_SIZE_MSB_REG 0xFD
#define STREAM_SIZE_LSB_REG 0xFE
#define STREAM_REG 0xFF

#define UBX_SYNC_CHAR_1 0xB5
#define UBX_SYNC_CHAR_2 0x62

#define UBX_ACK_NACK 0x00

#define MESSAGE_RETRY_TIME 200  // milliseconds
#define MESSAGE_RETRY_COUNT 5   // number of times to retry (up to a second)

#define MAX_BUFFER_SIZE 4000  // bytes, prevents over reading when there is an 
                               // error with message size
#define MAX_PAYLOAD_SIZE 256

#define READ_NEXT_MESSAGE_TIMEOUT 500  // milliseconds

inline void wait(int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

/* U2 - Unsigned Short */
inline uint16_t parseU2(const uint8_t msb, const uint8_t lsb) {
    return (uint16_t)lsb | ((uint16_t)msb << 8);
}

/* U4 - Unsigned Long */
inline uint32_t parseU4(
    const uint8_t msb, 
    const uint8_t mid_msb, 
    const uint8_t mid_lsb, 
    const uint8_t lsb) {
    return (uint32_t)lsb | ((uint32_t)mid_lsb << 8) | ((uint32_t)mid_msb << 16) | ((uint32_t)msb << 24);
}

/* I2 - Signed Short */
inline int16_t parseI2(const uint8_t msb, const uint8_t lsb) {
    return (int16_t)(lsb | (msb << 8));
}

/* I4 - Signed Long */
inline int32_t parseI4(
    const uint8_t msb, 
    const uint8_t mid_msb, 
    const uint8_t mid_lsb, 
    const uint8_t lsb) {
    return (int32_t)(lsb | (mid_lsb << 8) | (mid_msb << 16) | (msb << 24));
}

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
      mClass(class_ID),
      mID(msg_ID),
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
    buffer[0] = mClass;
    buffer[1] = mID;
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

bool ubx::UBXMessage::verifyChecksum() {
    if (payload == nullptr) {
        return false;
    }
    uint8_t *buffer = new uint8_t[length + 4];
    buffer[0] = mClass;
    buffer[1] = mID;
    buffer[2] = length & 0xFF;
    buffer[3] = length >> 8;
    for (int i = 0; i < length; i++) {
        buffer[i + 4] = payload[i];
    }
    uint8_t a = 0;
    uint8_t b = 0;
    for (int i = 0; i < length + 4; i++) {
        a += buffer[i];
        b += a;
    }
    delete[] buffer;
    if (a != ck_a || b != ck_b) {
        //std::cout << "Checksum error [" << (int)a << ", " << (int)b << "] != [" 
        //<< (int)ck_a << ", " << (int)ck_b << "]" << std::endl;
        return false;
    }
    return true;
}

int ubx::getStreamSize(I2C &i2c) {
    if (i2c.status() != I2C_STATUS::OK) {
        i2c.connect();
        if (i2c.status() != I2C_STATUS::OK) {
            //std::cout << "I2C ERROR" << std::endl;
            return -1;
        }
    }
    int msb = i2c.readByteFromReg(STREAM_SIZE_MSB_REG);
    int lsb = i2c.readByteFromReg(STREAM_SIZE_LSB_REG);
    int stream_size = (msb << 8) | lsb;
    if (stream_size > MAX_BUFFER_SIZE) {
        //std::cout << "MESSAGE SIZE ERROR " << stream_size  << std::endl;
        stream_size = 0;
    }
    return stream_size;
}

bool ubx::flushStream(I2C &i2c) {
    int stream_size = ubx::getStreamSize(i2c);
    if (stream_size <= 0) { // No data to flush
        return true;
    }
    uint8_t *buffer = new uint8_t[stream_size];
    i2c.readChunkFromReg(buffer, stream_size, STREAM_REG);
    delete[] buffer;
    stream_size = ubx::getStreamSize(i2c);
    if (stream_size != 0) {
        std::cout << stream_size << " ";
        return false;
    }
    return true;
}

/**
 * @details First finds sync characters, then reads the rest of the message
 * once it becomes available.
*/
bool ubx::readNextUBX(I2C &i2c, ubx::UBXMessage &message) {
    if (message.payload != nullptr) { // Clear the old payload
        delete[] message.payload;
        message.payload = nullptr;
    }

    // First find the sync characters
    volatile int stream_size;
    uint8_t byte_buffer;
    bool found_sync = false;
    
    time_t start = time(NULL);
    static const int kTimeout_LengthSeconds = 1;
    while (time(NULL) - start < kTimeout_LengthSeconds) {
        stream_size = ubx::getStreamSize(i2c);
        if (stream_size <= 8) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }
        
        // Find the sync characters
        for (int i = 0; i < stream_size; i++) {
            // Find the first sync char
            byte_buffer = i2c.readByteFromReg(STREAM_REG);
            if (byte_buffer != UBX_SYNC_CHAR_1) {
                continue;
            }
            // Find the second sync char
            byte_buffer = i2c.readByteFromReg(STREAM_REG);
            if (byte_buffer != UBX_SYNC_CHAR_2) {
                continue;
            }
            found_sync = true;
            break;
        }
        if (!found_sync) {
            //std::cout << "NO SYNC CHARACTERS FOUND" << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        } else {
            //std::cout << "SYNC CHARACTERS FOUND" << std::endl;
        }
        // Sync characters found, next read the rest of the message
        // if it is available

        // Read the rest of the message
        stream_size = getStreamSize(i2c);
        if (stream_size < 0) { // Error
            continue;
        }
        while (stream_size < 6) {
            stream_size = getStreamSize(i2c);
            start = time(NULL); // Reset the timeout
            if (time(NULL) - start > kTimeout_LengthSeconds) {
                //std::cout << "TIMEOUT 1" << std::endl;
                return false;
            }
        }

        // Read the class, ID, and length
        uint8_t *buffer = new uint8_t[4];
        int bytes_read = i2c.readChunkFromReg(buffer, 4, STREAM_REG);
        if (bytes_read != 4) {
            //std::cout << "ERROR READING CLASS, ID, AND LENGTH" << std::endl;
            delete[] buffer;
            return false;
        }
        message.mClass = buffer[0];
        message.mID = buffer[1];
        message.length = (buffer[3] << 8) | buffer[2];
        delete[] buffer;
        // Make sure the class and ID are valid
        if (!UbxClassToString.contains(message.mClass)) {
            //std::cout << "READ INVALID CLASS" << std::endl;
            return false;
        }
        if (!UbxIdToString.contains(message.mID)) {
            //std::cout << "READ INVALID ID" << std::endl;
            return false;
        }


        // Verify the message length
        if (message.length > MAX_PAYLOAD_SIZE) {
            //std::cout << "MESSAGE LENGTH ERROR" << std::endl;
            return false;
        }

        // Wait for the rest of the message to be available
        start = time(NULL);
        while (stream_size < message.length + 2) { // +2 for checksum
            stream_size = getStreamSize(i2c);
            if (time(NULL) - start > kTimeout_LengthSeconds) {
                //std::cout << "TIMEOUT 2" << std::endl;
                return false;
            }
        }

        // Read the payload
        buffer = new uint8_t[message.length];
        bytes_read = i2c.readChunkFromReg(buffer, message.length, STREAM_REG);
        if (bytes_read != message.length) {
            //std::cout << "ERROR READING PAYLOAD" << std::endl;
            delete[] buffer;
            return false;
        }
        message.payload = buffer;

        // Read the checksum
        buffer = new uint8_t[2];
        bytes_read = i2c.readChunkFromReg(buffer, 2, STREAM_REG);
        if (bytes_read != 2) {
            //std::cout << "ERROR READING CHECKSUM" << std::endl;
            delete[] buffer;
            return false;
        }
        message.ck_a = buffer[0];
        message.ck_b = buffer[1];
        delete[] buffer;
        
        // Verify the checksum
        if (message.verifyChecksum()) {
            //std::cout << "CHECKSUM VERIFIED" << std::endl;
            return true;
        } else {
            //std::cout << "||CHECKSUM ERROR ss: " << stream_size << "||";
            //std::cout << message << "||" << std::endl;
            return false;
        }
    }
    return false;
}

bool ubx::readSpecificMessage(I2C &i2c, ubx::UBXMessage &message) {
    uint8_t class_num = message.mClass;
    uint8_t id_num = message.mID;
    uint16_t length = message.length;
    while (readNextUBX(i2c, message)) {
        if (message.mClass == class_num && message.mID == id_num && message.length == length) {
            return true;
        }
    }
    return false;
}

bool ubx::writeUBX(I2C &i2c, const ubx::UBXMessage &message) {
    // Create the buffer space. Fixed size is 8 bytes for the header
    // (and checksum) plus the payload size.
    uint8_t *buffer = new uint8_t[message.length + 8];

    buffer[0] = message.sync1;                  // sync char 1
    buffer[1] = message.sync2;                  // sync char 2
    buffer[2] = message.mClass;                // class ID
    buffer[3] = message.mID;                  // message ID
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
        while (getStreamSize(i2c) >= 8) {
            ubx::UBXMessage message;
            if (ubx::readNextUBX(i2c, message)) {
                if (message.mClass == kAckClassID) {
                    if (message.mID == kAckMsgID) {
                        if (message.payload[0] == msg_class && message.payload[1] == msg_id) {
                            return ACK::ACK;
                        }
                    } else if (message.mID == kNackMsgID) {
                        if (message.payload[0] == msg_class && message.payload[1] == msg_id) {
                            return ACK::NACK;
                        }
                    }
                }
            }
        
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(MESSAGE_RETRY_TIME));
        //std::cout << "Sleep" << std::endl;
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

/**
 * @brief Poll a message and read it from the GPS
 * @details Messages can be polled by sending the header with a zero length 
 * payload.
*/
bool ubx::pollMessage(I2C &i2c,
        ubx::UBXMessage &message,
        const uint8_t msg_class,
        const uint8_t msg_id,
        const int expected_size,
        const unsigned int timeout_ms) {
    
    uint64_t start_time = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
    uint64_t current_time = start_time;

    while (current_time - start_time < timeout_ms) {
        current_time = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count();
        // Check if the stream is empty, if not, flush it.
        int bytes_available = getStreamSize(i2c);
        if (bytes_available > 0) { // Flush the stream
            if (!flushStream(i2c)) { // Failed to flush the stream
                wait(200);
                continue;
            }
        }

        uint8_t payload[1] = {0x00};
        // Create a poll message, can not have a null ptr for payload
        ubx::UBXMessage poll(msg_class, msg_id, 0, payload);

        // Send the poll message
        if (!writeUBX(i2c, poll)) { // If the write failed
            continue;
        }

        // Wait for the response to be available

        const int standard_delay = 300; // ms This should be changed depending on the measurement rate
        const int max_retries = 3;
        const int retry_delay = 150; // ms

        std::this_thread::sleep_for(std::chrono::milliseconds(standard_delay));

        bool message_available = false;
        int tries = 0;
        while (tries < max_retries) {
            bytes_available = getStreamSize(i2c);
            if (bytes_available < 0) { // Failed to get stream size
                return false;
            }

            if (bytes_available >= expected_size) {
                message_available = true;
                break;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(retry_delay));
            tries++;
        }

        if (!message_available) { // Message not available, try again
            continue;
        }

        // Read the message
        uint8_t *buffer = new uint8_t[bytes_available];
        int bytes_read = i2c.readChunkFromReg(buffer, bytes_available, 0xFF);
        if (bytes_read != bytes_available) { // Failed to read the message
            delete[] buffer;
            continue;
        }

        if (getStreamSize(i2c) > 0) { // Stream is not empty as expected
            delete[] buffer;
            continue;
        }

        if (buffer[0] != 0xB5 || buffer[1] != 0x62) { // Invalid header
            //std::cout << "Invalid header" << std::endl;
            delete[] buffer;
            continue;
        } else if (buffer[2] != msg_class || buffer[3] != msg_id) { // Wrong message
            //std::cout << "Wrong message " << std::hex << (int)buffer[2] << " " << (int)buffer[3] << std::endl;
            delete[] buffer;
            continue;
        }

        uint16_t length = (buffer[5] << 8) | buffer[4]; // Little endian
        if (length != (uint16_t) expected_size - 8) { // Wrong length
            //std::cout << "Wrong length " << std::dec << length << std::endl;
            delete[] buffer;
            continue;
        }

        if (message.payload != nullptr) {
            delete[] message.payload;
            message.payload = nullptr;
        }

        uint8_t *message_buffer = new uint8_t[length]; // Must be deleted set to null ptr
        std::memcpy(message_buffer, buffer + 6, length);
        message.sync1 = buffer[0];
        message.sync2 = buffer[1];
        message.mClass = buffer[2];
        message.mID = buffer[3];
        message.length = length;
        message.payload = message_buffer;
        message.ck_a = buffer[bytes_available - 2];
        message.ck_b = buffer[bytes_available - 1];
        
        if (message.verifyChecksum()) {
            delete[] buffer;
            return true;
        } else {
            //std::cout << "Invalid checksum" << std::endl;
            delete[] buffer;
            continue;
        }
    }
    std::cout << "timeout" << std::endl;
    return false;
}

bool ubx::parsePVT(const UBXMessage &message, NAV_DATA &data) {
    /*
    First check for message integrity.
    PVT Messages are 92 bytes long
    */ 
    if (message.mClass != 0x01 || message.mID != 0x07 ||
        message.payload == nullptr || message.length != 92) {
        return false;
    }

    /*
    payload[0] - U4 iTOW (GPS Time of Week of the navigation epoch)
    payload[4] - U2 year (UTC)
    payload[6] - U1 month 1-12 (UTC)
    payload[7] - U1 day 1-31 (UTC)
    payload[8] - U1 hour 0-23 (UTC)
    payload[9] - U1 min 0-59 (UTC)
    payload[10] - U1 sec
    payload[11] - U1 valid (Validity Flag)
    payload[12] - U4 tAcc (Time Accuracy Estimate in UTC)
    payload[16] - I4 nano (Fraction of second in nanoseconds)
    payload[20] - U1 fixType (fix Type)
        0x00 - No Fix
        0x01 - Dead Reckoning only
        0x02 - 2D-Fix
        0x03 - 3D-Fix
        0x04 - GNSS + dead reckoning combined
        0x05 - Time only fix
    payload[21] - U1 flags (fix Status Flags)
    payload[22] - U1 flags2 (Additional Flags)
    payload[23] - U1 numSV (Number of satellites used in Nav Solution)
    payload[24] - I4 lon (Longitude in 1e-7 degrees)
    payload[28] - I4 lat (Latitude in 1e-7 degrees)
    payload[32] - I4 height (Height above Ellipsoid in mm)
    payload[36] - I4 hMSL (Height above mean sea level in mm)
    payload[40] - U4 hAcc (Horizontal Accuracy Estimate in mm)
    payload[44] - U4 vAcc (Vertical Accuracy Estimate in mm)
    payload[48] - I4 velN (NED north velocity in mm/s)
    payload[52] - I4 velE (NED east velocity in mm/s)
    payload[56] - I4 velD (NED down velocity in mm/s)
    payload[60] - I4 gSpeed (Ground Speed (2-D) in mm/s)
    payload[64] - I4 headMot (Heading of motion 2-D in degrees * 1e-5)
    payload[68] - U4 sAcc (Speed Accuracy Estimate in mm/s)
    payload[72] - U4 headAcc (Heading Accuracy Estimate in degrees * 1e-5)
    payload[76] - U2 pDOP (Position DOP in 0.01)
    payload[78] - X2 flags3 (Additional Flags)
    payload[80] - U1[4] reserved1 (Reserved)
    payload[84] - I4 headVeh (Heading of vehicle 2-D in degrees * 1e-5)
    payload[88] - I2 magDec (Magnetic Declination in degrees * 1e-2)
    payload[90] - U2 magAcc (Magnetic Declination Accuracy Estimate in degrees * 1e-2)
    */
    // Parse the message
    data.year = parseU2(message.payload[5], message.payload[4]);
    data.month = message.payload[6];
    data.day = message.payload[7];
    data.hour = message.payload[8];
    data.minute = message.payload[9];
    data.second = message.payload[10];

    uint8_t fixType = message.payload[20];
    switch (fixType) {
        case(0x00):
            data.fixType = FIX_TYPE::NO_FIX;
            break;
        case(0x01):
            data.fixType = FIX_TYPE::DEAD_RECK;
            break;
        case(0x02):
            data.fixType = FIX_TYPE::FIX_2D;
            break;
        case(0x03):
            data.fixType = FIX_TYPE::FIX_3D;
            break;
        case(0x04):
            data.fixType = FIX_TYPE::COMBINED;
            break;
        case(0x05):
            data.fixType = FIX_TYPE::TIME_ONLY;
            break;
        default:
            data.fixType = FIX_TYPE::ERROR;
    }

    data.num_satellites = message.payload[23];

    int32_t longitude = parseI4(message.payload[27], message.payload[26], message.payload[25], message.payload[24]);
    data.longitude = (double)longitude / 10000000.0;
    int32_t latitude = parseI4(message.payload[31], message.payload[30], message.payload[29], message.payload[28]);
    data.latitude = (double)latitude / 10000000.0;
    int32_t horz_accuracy = parseU4(message.payload[43], message.payload[42], message.payload[41], message.payload[40]);
    data.horz_accuracy = (double)horz_accuracy / 1000.0;

    int32_t altitude = parseI4(message.payload[35], message.payload[34], message.payload[33], message.payload[32]);
    data.altitude = (double)altitude / 1000.0;
    int32_t vert_accuracy = parseU4(message.payload[47], message.payload[46], message.payload[45], message.payload[44]);
    data.vert_accuracy = (double)vert_accuracy / 1000.0;

    int32_t ground_speed = parseI4(message.payload[63], message.payload[62], message.payload[61], message.payload[60]);
    data.ground_speed = (double)ground_speed / 1000.0;
    int32_t speed_accuracy = parseU4(message.payload[71], message.payload[70], message.payload[69], message.payload[68]);
    data.speed_accuracy = (double)speed_accuracy / 1000.0;

    int32_t heading = parseI4(message.payload[67], message.payload[66], message.payload[65], message.payload[64]);
    data.heading_of_motion = (double)heading / 100000.0;
    int32_t heading_accuracy = parseU4(message.payload[75], message.payload[74], message.payload[73], message.payload[72]);
    data.heading_accuracy = (double)heading_accuracy / 100000.0;

    return true;
}

/* 
----------------- 
DEBUGGING HELPERS
-----------------
 */

std::ostream& ubx::operator<<(std::ostream& o, const ubx::UBXMessage& ubx) {
    if (ubx.payload == nullptr) {
        o << "No Message" << std::endl;
        return o;
    }
    if (!UbxClassToString.contains(ubx.mClass)) {
        o << "Unknown Class: " << std::hex << ubx.mClass << " - ";
    } else {
        o << ubx::UbxClassToString.at(ubx.mClass);
    }
    if (!UbxIdToString.contains(ubx.mID)) {
        o << "Unknown ID: " << std::hex << ubx.mID << " - ";
    } else {
        o << " " << ubx::UbxIdToString.at(ubx.mID);
    }
    o << " Length: " << std::dec << ubx.length;
    o << " Payload: ";
    for (int i = 0; i < ubx.length; i++) {
        o << std::hex << (int)ubx.payload[i] << " ";
    }
    o << " ck_a: 0x" << std::hex << (int)ubx.ck_a;
    o << " ck_b: 0x" << std::hex << (int)ubx.ck_b;
    return o;
}

std::ostream& ubx::operator<<(std::ostream& o, const ubx::NAV_DATA& nv) {
    if (nv.valid == true) {
        o << "PVT Parse Error" << std::endl;
        return o;
    }
    o << std::dec << nv.year << "-"  << nv.month << "-" << nv.day << " ";
    o << nv.hour << ":" << nv.minute << ":" << nv.second << " -- ";
    switch (nv.fixType) {
        case ubx::FIX_TYPE::NO_FIX:
            o << "No Fix";
            break;
        case ubx::FIX_TYPE::DEAD_RECK:
            o << "Dead Reckoning Only";
            break;
        case ubx::FIX_TYPE::FIX_2D:
            o << "2D Fix";
            break;
        case ubx::FIX_TYPE::FIX_3D:
            o << "3D Fix";
            break;
        case ubx::FIX_TYPE::COMBINED:
            o << "GPS + Dead Reckoning combined";
            break;
        case ubx::FIX_TYPE::TIME_ONLY:
            o << "Time only fix";
            break;
        default:
            o << "Unknown Fix Type";
            break;
    }
    o << " -- ";
    o << "Lat: " << nv.latitude << " ";
    o << "Lon: " << nv.longitude << " ";
    o << "Alt: " << nv.altitude << " ";
    o << "Speed: " << nv.ground_speed << " ";
    o << "Heading: " << nv.heading_of_motion << " ";
    o << "Acc: " << nv.horz_accuracy << " " << nv.vert_accuracy << " ";
    o << nv.speed_accuracy << " " << nv.heading_accuracy << " ";

    return o;
}

