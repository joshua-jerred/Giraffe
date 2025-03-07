/// =*========GIRAFFE========*=
/// A Unified Flight Command and Control System
/// https://github.com/joshua-jerred/Giraffe
/// https://giraffe.joshuajer.red/
/// =*=======================*=
///
/// @file   adc_data.hpp
///
/// =*=======================*=
/// @author     Joshua Jerred (https://joshuajer.red)
/// @date       2025-03-02
/// @copyright  2025 (license to be defined)

#pragma once

#include <cstdint>
#include <vector>

#include "hardware/i2c.h"

#include "new_ubx.hpp"
#include "periodic_processor.hpp"

namespace power_board {

struct UbloxMessage {
  uint8_t sync1 = 0;
  uint8_t sync2 = 0;
  uint8_t class_id = 0;
  uint8_t msg_id = 0;
  uint16_t length = 0;
  std::vector<uint8_t> payload{};
  uint8_t ck_a = 0;
  uint8_t ck_b = 0;

  void setMessage(uint8_t new_class_id, uint8_t new_msg_id,
                  std::vector<uint8_t> new_payload) {
    sync1 = 0xB5;
    sync2 = 0x62;
    class_id = new_class_id;
    msg_id = new_msg_id;
    length = new_payload.size();
    payload = new_payload;

    uint16_t new_ck = calculateChecksum();
    ck_a = new_ck & 0xFF;
    ck_b = new_ck >> 8;
  }

  uint16_t calculateChecksum() const {
    uint8_t new_ck_a = 0;
    uint8_t new_ck_b = 0;

    auto addByte = [&](uint8_t byte) {
      new_ck_a += byte;
      new_ck_b += new_ck_a;
    };

    addByte(class_id);
    addByte(msg_id);
    addByte(length & 0xFF);
    addByte(length >> 8);
    for (int i = 0; i < length; i++) {
      addByte(payload[i]);
    }

    uint16_t checksum = (new_ck_b << 8) | new_ck_a;
    return checksum;
  }
};

static constexpr uint8_t I2C_ADDRESS = 0x42;

#define I2C_PORT i2c1
#define I2C_SDA_PIN 4
#define I2C_SCL_PIN 5

enum class ACK { ACK, NACK, NONE, WRITE_ERROR, READ_ERROR };

class SamM8Q : public PeriodicProcessor {
public:
  SamM8Q(uint64_t module_process_interval_ms)
      : PeriodicProcessor(module_process_interval_ms) {

    // bi_decl(bi_1pin_with_name(ADC_PIN, "ADC input pin"));
    i2c_init(I2C_PORT, 100 * 1000); // 100 kHz

    static constexpr uint8_t SDA_PIN = 14;
    static constexpr uint8_t SCL_PIN = 15;
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);

    i2c_set_slave_mode(I2C_PORT, false, I2C_ADDRESS);

    init();
  }

  void processCommand(const std::string &command) {
    if (command == "init") {
      printf("Initializing SAM-M8Q\n");
      init();
    } else if (command == "reset") {
      printf("Resetting SAM-M8Q\n");
      sendResetCommand();
    } else if (command == "nav") {
      printf("Requesting navigation data\n");
      requestNavData();
    } else {
      printf("Unknown gps command: %s\n", command.c_str());
    }
  }

private:
  void init() {
    printf("Initializing SAM-M8Q\n");
    sendResetCommand();
    printf("Reset command sent!\n");
    sleep_ms(500);
    setProtocolDDC();
    printf("SAM-M8Q initialized\n");
  }

  void processImpl() {
    printf("\n\n\nProcessing SAM-M8Q\n");
    requestNavData();
    // readNavData();
  }

  float readAltitudeM() {
    // adc_select_input(4);

    // From Raspberry Pi Pico-series C/C++ SDK documentation
    // uint16_t adc_count = adc_read();
    // const float conversion_factor = 3.3f / (1 << 12);
    // float voltage = adc_count * conversion_factor;
    // float temp_c = 27 - ((voltage - 0.706f) / 0.001721f);

    return 0.0f;
  }

