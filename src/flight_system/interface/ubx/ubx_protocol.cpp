/**
 * @file ubx.cpp
 * @author Joshua Jerred (https://joshuajer.red/)
 * @brief Initial implementation of the UBX protocol, in progress.
 * @details This is a work in progress, it is currently a bit of a mess
 * as I test different methods of implementing this protocol.
 *
 * The main issue right now is poor reliability of the electrical connection
 * as a logic analyzer shows different behavior compared to what the Pi sees.
 * Need a real scope for this one, not my hantek...
 *
 * @version 0.3
 * @date 2023-01-06
 * @copyright Copyright (c) 2023
 */

#include <BoosterSeat/timer.hpp>

#include <chrono>
#include <cstring>
#include <iostream> // DEBUG
#include <thread>
#include <unistd.h>
#include <unordered_map>

#include "ubx_ids.hpp"
#include "ubx_protocol.hpp"

inline constexpr uint8_t kStreamRegister = 0xFF;

inline constexpr uint8_t kSyncChar1 = 0xB5;
inline constexpr uint8_t kSyncChar2 = 0x62;

#define UBX_ACK_NACK 0x00

inline constexpr uint32_t kMessageRetryDelayTime = 200; // milliseconds
inline constexpr uint32_t kMaxMessageRetryAttempts =
    5; // number of times to retry (up to a second)

/**
 * @brief The maximum size of a UBX message payload in bytes.
 */
inline constexpr uint32_t kMaxPayloadSize = 256;

inline constexpr uint32_t kReadNextMessageTimeout = 500; // milliseconds

