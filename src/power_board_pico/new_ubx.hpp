/// =*========GIRAFFE========*=
/// A Unified Flight Command and Control System
/// https://github.com/joshua-jerred/Giraffe
/// https://giraffe.joshuajer.red/
/// =*=======================*=
///
/// @file   new_ubx.hpp
///
/// =*=======================*=
/// @author     Joshua Jerred (https://joshuajer.red)
/// @date       2025-03-06
/// @copyright  2025 (license to be defined)

#pragma once

#include <cstdint>
#include <span>

namespace keystone {

namespace UBX {

/// @brief Combined UBX class and message IDs, MSB is class, LSB is message
/// @details Bringing them in as needed.
enum class MessageClass : uint16_t {
  UNKNOWN = 0x0000,

  // ACK
  ACK_ACK = 0x0501,
  ACK_NACK = 0x0500,

  // CFG
  // CFG_PRT = 0x0600,
  // CFG_MSG = 0x0601,
  // CFG_RESET = 0x0604,
};

static constexpr size_t HEADER_NUM_BYTES = 6; // sync + class + msg + length
static constexpr size_t CHECKSUM_NUM_BYTES = 2;
static constexpr uint8_t SYNC_CHAR_1 = 0xB5;
static constexpr uint8_t SYNC_CHAR_2 = 0x62;

struct Frame {
  uint8_t sync1 = 0;
  uint8_t sync2 = 0;
  uint8_t class_id = 0;
  uint8_t msg_id = 0;
  uint16_t length = 0;
  std::span<uint8_t> payload;
  uint8_t ck_a = 0;
  uint8_t ck_b = 0;

  MessageClass getMessageClass() const {
    return static_cast<MessageClass>((class_id << 8) | msg_id);
  }

  std::string getMessageClassString() const {
    MessageClass message_class = getMessageClass();
    switch (message_class) {
    case MessageClass::ACK_ACK:
      return "ACK_ACK";
    case MessageClass::ACK_NACK:
      return "ACK_NACK";
    case MessageClass::UNKNOWN:
      return "UNKNOWN";
    default:
      return "NULL";
    }
  }
};

void calculateChecksum(const UBX::Frame &frame, uint8_t &calculated_ck_a,
                       uint8_t &calculated_ck_b) {
  uint8_t new_ck_a = 0;
  uint8_t new_ck_b = 0;

  auto addByte = [&](uint8_t byte) {
    new_ck_a += byte;
    new_ck_b += new_ck_a;
  };

  addByte(frame.class_id);
  addByte(frame.msg_id);
  addByte(frame.length & 0xFF);
  addByte((frame.length >> 8) & 0xFF);

  for (uint8_t byte : frame.payload) {
    addByte(byte);
  }

  calculated_ck_a = new_ck_a;
  calculated_ck_b = new_ck_b;
}

void setFrameForWrite(UBX::Frame &frame, uint8_t new_class_id,
                      uint8_t new_msg_id, std::span<uint8_t> new_payload) {
  frame.class_id = new_class_id;
  frame.msg_id = new_msg_id;
  frame.length = new_payload.size();
  frame.payload = new_payload;

  calculateChecksum(frame, frame.ck_a, frame.ck_b);
};

int processHeader(UBX::Frame &frame, const std::span<uint8_t> &header_buffer) {
  if (header_buffer.size() != HEADER_NUM_BYTES) {
    return -1;
  }

  if (header_buffer[0] != keystone::UBX::SYNC_CHAR_1 ||
      header_buffer[1] != keystone::UBX::SYNC_CHAR_2) {
    return -2;
  }

  frame.class_id = header_buffer[2];
  frame.msg_id = header_buffer[3];
  frame.length = (header_buffer[5] << 8) | header_buffer[4];

  return 0;
}

/// @brief Process a UBX payload and checksum span into a UBX frame. After
/// populating the payload and checksum fields, the is_valid field is set to
/// true if the checksums match.
/// @warning Must be called after processHeader.
/// @param frame[out] - The UBX frame to populate the payload, and checksum
/// fields.
/// @param buffer - The buffer containing the payload and checksum. Must be
/// frame.length + UBX::CHECKSUM_NUM_BYTES in size.
/// @return 0 if the payload and checksum were successfully processed, -1 if
/// the buffer size is incorrect, -2 if the checksums do not match.
int processPayloadAndChecksum(UBX::Frame &frame,
                              const std::span<uint8_t> &buffer) {
  if (frame.length + UBX::CHECKSUM_NUM_BYTES != buffer.size()) {
    return -1;
  }

  frame.payload = buffer.subspan(0, frame.length);
  frame.ck_a = buffer[frame.length];
  frame.ck_b = buffer[frame.length + 1];

  uint8_t calculated_ck_a = 0;
  uint8_t calculated_ck_b = 0;
  calculateChecksum(frame, calculated_ck_a, calculated_ck_b);

  if (frame.ck_a != calculated_ck_a || frame.ck_b != calculated_ck_b) {
    return -2;
  }

  return 0;
}

}; // namespace UBX

}; // namespace keystone