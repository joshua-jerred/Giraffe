/**
 * @file mcp3021.cpp
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief Driver for the MCP3021
 * @date 2023-02-07
 * @copyright Copyright (c) 2023
 * @version 0.4
 */

#include "extensions.h"

#define MCP3021_ADDRESS 0x4D /** @todo REMOVE THESE */
#define VOLTAGE_REFERENCE 3.385
#define RESISTOR_1 30000
#define RESISTOR_2 7500

extension::MCP3021::MCP3021(DataStream *p_data_stream,
                            ExtensionMetadata extension_metadata)
    : Extension(p_data_stream, extension_metadata),
      bus_number_(extension_metadata.extra_args.I2C_bus),
      device_address_(MCP3021_ADDRESS),
      i2c_bus_(
          I2C(bus_number_, device_address_, p_data_stream->getI2CBusLock())),
      voltage_reference_(VOLTAGE_REFERENCE),
      resistor_1_(RESISTOR_1),
      resistor_2_(RESISTOR_2) {}

extension::MCP3021::~MCP3021() {}

int extension::MCP3021::runner() {
  int result = i2c_bus_.connect();
  if (result != 0 || i2c_bus_.status() != I2C_STATUS::OK) {
    setStatus(ExtensionStatus::ERROR);

    I2C_STATUS status = i2c_bus_.status();
    if (status == I2C_STATUS::CONFIG_ERROR_BUS) {
      error("I2C_CB");
    } else if (status == I2C_STATUS::CONFIG_ERROR_ADDRESS) {
      error("I2C_CA");
    } else if (status == I2C_STATUS::BUS_ERROR) {
      error("I2C_BE");
    } else if (status == I2C_STATUS::ADDRESS_ERROR) {
      error("I2C_AE");
    } else {
      error("I2C_CU");
    }
    return -1;
  }

  result = handshake();
  if (result == 0) {  // Good handshake
    setStatus(ExtensionStatus::RUNNING);
  } else if (result == -1) {
    setStatus(ExtensionStatus::ERROR);
    error("HSK_F");
    return -1;
  } else {
    setStatus(ExtensionStatus::ERROR);
    error("HSK_U");
    return -1;
  }

  setStatus(ExtensionStatus::RUNNING);
  while (!stop_flag_) {
    if (ReadVoltage()) {
      sendData("BAT_V", voltage_, 4);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(getUpdateInterval()));
  }
  setStatus(ExtensionStatus::STOPPED);
  return 0;
}

/**
 * @brief This function is used to test the connection to the MCP3021.
 *
 *
 * @return true - if the handshake was successful
 * @return false - if the handshake was not successful
 */
int extension::MCP3021::handshake() {
  uint8_t data[2] = {0, 0};
  int result = i2c_bus_.readChunk(data, 2);

  if (result == 2) {
    return 0; // Good handshake
  } else if (result == -1) {
    return -1; // Bad handshake
  } else {
    return result; // Unknown failure
  }
}

bool extension::MCP3021::ReadVoltage() {
  uint8_t data[2] = {0, 0};
  int result = i2c_bus_.readChunk(data, 2);

  if (result != 2) {
    return false;
  }

  if ((data[0] & 0b11110000) !=
      0b00000000) {  // Per the datasheet, the first 4 bits of the
                     // upper data byte should be 0
    return false;
  }

  uint16_t raw_data = data[0] << 6;
  raw_data |= data[1] >> 2;

  float raw_voltage = (float)raw_data * voltage_reference_ / 1024.0;

  if ((int)resistor_2_ == 0) {
    voltage_ = raw_voltage;
    return false;
  }

  voltage_ = (raw_voltage * (resistor_1_ + resistor_2_)) / resistor_2_;

  return true;
}