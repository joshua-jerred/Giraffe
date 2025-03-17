/// =*========GIRAFFE========*=
/// A Unified Flight Command and Control System
/// https://github.com/joshua-jerred/Giraffe
/// https://giraffe.joshuajer.red/
/// =*=======================*=
///
/// @file   EEPROM.hpp
///
/// =*=======================*=
/// @author     Joshua Jerred (https://joshuajer.red)
/// @date       2025-03-15
/// @copyright  2025 (license to be defined)

#pragma once

#include <cstdint>
#include <vector>

#include "hardware/i2c.h"
#include "keystone/ubx.hpp"

#include "periodic_processor.hpp"

#define EEPROM_I2C_PORT i2c0

namespace power_board {

class EEPROM {
public:
  static constexpr uint8_t DEVICE_I2C_ADDRESS = 0x50;

  EEPROM() {
    init();
  }

  void init() {
    if (is_initialized_) {
      printf("<><>EEPROM already initialized\n");
      return;
    }
    is_initialized_ = true;

    i2c_init(EEPROM_I2C_PORT, 100 * 1000); // 100 kHz

    static constexpr uint8_t SDA_PIN = 4;
    static constexpr uint8_t SCL_PIN = 5;
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);

    i2c_set_slave_mode(EEPROM_I2C_PORT, false, DEVICE_I2C_ADDRESS);

    printf("EEPROM initialized\n");
  }

  std::vector<uint8_t> read(uint16_t address, size_t length) {
    printf("Reading from EEPROM\n");
    std::vector<uint8_t> data(length);
    i2c_write_blocking(EEPROM_I2C_PORT, DEVICE_I2C_ADDRESS, (uint8_t *)&address,
                       2, true);
    i2c_read_blocking(EEPROM_I2C_PORT, DEVICE_I2C_ADDRESS, data.data(), length,
                      false);
    return data;
  }

  void write(uint16_t address, const std::vector<uint8_t> &data) {
    printf("Writing to EEPROM\n");
    std::vector<uint8_t> buffer(data.size() + 2);
    buffer[0] = address >> 8;   // MSB
    buffer[1] = address & 0xFF; // LSB
    for (size_t i = 0; i < data.size(); i++) {
      buffer[i + 2] = data[i];
    }
    i2c_write_blocking(EEPROM_I2C_PORT, DEVICE_I2C_ADDRESS, buffer.data(),
                       buffer.size(), false);
  }

  bool eepromExists() {
    // https://github.com/raspberrypi/pico-examples/blob/master/i2c/bus_scan/bus_scan.c
    printf("Scanning for EEPROM\n");

    // Skip over any reserved addresses.
    int ret;
    uint8_t rxdata;
    ret = i2c_read_blocking(i2c_default, DEVICE_I2C_ADDRESS, &rxdata, 1, false);

    if (ret == 1) {
      printf("EEPROM found at address 0x%02X\n", DEVICE_I2C_ADDRESS);
      return true;
    } else {
      printf("EEPROM not found\n");
      return false;
    }
  }

  void processCommand(const std::string &command) {
    if (command == "init") {
      init();
    } else if (command == "read") {
      // Example: read 16 bytes from address 0x0000
      auto data = read(0x0000, 16);
      printf("Read data: ");
      for (auto byte : data) {
        printf("%c ", byte);
      }
      printf("\n");
    } else if (command == "scan") {
      eepromExists();
    } else if (command == "write") {
      // Example: write 16 bytes to address 0x0000
      std::string data_str = "KEYSTONE";
      std::vector<uint8_t> data(data_str.begin(), data_str.end());
      write(0x0000, data);
      printf("Wrote data to EEPROM\n");
    } else {
      printf("Unknown command in EEPROM: %s\n", command.c_str());
    }
  }

private:
  bool is_initialized_ = false;
};

} // namespace power_board