inline void wait(int ms) {
  std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

/* U2 - Unsigned Short */
inline uint16_t parseU2(uint8_t msb, uint8_t lsb) {
  return static_cast<uint16_t>(lsb) | (static_cast<uint16_t>(msb) << 8);
}

/* U4 - Unsigned Long */
inline uint32_t parseU4(const uint8_t msb, const uint8_t mid_msb,
                        const uint8_t mid_lsb, const uint8_t lsb) {
  return (uint32_t)lsb | ((uint32_t)mid_lsb << 8) | ((uint32_t)mid_msb << 16) |
         ((uint32_t)msb << 24);
}

/* I2 - Signed Short */
inline int16_t parseI2(const uint8_t msb, const uint8_t lsb) {
  return (int16_t)(lsb | (msb << 8));
}

/* I4 - Signed Long */
inline int32_t parseI4(const uint8_t msb, const uint8_t mid_msb,
                       const uint8_t mid_lsb, const uint8_t lsb) {
  return (int32_t)(lsb | (mid_lsb << 8) | (mid_msb << 16) | (msb << 24));
}

namespace ubx {

UBXMessage::UBXMessage(uint8_t class_ID, uint8_t msg_ID, uint16_t length,
                       std::vector<uint8_t> payload)
    : sync1(kSyncChar1), sync2(kSyncChar2), mClass(class_ID), mID(msg_ID),
      length(length), payload(payload) {
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
bool UBXMessage::calculateChecksum() {
  std::vector<uint8_t> buffer(length + 4, 0);
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
  return true;
}

bool UBXMessage::verifyChecksum() {
  std::vector<uint8_t> buffer(length + 4, 0);
  buffer.at(0) = mClass;
  buffer.at(1) = mID;
  buffer.at(2) = length & 0xFF;
  buffer.at(3) = length >> 8;
  for (int i = 0; i < length; i++) {
    buffer.at(i + 4) = payload.at(i);
  }
  uint8_t a = 0;
  uint8_t b = 0;
  for (int i = 0; i < length + 4; i++) {
    a += buffer.at(i);
    b += a;
  }
  if (a != ck_a || b != ck_b) {
    // std::cout << "Checksum error [" << (int)a << ", " << (int)b << "] != ["
    //<< (int)ck_a << ", " << (int)ck_b << "]" << std::endl;
    return false;
  }
  return true;
}

int getStreamSize(I2cInterface &i2c) {
  constexpr uint8_t kStreamSizeMsbRegister = 0xFD;
  constexpr uint8_t kStreamSizeLsbRegister = 0xFE;

  /**
   * @brief In bytes, prevents over reading when there is an error with message
   * size
   */
  constexpr int32_t kMaxBufferSize = 4000;

  uint8_t msb;
  uint8_t lsb;

  auto msb_res = i2c.readByteFromReg(msb, kStreamSizeMsbRegister);
  auto lsb_res = i2c.readByteFromReg(lsb, kStreamSizeLsbRegister);

  if (msb_res != I2cInterface::Result::SUCCESS ||
      lsb_res != I2cInterface::Result::SUCCESS) {
    return 0;
  }

  int stream_size = (msb << 8) | lsb;
  if (stream_size > kMaxBufferSize) {
    // std::cout << "MESSAGE SIZE ERROR " << stream_size  << std::endl;
    stream_size = 0;
  }
  return stream_size;
}

bool flushStream(I2cInterface &i2c) {
  int stream_size = getStreamSize(i2c);
  if (stream_size <= 0) { // No data to flush
    return true;
  }
  std::vector<uint8_t> buffer(stream_size, 0);
  i2c.readChunkFromReg(buffer, kStreamRegister, stream_size);
  stream_size = getStreamSize(i2c);
  return stream_size == 0;
}

/**
 * @details First finds sync characters, then reads the rest of the message
 * once it becomes available.
 */
bool readNextUBX(I2cInterface &i2c, UBXMessage &message) {
  constexpr uint32_t kTimeoutMs = 1000;

  // clear the old payload
  message.payload.clear();

  // First find the sync characters
  volatile int stream_size = 0;
  uint8_t byte_buffer = 0;
  bool found_sync = false;
  I2cInterface::Result result;

  BoosterSeat::Timer timer(kTimeoutMs);
  while (!timer.isDone()) {
    stream_size = getStreamSize(i2c);
    if (stream_size <= 8) {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      continue;
    }

    // Find the sync characters
    for (int i = 0; i < stream_size; i++) {
      // Find the first sync char
      result = i2c.readByteFromReg(byte_buffer, kStreamRegister);
      if (result != I2cInterface::Result::SUCCESS) {
        continue;
      }
      if (byte_buffer != kSyncChar1) {
        continue;
      }

      // Find the second sync char
      result = i2c.readByteFromReg(byte_buffer, kStreamRegister);
      if (result != I2cInterface::Result::SUCCESS) {
        continue;
      }
      if (byte_buffer != kSyncChar2) {
        continue;
      }

      found_sync = true;
      break;
    }
    if (!found_sync) {
      // std::cout << "NO SYNC CHARACTERS FOUND" << std::endl;
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      continue;
    } else {
      // std::cout << "SYNC CHARACTERS FOUND" << std::endl;
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
      timer.reset();
      if (timer.isDone()) {
        // std::cout << "TIMEOUT 1" << std::endl;
        return false;
      }
    }

    // Read the class, ID, and length
    std::vector<uint8_t> buffer(4, 0);
    result = i2c.readChunkFromReg(buffer, kStreamRegister, 4);
    if (result != I2cInterface::Result::SUCCESS) {
      // std::cout << "ERROR READING CLASS, ID, AND LENGTH" << std::endl;
      return false;
    }
    message.mClass = buffer.at(0);
    message.mID = buffer.at(1);
    message.length = (buffer.at(3) << 8) | buffer[2];

    // Make sure the class and ID are valid
    if (!UbxClassToString.contains(message.mClass)) {
      // std::cout << "READ INVALID CLASS" << std::endl;
      return false;
    }
    if (!UbxIdToString.contains(message.mID)) {
      // std::cout << "READ INVALID ID" << std::endl;
      return false;
    }

    // Verify the message length
    if (message.length > kMaxPayloadSize) {
      // std::cout << "MESSAGE LENGTH ERROR" << std::endl;
      return false;
    }

    // Wait for the rest of the message to be available
    timer.reset();
    while (stream_size < message.length + 2) { // +2 for checksum
      stream_size = getStreamSize(i2c);
      if (timer.isDone()) {
        // std::cout << "TIMEOUT 2" << std::endl;
        return false;
      }
    }

    // Read the payload
    buffer.clear();
    buffer.resize(message.length);
    result = i2c.readChunkFromReg(buffer, kStreamRegister, message.length);
    if (result != I2cInterface::Result::SUCCESS) {
      // std::cout << "ERROR READING PAYLOAD" << std::endl;
      return false;
    }
    message.payload = buffer;

    // Read the checksum
    buffer.clear();
    buffer.resize(2);
    result = i2c.readChunkFromReg(buffer, kStreamRegister, 2);
    if (result != I2cInterface::Result::SUCCESS) {
      // std::cout << "ERROR READING CHECKSUM" << std::endl;
      return false;
    }
    message.ck_a = buffer.at(0);
    message.ck_b = buffer.at(1);

    // Verify the checksum
    if (message.verifyChecksum()) {
      // std::cout << "CHECKSUM VERIFIED" << std::endl;
      return true;
    } else {
      // std::cout << "||CHECKSUM ERROR ss: " << stream_size << "||";
      // std::cout << message << "||" << std::endl;
      return false;
    }
  }
  return false;
}

bool readSpecificMessage(I2cInterface &i2c, UBXMessage &message) {
  uint8_t class_num = message.mClass;
  uint8_t id_num = message.mID;
  uint16_t length = message.length;
  while (readNextUBX(i2c, message)) {
    if (message.mClass == class_num && message.mID == id_num &&
        message.length == length) {
      return true;
    }
  }
  return false;
}

bool writeUBX(I2cInterface &i2c, UBXMessage &message) {
  // Create the buffer space. Fixed size is 8 bytes for the header
  // (and checksum) plus the payload size.
  std::vector<uint8_t> buffer(8 + message.length, 0);

  buffer[0] = message.sync1;                 // sync char 1
  buffer[1] = message.sync2;                 // sync char 2
  buffer[2] = message.mClass;                // class ID
  buffer[3] = message.mID;                   // message ID
  buffer[4] = message.length & 0xFF;         // length LSB (little endian)
  buffer[5] = (message.length >> 8) & 0xFF;  // length MSB (little endian)
  for (int i = 0; i < message.length; i++) { // payload
    buffer[6 + i] = message.payload[i];
  }
  buffer[6 + message.length] = message.ck_a; // checksum A
  buffer[7 + message.length] = message.ck_b; // checksum B
  auto result = i2c.writeChunk(buffer);

  return result == I2cInterface::Result::SUCCESS; // Return true if all bytes
                                                  // were written.
}

ACK checkForAck(I2cInterface &i2c, const uint8_t msg_class,
                const uint8_t msg_id) {
  constexpr uint8_t kAckClassID = 0x05;
  constexpr uint8_t kAckMsgID = 0x01;
  constexpr uint8_t kNackMsgID = 0x00;

  for (uint32_t i = 0; i < kMaxMessageRetryAttempts; i++) {
    while (getStreamSize(i2c) >= 8) {
      UBXMessage message;
      if (readNextUBX(i2c, message)) {
        if (message.mClass == kAckClassID) {
          if (message.mID == kAckMsgID) {
            if (message.payload[0] == msg_class &&
                message.payload[1] == msg_id) {
              return ACK::ACK;
            }
          } else if (message.mID == kNackMsgID) {
            if (message.payload[0] == msg_class &&
                message.payload[1] == msg_id) {
              return ACK::NACK;
            }
          }
        }
      }
    }
    std::this_thread::sleep_for(
        std::chrono::milliseconds(kMessageRetryDelayTime));
    // std::cout << "Sleep" << std::endl;
  }
  return ACK::NONE;
}

/**
 * @brief Send a UBX-CFG-RST command to the receiver (Hardware Reset)
 */
bool sendResetCommand(I2cInterface &i2c) {
  static const uint8_t kMessageClass = 0x06; // CFG
  static const uint8_t kMessageId = 0x04;    // RST
  static const uint16_t kPayloadLength = 4;  // 4 bytes

  // Create the payload
  std::vector<uint8_t> payload = {
      0x00, 0x00, // navBbrMask (Hot Start)
      0x00, 0x00  // resetMode  (Hardware Reset)
  };
  UBXMessage message(kMessageClass, kMessageId, kPayloadLength, payload);
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
ACK setProtocolDDC(I2cInterface &i2c, const bool extended_timeout) {
  static const uint8_t kMessageClass = 0x06; // CFG
  static const uint8_t kMessageId = 0x00;    // PRT
  static const uint16_t kPayloadLength = 20; // 20 bytes

  uint8_t i2c_address = i2c.getAddress();

  // 0b00000010 = extended timeout, 0x00 = normal timeout
  uint8_t extended_timeout_byte = extended_timeout ? 0b00000010 : 0x00;

  // 32.10.25.5 of u-blox 8 / u-blox M8 Receiver Description
  std::vector<uint8_t> payload = {
      0x00, // portID
      0x00, // reserved0
      0x00,
      0x00, // txReady (disabled)
      (uint8_t)(i2c_address << 1),
      0x00,
      0x00,
      0x00, // mode (I2C address, shifted left by 1, little endian)
      0x00,
      0x00,
      0x00,
      0x00, // reserved1
      0x01,
      0x00, // inProtoMask (UBX only)
      0x01,
      0x00, // outProtoMask (UBX only)
      extended_timeout_byte,
      0x00, // flags (extended timeout)
      0x00,
      0x00 // reserved2
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
ACK setMessageRate(I2cInterface &i2c, uint8_t msg_class, uint8_t msg_id,
                   uint8_t rate) {
  static const uint8_t kMessageClass = 0x06; // CFG
  static const uint8_t kMessageId = 0x01;    // MSG
  static const uint16_t kPayloadLength = 3;  // 3 bytes

  std::vector<uint8_t> payload = {msg_class, msg_id, rate};
  UBXMessage message(kMessageClass, kMessageId, kPayloadLength, payload);

  // Send the message
  if (!writeUBX(i2c, message)) {
    return ACK::WRITE_ERROR;
  }
  return checkForAck(i2c, kMessageClass, kMessageId);
}

ACK setMeasurementRate(I2cInterface &i2c, const uint16_t rate_ms) {
  static const uint8_t kMessageClass = 0x06; // CFG
  static const uint8_t kMessageId = 0x08;    // RATE
  static const uint16_t kPayloadLength = 6;  // 6 bytes

  std::vector<uint8_t> payload = {
      (uint8_t)(rate_ms & 0xFF),
      (uint8_t)(rate_ms >> 8), // measRate (ms)
      0x01,
      0x00, // navRate (cycles)
      0x00,
      0x00 // timeRef (UTC)
  };
  UBXMessage message(kMessageClass, kMessageId, kPayloadLength, payload);

  // Send the message
  if (!writeUBX(i2c, message)) {
    return ACK::WRITE_ERROR;
  }
  return checkForAck(i2c, kMessageClass, kMessageId);
}

ACK setDynamicModel(I2cInterface &i2c, const DYNAMIC_MODEL model) {
  static const uint8_t kMessageClass = 0x06; // CFG
  static const uint8_t kMessageId = 0x24;    // NAV5
  static const uint16_t kPayloadLength = 36; // 36 bytes

  std::vector<uint8_t> payload = {
      0x01,           0x00,                  // mask (dynamic model only)
      (uint8_t)model,                        // Dynamic Model
      0x00,                                  // fixMode
      0x00,           0x00, 0x00, 0x00,      // fixedAlt
      0x00,           0x00, 0x00, 0x00,      // fixedAltVar
      0x00,                                  // minElev
      0x00,                                  // drLimit (reserved)
      0x00,           0x00,                  // pDop
      0x00,           0x00,                  // tDop
      0x00,           0x00,                  // pAcc
      0x00,           0x00,                  // tAcc
      0x00,                                  // staticHoldThresh
      0x00,                                  // DGNSS timeout
      0x00,                                  // cnoThreshNumSVs
      0x00,                                  // cnoThresh
      0x00,           0x00,                  // reserved1
      0x00,           0x00,                  // staticHoldMax
      0x00,                                  // UTC standard
      0x00,           0x00, 0x00, 0x00, 0x00 // reserved2
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
bool pollMessage(I2cInterface &i2c, UBXMessage &message,
                 const uint8_t msg_class, const uint8_t msg_id,
                 const int expected_size, const unsigned int timeout_ms) {
  BoosterSeat::Timer timer(timeout_ms);

  while (!timer.isDone()) {
    // Check if the stream is empty, if not, flush it.
    int bytes_available = getStreamSize(i2c);
    if (bytes_available > 0) { // Flush the stream
      if (!flushStream(i2c)) { // Failed to flush the stream
        wait(200);
        continue;
      }
    }

    std::vector<uint8_t> payload(0);
    // empty payload
    UBXMessage poll(msg_class, msg_id, 0, payload);

    // Send the poll message
    if (!writeUBX(i2c, poll)) { // If the write failed
      continue;
    }

    // Wait for the response to be available

    const int standard_delay =
        300; // ms This should be changed depending on the measurement rate
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
    std::vector<uint8_t> buffer(bytes_available);
    auto result =
        i2c.readChunkFromReg(buffer, kStreamRegister, bytes_available);
    if (result != I2cInterface::Result::SUCCESS) { // Failed to read the message
      continue;
    }

    /**
     * @warning I'm suspicious of this after looking back over 6 months later.
     * Then again, I'm suspicious of this entire file.
     */
    if (getStreamSize(i2c) > 0) { // Stream is not empty as expected
      continue;
    }

    if (buffer.at(0) != 0xB5 || buffer.at(1) != 0x62) { // Invalid header
      // std::cout << "Invalid header" << std::endl;
      continue;
    } else if (buffer.at(2) != msg_class ||
               buffer.at(3) != msg_id) { // Wrong message
      // std::cout << "Wrong message " << std::hex << (int)buffer[2] << " " <<
      // (int)buffer[3] << std::endl;
      continue;
    }

    uint16_t length = (buffer.at(5) << 8) | buffer.at(4); // Little endian
    if (length != (uint16_t)expected_size - 8) {          // Wrong length
      // std::cout << "Wrong length " << std::dec << length << std::endl;
      continue;
    }

    /**
     * @brief Not exactly sure if this was the intended purpose, but previously
     * there was undocumented pointer math. This is something to look back at if
     * it's broken.
     */
    std::vector<uint8_t>::const_iterator payload_start = buffer.begin() + 6;
    std::vector<uint8_t>::const_iterator payload_end = buffer.end() + length;
    std::vector<uint8_t> new_payload(payload_start, payload_end);

    message.sync1 = buffer[0];
    message.sync2 = buffer[1];
    message.mClass = buffer[2];
    message.mID = buffer[3];
    message.length = length;
    message.payload = new_payload;
    message.ck_a = buffer[bytes_available - 2];
    message.ck_b = buffer[bytes_available - 1];

    if (message.verifyChecksum()) {
      return true;
    } else {
      // std::cout << "Invalid checksum" << std::endl;
      continue;
    }
  }
  // std::cout << "timeout" << std::endl;
  return false;
}

bool parsePVT(const UBXMessage &message, NavData &data) {
  /*
  First check for message integrity.
  PVT Messages are 92 bytes long
  */
  if (message.mClass != 0x01 || message.mID != 0x07 ||
      message.payload.empty() || message.length != 92) {
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
  payload[20] - U1 fix_type (fix Type)
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
  payload[90] - U2 magAcc (Magnetic Declination Accuracy Estimate in degrees *
  1e-2)
  */
  // Parse the message
  data.year = parseU2(message.payload.at(5), message.payload.at(4));
  data.month = message.payload.at(6);
  data.day = message.payload.at(7);
  data.hour = message.payload.at(8);
  data.minute = message.payload.at(9);
  data.second = message.payload.at(10);

  uint8_t fix_type = message.payload.at(20);
  switch (fix_type) {
  case (0x00):
    data.fix_type = FixType::NO_FIX;
    break;
  case (0x01):
    data.fix_type = FixType::DEAD_RECK;
    break;
  case (0x02):
    data.fix_type = FixType::FIX_2D;
    break;
  case (0x03):
    data.fix_type = FixType::FIX_3D;
    break;
  case (0x04):
    data.fix_type = FixType::COMBINED;
    break;
  case (0x05):
    data.fix_type = FixType::TIME_ONLY;
    break;
  default:
    data.fix_type = FixType::ERROR;
  }

  data.num_satellites = message.payload.at(23);

  int32_t longitude = parseI4(message.payload.at(27), message.payload.at(26),
                              message.payload.at(25), message.payload.at(24));
  data.longitude = (double)longitude / 10000000.0;
  int32_t latitude = parseI4(message.payload.at(31), message.payload.at(30),
                             message.payload.at(29), message.payload.at(28));
  data.latitude = (double)latitude / 10000000.0;
  int32_t horz_accuracy =
      parseU4(message.payload.at(43), message.payload.at(42),
              message.payload.at(41), message.payload.at(40));
  data.horz_accuracy = (double)horz_accuracy / 1000.0;

  int32_t altitude = parseI4(message.payload.at(35), message.payload.at(34),
                             message.payload.at(33), message.payload.at(32));
  data.altitude = (double)altitude / 1000.0;
  int32_t vert_accuracy =
      parseU4(message.payload.at(47), message.payload.at(46),
              message.payload.at(45), message.payload.at(44));
  data.vert_accuracy = (double)vert_accuracy / 1000.0;

  int32_t ground_speed =
      parseI4(message.payload.at(63), message.payload.at(62),
              message.payload.at(61), message.payload.at(60));
  data.ground_speed = (double)ground_speed / 1000.0;
  int32_t speed_accuracy =
      parseU4(message.payload.at(71), message.payload.at(70),
              message.payload.at(69), message.payload.at(68));
  data.speed_accuracy = (double)speed_accuracy / 1000.0;

  int32_t heading = parseI4(message.payload.at(67), message.payload.at(66),
                            message.payload.at(65), message.payload.at(64));
  data.heading_of_motion = (double)heading / 100000.0;
  int32_t heading_accuracy =
      parseU4(message.payload.at(75), message.payload.at(74),
              message.payload.at(73), message.payload.at(72));
  data.heading_accuracy = (double)heading_accuracy / 100000.0;

  return true;
}

/*
-----------------
DEBUGGING HELPERS
-----------------
 */

std::ostream &operator<<(std::ostream &o, const UBXMessage &ubx) {
  if (!UbxClassToString.contains(ubx.mClass)) {
    o << "Unknown Class: " << std::hex << ubx.mClass << " - ";
  } else {
    o << UbxClassToString.at(ubx.mClass);
  }
  if (!UbxIdToString.contains(ubx.mID)) {
    o << "Unknown ID: " << std::hex << ubx.mID << " - ";
  } else {
    o << " " << UbxIdToString.at(ubx.mID);
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

std::ostream &operator<<(std::ostream &o, const NavData &nv) {
  if (nv.valid == true) {
    o << "PVT Parse Error" << std::endl;
    return o;
  }
  o << std::dec << nv.year << "-" << nv.month << "-" << nv.day << " ";
  o << nv.hour << ":" << nv.minute << ":" << nv.second << " -- ";
  switch (nv.fix_type) {
  case FixType::NO_FIX:
    o << "No Fix";
    break;
  case FixType::DEAD_RECK:
    o << "Dead Reckoning Only";
    break;
  case FixType::FIX_2D:
    o << "2D Fix";
    break;
  case FixType::FIX_3D:
    o << "3D Fix";
    break;
  case FixType::COMBINED:
    o << "GPS + Dead Reckoning combined";
    break;
  case FixType::TIME_ONLY:
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

} // namespace ubx