  void writeUbx() {
    if (ubx_write_buffer_.payload.size() != ubx_write_buffer_.length) {
      printf("Payload size does not match length\n");
      return;
    }

    ubx_write_buffer_.calculateChecksum();

    std::vector<uint8_t> buffer(8 + ubx_write_buffer_.length, 0);

    buffer[0] = ubx_write_buffer_.sync1;
    buffer[1] = ubx_write_buffer_.sync2;
    buffer[2] = ubx_write_buffer_.class_id;
    buffer[3] = ubx_write_buffer_.msg_id;
    buffer[4] = ubx_write_buffer_.length & 0xFF;
    buffer[5] = (ubx_write_buffer_.length >> 8) & 0xFF;
    for (int i = 0; i < ubx_write_buffer_.length; i++) {
      buffer[6 + i] = ubx_write_buffer_.payload[i];
    }

    buffer[6 + ubx_write_buffer_.length] = ubx_write_buffer_.ck_a;
    buffer[7 + ubx_write_buffer_.length] = ubx_write_buffer_.ck_b;

    i2c_write_blocking(I2C_PORT, I2C_ADDRESS, buffer.data(), buffer.size(),
                       false);

    // std::array<uint8_t, 5> buffer = {0xB5, 0x62, 0x06, 0x00, 0x14};
    // i2c_write_blocking(I2C_PORT, 0x42, buffer.data(), buffer.size(),
    // false);
  }

