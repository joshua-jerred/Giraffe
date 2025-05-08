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
#include "keystone/ubx.hpp"

#include "periodic_processor.hpp"
#include "software_timer.hpp"

namespace power_board {

static constexpr uint8_t I2C_ADDRESS = 0x42;

#define I2C_PORT i2c1

enum class ACK {
  ACK,
  NACK,
  NONE,
  WRITE_ERROR,
  READ_ERROR
};

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
      printf("Initializing SAM-M8Q\n/");
      init();
    } else if (command == "reset") {
      printf("Resetting SAM-M8Q\n");
      sendResetCommand();
    } else if (command == "nav") {
      printf("Requesting navigation data\n");
      requestNavData();
      displayNavPvt();
    } else if (command == "stream") {
      stream_enabled_ = !stream_enabled_;
    } else {
      printf("Unknown gps command: %s\n", command.c_str());
    }
  }

private:
  void init() {
    // printf("Initializing SAM-M8Q\n");
    sendResetCommand();
    // printf("Reset command sent!\n");
    sleep_ms(500);
    setProtocolDDC();
    setMeasurementRate();
    // printf("SAM-M8Q initialized\n");
  }

  void processImpl() {
    if (nav_update_timer_.isExpired()) {
      if (requestNavData() == ACK::ACK) {
        nav_update_timer_.reset();
        gps_watchdog_timer_.reset();
      }
    }

    if (stream_enabled_ && stream_timer_.isExpired()) {
      printf(nav_data_.toNmeaGPGGA().c_str());
      stream_timer_.reset();
    }

    if (gps_watchdog_timer_.isExpired()) {
      printf("GPS Watchdog timeout\n");
      init();
    }
  }

  void writeUbx() {
    if (ubx_write_buffer_.payload.size() != ubx_write_buffer_.length) {
      printf("Payload size does not match length\n");
      return;
    }

    ubx_write_buffer_.ck_a = 0;
    ubx_write_buffer_.ck_b = 0;
    keystone::UBX::calculateChecksum(ubx_write_buffer_, ubx_write_buffer_.ck_a,
                                     ubx_write_buffer_.ck_b);

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

    // printf("Writing UBX: ");
    // for (size_t i = 0; i < buffer.size(); i++) {
    // printf("%02X ", buffer[i]);
    // }
    // printf("\n");

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
    keystone::UBX::setFrameForWrite(ubx_write_buffer_, kMessageClass,
                                    kMessageId, payload);

    // Send the message

    return transaction(2);
    // return checkForAck(kMessageClass, kMessageId);
  }

  ACK setMeasurementRate(const uint16_t rate_ms = 100) {
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

    keystone::UBX::setFrameForWrite(ubx_write_buffer_, kMessageClass,
                                    kMessageId, payload);

    // Send the message
    return transaction(2);
    // return checkForAck(kMessageClass, kMessageId);
  }

  uint16_t getStreamSize() {
    constexpr uint8_t kStreamSizeMsbRegister = 0xFD;
    constexpr uint8_t kStreamSizeLsbRegister = 0xFE;

    uint8_t buffer[2];
    i2c_write_blocking(I2C_PORT, I2C_ADDRESS, &kStreamSizeMsbRegister, 1, true);
    i2c_read_blocking(I2C_PORT, I2C_ADDRESS, buffer, 2, false);

    uint16_t size = (buffer[0] << 8) | buffer[1];
    // printf("\n");
    // printf("[%d vs %d]", buffer[1] << 8 | buffer[0], size);

    return (buffer[0] << 8) | buffer[1];
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
    std::vector<uint8_t> payload = {0xFF, 0xFF, 0x00, 0x00};
    keystone::UBX::setFrameForWrite(ubx_write_buffer_, 0x06, 0x04, payload);
    writeUbx();
  }

  ACK requestNavData(uint8_t poll_request_id = 0x00) {
    std::vector<uint8_t> payload = {};
    if (poll_request_id != 0x00) {
      payload.push_back(poll_request_id);
    }
    keystone::UBX::setFrameForWrite(ubx_write_buffer_, 0x01, 0x07, payload);
    return transaction(92);
  }

  void flushStream() {
    static constexpr uint8_t kStreamRegister = 0xFF;

    const size_t bytes_available = getStreamSize();
    if (bytes_available <= 0) {
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

  ACK transaction(size_t expected_return_size) {
    // Flush the buffer
    flushStream();

    // Send the request
    writeUbx();
    // printf("Waiting for response. ");

    // Wait for the response
    {
      const size_t kStandardDelay = 1000; // ms
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
        // printf("Message not available, num bytes: %d\n", getStreamSize());
        return ACK::READ_ERROR;
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
        for (size_t i = 0; i < HEADER_SIZE; i++) {
          printf("%02X ", header_buffer[i]);
        }
        printf("\n");
        return ACK::READ_ERROR;
      }

      const std::string message_class = keystone::UBX::getMessageClassString(
          ubx_read_buffer_.getMessageClass());
      // printf("proc %s[%02X, %02X], Length: %d\n", message_class.c_str(),
      //  ubx_read_buffer_.class_id, ubx_read_buffer_.msg_id,
      //  ubx_read_buffer_.length);
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

        sleep_ms(10);
      }

      if (!payload_and_checksum_available) {
        printf("Payload or checksum not available\n");
        return ACK::READ_ERROR;
      }

      // Read the payload and checksum
      std::vector<uint8_t> payload(ubx_read_buffer_.length +
                                   keystone::UBX::CHECKSUM_NUM_BYTES);
      i2c_read_blocking(I2C_PORT, I2C_ADDRESS, payload.data(), payload.size(),
                        false);

      int result =
          keystone::UBX::processPayloadAndChecksum(ubx_read_buffer_, payload);
      if (result < 0) {
        printf("Error processing payload and checksum: %d\n", result);
        return ACK::READ_ERROR;
      }

      std::string frame_string =
          keystone::UBX::getFrameString(ubx_read_buffer_);

      auto message_class = ubx_read_buffer_.getMessageClass();
      if (message_class ==
          keystone ::UBX::MessageClass::NAV_PVT) { // NAV-PVT message
        int status = nav_data_.decode(ubx_read_buffer_);
        if (status != 0) {
          printf("Error decoding NAV-PVT: %d\n", status);
          return ACK::READ_ERROR;
        }

        // std::string nav_pvt_string = nav_data_.toString();
        // printf("NAV-PVT: %s\n", nav_pvt_string.c_str());
        return ACK::ACK;
      } else if (message_class ==
                 keystone ::UBX::MessageClass::ACK_ACK) { // ACK-ACK message
        return ACK::ACK;
      } else {
        printf("Rx %s\n", frame_string.c_str());
      }
    }

    (void)expected_return_size; /// @todo use
    return ACK::NONE;
  }

  void displayNavPvt() {
    std::string nav_pvt_string = nav_data_.toString();
    printf("NAV-PVT: %s\n", nav_pvt_string.c_str());
  }

  std::vector<uint8_t> tx_buffer_;
  keystone::UBX::Frame ubx_write_buffer_{tx_buffer_};

  std::vector<uint8_t> rx_buffer_;
  keystone::UBX::Frame ubx_read_buffer_{rx_buffer_};

  keystone::UBX::Data::NavPvt nav_data_;

  SoftwareTimer nav_update_timer_{800}; // @todo config

  SoftwareTimer gps_watchdog_timer_{3000}; // @todo config

  SoftwareTimer stream_timer_{1000}; // @todo config

  /// @todo disable by default in the future
  bool stream_enabled_ = true;
};

} // namespace power_board