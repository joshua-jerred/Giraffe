/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   mcp3021.cpp
 * @brief  MCP3021 I2C ADC Driver
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-07-05
 * @copyright  2023 (license to be defined)
 */

#include "mcp3021.hpp"

/**
 * @todo Remove this, make it configurable!
 */
inline constexpr uint8_t kMcp3021Address = 0x4D;

namespace extension {
Mcp3021Extension::Mcp3021Extension(ExtensionResources &resources,
                                   cfg::ExtensionMetadata metadata)
    : AdcExtensionBase(resources, metadata),
      I2cExtensionAdapter(resources, metadata, kMcp3021Address) {
}

void Mcp3021Extension::adcStartup() {
  DiagnosticId error;
  if (!connectToI2cBus(error)) {
    raiseFault(error);
    EXT_DEBUG("Failed to connect to I2C bus");
    return;
  }
  EXT_DEBUG("adcStartup() complete");
}

bool Mcp3021Extension::i2cHandshake() {
  uint32_t value;
  if (!readAdc(value)) {
    return false;
  }
  return true;
}

bool Mcp3021Extension::readAdc(uint32_t &value) {
  constexpr uint32_t kMcp3021ReadSize = 2;
  std::vector<uint8_t> read_buffer(kMcp3021ReadSize);
  auto result = i2c_.readChunk(read_buffer, kMcp3021ReadSize);

  if (result != I2cInterface::Result::SUCCESS) {
    EXT_DEBUG("Failed to read from I2C bus");
    return false;
  }

  // According to the data sheet, the first 4 bits of the upper data byte should
  // be 0
  if ((read_buffer.at(0) & 0b11110000) != 0b00000000) {
    EXT_DEBUG("Invalid data received from I2C bus");
    return false;
  }

  value = read_buffer.at(0) << 6;
  value |= read_buffer.at(1) >> 2;

  EXT_DEBUG("Read value: " << value);

  return true;
}

} // namespace extension

#ifdef notdef

#define MCP3021_ADDRESS 0x4D /** @todo REMOVE THESE */
#define VOLTAGE_REFERENCE 3.385
#define RESISTOR_1 30000
#define RESISTOR_2 7500

bool extension::MCP3021::ReadVoltage() {
  uint8_t data[2] = {0, 0};
  int result = i2c_bus_.readChunk(data, 2);

  if (result != 2) {
    return false;
  }

  if ((data[0] & 0b11110000) !=
      0b00000000) { // Per the datasheet, the first 4 bits of the
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

#endif