  ACK setProtocolDDC() {
    static const uint8_t kMessageClass = 0x06; // CFG
    static const uint8_t kMessageId = 0x00;    // PRT
    static const uint16_t kPayloadLength = 20; // 20 bytes

    // 0b00000010 = extended timeout, 0x00 = normal timeout
    const bool EXTENDED_TIMEOUT = false;
    uint8_t extended_timeout_byte = EXTENDED_TIMEOUT ? 0b00000010 : 0x00;

    // 32.10.25.5 of u-blox 8 / u-blox M8 Receiver Description
    std::vector<uint8_t> payload = {
        0x00, // portID
        0x00, // reserved0
        0x00,
        0x00, // txReady (disabled)
        (uint8_t)(I2C_ADDRESS << 1),
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
    ubx_write_buffer_.setMessage(kMessageClass, kMessageId, payload);

    // Send the message
    writeUbx();
    return ACK::ACK;
    // return checkForAck(kMessageClass, kMessageId);
  }

  uint16_t getStreamSize() {
    constexpr uint8_t kStreamSizeMsbRegister = 0xFD;
    constexpr uint8_t kStreamSizeLsbRegister = 0xFE;

    uint8_t buffer[2];
    i2c_write_blocking(I2C_PORT, I2C_ADDRESS, &kStreamSizeMsbRegister, 1, true);
    i2c_read_blocking(I2C_PORT, I2C_ADDRESS, buffer, 2, false);

    uint16_t size = (buffer[0] << 8) | buffer[1];
    printf("\n");
    printf("[%d vs %d]", buffer[1] << 8 | buffer[0], size);

    return (buffer[0] << 8) | buffer[1];
  }

  ACK checkForAck(const uint8_t msg_class, const uint8_t msg_id) {
    (void)msg_class;
    (void)msg_id;

    constexpr uint8_t kAckClassID = 0x05;
    constexpr uint8_t kAckMsgID = 0x01;
    constexpr uint8_t kNackMsgID = 0x00;

    // UBXMessage message;
    // if (readNextUBX(i2c, message)) {
    //   if (message.mClass == kAckClassID) {
    //     if (message.mID == kAckMsgID) {
    //       if (message.payload[0] == msg_class && message.payload[1] ==
    //       msg_id) {
    //         return ACK::ACK;
    //       }
    //     } else if (message.mID == kNackMsgID) {
    //       if (message.payload[0] == msg_class && message.payload[1] ==
    //       msg_id) {
    //         return ACK::NACK;
    //       }
    //     }
    //   }
    // }

    // std::this_thread::sleep_for(
    // std::chrono::milliseconds(kMessageRetryDelayTime));
    // std::cout << "Sleep" << std::endl;

    return ACK::NONE;
  }

  void processDdcBuffer() {
    static constexpr uint8_t kStreamRegister = 0xFF;
    static constexpr uint8_t kSyncChar1 = 0xB5;
    static constexpr uint8_t kSyncChar2 = 0x62;
    static constexpr uint8_t HEADER_SIZE = 6;

    // auto readByte = []() {
    //   static constexpr size_t READ_SIZE = 1;
    //   uint8_t buffer[READ_SIZE];
    //   i2c_read_blocking(I2C_PORT, I2C_ADDRESS, buffer, READ_SIZE, false);
    //   return buffer[0];
    // };

    size_t bytes_available = getStreamSize();
    if (bytes_available < 6) { // Not enough bytes for a message with its header
      printf("Not enough bytes");
      return;
    }

    // if (bytes_available >= 1000) {
    // printf("Too many bytes available: %d, resetting.\n", bytes_available);
    // Flush the buffer
    // }

    // Read from the DDC buffer
    i2c_write_blocking(I2C_PORT, I2C_ADDRESS, &kStreamRegister, 1, true);

    // Read the header
    {
      uint8_t header_buffer[HEADER_SIZE];
      i2c_read_blocking(I2C_PORT, I2C_ADDRESS, header_buffer, HEADER_SIZE,
                        false);

      printf("Header: ");
      for (size_t i = 0; i < HEADER_SIZE; i++) {
        printf("%02X ", header_buffer[i]);
      }
    }
  }

  void sendResetCommand() {
    ubx_write_buffer_.setMessage(0x06, 0x04, {0x00, 0x00, 0x00, 0x00});
    writeUbx();
  }

  void requestNavData() {
    ubx_write_buffer_.setMessage(0x06, 0x24, {0x00, 0x00, 0x00, 0x00});
    transaction(92);
  }

  void flushStream() {
    static constexpr uint8_t kStreamRegister = 0xFF;

    const size_t bytes_available = getStreamSize();
    if (bytes_available <= 0) {
      printf("No data to flush\n");
      return;
    }

    for (size_t i = 0; i < bytes_available; i++) {
      uint8_t buffer[1];
      i2c_read_blocking(I2C_PORT, I2C_ADDRESS, buffer, 1, false);
    }
    const size_t post_bytes_available = getStreamSize();
    printf("Flushed %d bytes, %d remain\n", bytes_available,
           post_bytes_available);
  }

  void transaction(size_t expected_return_size) {
    // Flush the buffer
    flushStream();
    printf("Sending request\n");

    // Send the request
    writeUbx();
    printf("Waiting for response\n");

    // Wait for the response
    {
      const size_t kStandardDelay = 500; // ms
      const uint64_t start_time = time_us_64() / 1000;
      bool message_header_available = false;
      while (time_us_64() / 1000 - start_time < kStandardDelay) {
        if (getStreamSize() >= keystone::UBX::HEADER_NUM_BYTES) {
          message_header_available = true;
          break;
        }
        sleep_ms(50);
      }

      if (!message_header_available) {
        printf("Message not available\n");
        return;
      }
    }

    // Read the header
    {
      static constexpr uint8_t HEADER_SIZE = 6;
      uint8_t header_buffer[HEADER_SIZE];
      i2c_read_blocking(I2C_PORT, I2C_ADDRESS, header_buffer, HEADER_SIZE,
                        false);

      int result = processHeader(ubx_read_buffer_, header_buffer);
      if (result < 0) {
        printf("Error processing header: %d\n", result);
        return;
      }

      const std::string message_class =
          ubx_read_buffer_.getMessageClassString();
      printf("proc %s[%02X, %02X], Length: %d\n", message_class.c_str(),
             ubx_read_buffer_.class_id, ubx_read_buffer_.msg_id,
             ubx_read_buffer_.length);
    }

    {

      constexpr uint64_t TIMEOUT_us = 500 * 1000; // 500 ms
      const uint64_t start_time_us = time_us_64();
      uint16_t bytes_available = 0;
      bool payload_and_checksum_available = false;
      while (time_us_64() - start_time_us < TIMEOUT_us) {
        bytes_available = getStreamSize();
        if (bytes_available >=
            ubx_read_buffer_.length +
                static_cast<uint16_t>(keystone::UBX::CHECKSUM_NUM_BYTES)) {
          payload_and_checksum_available = true;
          break;
        }

        sleep_ms(50);
      }

      if (!payload_and_checksum_available) {
        printf("Payload or checksum not available\n");
        return;
      }

      // Read the payload and checksum
      std::vector<uint8_t> payload(ubx_read_buffer_.length +
                                   keystone::UBX::CHECKSUM_NUM_BYTES);
      i2c_read_blocking(I2C_PORT, I2C_ADDRESS, payload.data(), payload.size(),
                        false);

      keystone::UBX::processPayloadAndChecksum(ubx_read_buffer_, payload);
    }

    // std::vector<uint8_t> payload(ubx_read_buffer_.length);

    (void)expected_return_size; /// @todo use
  }

  UbloxMessage ubx_write_buffer_;
  keystone::UBX::Frame ubx_read_buffer_;
};

} // namespace power_board