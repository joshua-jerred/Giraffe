/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   bme280.cpp
 * @brief  BME280 sensor extension
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-07-02
 * @copyright  2023 (license to be defined)
 */

#include "bme280.hpp"

namespace extension {

inline constexpr uint8_t kBme280I2cAddress = 0x76;
inline constexpr int kCompensationDataReadRate = 5000; // ms
inline constexpr int K_READ_TIMEOUT = 5000;            // ms

Bme280Extension::Bme280Extension(ExtensionResources &resources,
                                 cfg::ExtensionMetadata metadata)
    : Extension(resources, metadata),
      i2c_(resources.i2c_bus, kBme280I2cAddress, resources.i2c_bus_lock),
      compensation_timer_(kCompensationDataReadRate),
      read_timer_(K_READ_TIMEOUT + metadata.update_interval) {
}

enum class StartupState {
  I2C_CONNECT,
  HANDSHAKE,
  CONFIGURE,
  READ_COMPENSATION_DATA,
  DONE
};

void Bme280Extension::startup() {
  constexpr uint32_t kStartupTimeout = 5000;   // ms
  constexpr uint32_t kStartupRetryDelay = 100; // ms

  BoosterSeat::Timer startup_timer(kStartupTimeout);
  startup_timer.reset();

  StartupState state = StartupState::I2C_CONNECT;

  I2cInterface::Result connect_result;

  while (!startup_timer.isDone()) {
    switch (state) {
    case StartupState::I2C_CONNECT:
      connect_result = i2c_.connect();
      if (connect_result == I2cInterface::Result::SUCCESS) {
        state = StartupState::HANDSHAKE;
        break;
      }
      extSleep(kStartupRetryDelay);
      break;
    case StartupState::HANDSHAKE:
      if (handshake()) {
        state = StartupState::CONFIGURE;
        break;
      }
      extSleep(kStartupRetryDelay);
      break;
    case StartupState::CONFIGURE:
      if (configureSensor()) {
        state = StartupState::READ_COMPENSATION_DATA;
        break;
      }
      extSleep(kStartupRetryDelay);
      break;
    case StartupState::READ_COMPENSATION_DATA:
      if (readCompensationData()) {
        state = StartupState::DONE;
        break;
      }
      extSleep(kStartupRetryDelay);
      break;
    case StartupState::DONE: // Return, startup is complete
      return;
    }

    if (stopRequested()) { // A stop was requested during startup
      return;
    }
  }

  // Startup timed out
  switch (state) {
  case StartupState::I2C_CONNECT:
    raiseFault(DiagnosticId::EXT_FAULT_bme280StartupI2CConnect);
    break;
  case StartupState::HANDSHAKE:
    raiseFault(DiagnosticId::EXT_FAULT_bme280StartupHandshake);
    break;
  case StartupState::CONFIGURE:
    raiseFault(DiagnosticId::EXT_FAULT_bme280StartupConfigure);
    break;
  case StartupState::READ_COMPENSATION_DATA:
    raiseFault(DiagnosticId::EXT_FAULT_bme280StartupReadCompensation);
    break;
  case StartupState::DONE:
    return;
  }
}

void Bme280Extension::loop() {
  if (read_timer_.isDone()) {
    raiseFault(DiagnosticId::EXT_FAULT_bme280ReadTimeout);
    return;
  }

  if (compensation_timer_.isDone()) {
    if (!readCompensationData()) {
      return;
    } else {
      compensation_timer_.reset();
    }
  }

  if (!readEnvironmentalData()) {
    return;
  }
  if (!processEnvironmentalData()) {
    return;
  }

  read_timer_.reset();

  constexpr double kMinTemperature = -40.0; // C
  constexpr double kMaxTemperature = 85.0;
  constexpr double kMinPressure = 300.0; // hPa
  constexpr double kMaxPressure = 1100.0;
  constexpr double kMinHumidity = 0.0; // rh%
  constexpr double kMaxHumidity = 100.0;

  if (temperature_ < kMinTemperature || temperature_ > kMaxTemperature) {
    error(DiagnosticId::EXTENSION_bme280TemperatureRange);
  }
  if (pressure_ < kMinPressure || pressure_ > kMaxPressure) {
    error(DiagnosticId::EXTENSION_bme280PressureRange);
  }
  if (humidity_ < kMinHumidity || humidity_ > kMaxHumidity) {
    error(DiagnosticId::EXTENSION_bme280HumidityRange);
  }

  data(data::DataId::ENVIRONMENTAL_temperature, temperature_);
  data(data::DataId::ENVIRONMENTAL_pressure, pressure_);
  data(data::DataId::ENVIRONMENTAL_humidity, humidity_);
}

bool Bme280Extension::handshake() {
  constexpr uint8_t K_ID_REGISTER = 0xD0;
  constexpr uint8_t K_EXPECTED_ID = 0x60;

  uint8_t result;
  if (i2c_.readByteFromReg(result, K_ID_REGISTER) !=
      I2cInterface::Result::SUCCESS) {
    return false;
  }

  if (result != K_EXPECTED_ID) {
    return false;
  }

  return true;
}

bool Bme280Extension::configureSensor() {
  constexpr uint8_t kRegCtrlHum =
      0xF2; // Humidity oversampling (Written to before Ctrl_Meas)
  constexpr uint8_t kRegCtrlMeas = 0xF4; // Temp and pressure oversampling
  constexpr uint8_t kRegConfig = 0xF5;   // Rate, filter, interface

  // The following can be changed
  constexpr uint8_t kCtrlHumSetting = 0x01; // 1x oversampling
  constexpr uint8_t kTempAndPressureSettings =
      0b00100111; // 1x oversampling for temp and pressure, normal mode
  constexpr uint8_t kConfigSettings = 0b01000000; // 125ms standby time

  if (i2c_.writeByteToReg(kCtrlHumSetting, kRegCtrlHum) !=
      I2cInterface::Result::SUCCESS) {
    return false;
  }

  if (i2c_.writeByteToReg(kTempAndPressureSettings, kRegCtrlMeas) !=
      I2cInterface::Result::SUCCESS) {
    return false;
  }

  if (i2c_.writeByteToReg(kConfigSettings, kRegConfig) !=
      I2cInterface::Result::SUCCESS) {
    return false;
  }

  return true;
}

bool Bme280Extension::readCompensationData() {
  constexpr uint8_t K_CALIBRATION_DATA_HEAD_SEC1 =
      0x88; // Head of calibration data
  constexpr uint8_t K_CALIBRATION_DATA_LENGTH_SEC1 =
      26; // Length of calibration data (section 1)

  constexpr uint8_t K_CALIBRATION_DATA_HEAD_SEC2 =
      0xE1; // Head of calibration data
  constexpr uint8_t K_CALIBRATION_DATA_LENGTH_SEC2 =
      7; // Length of calibration data (section 2)

  std::vector<uint8_t> data_section1(K_CALIBRATION_DATA_LENGTH_SEC1, 0);
  std::vector<uint8_t> data_section2(K_CALIBRATION_DATA_LENGTH_SEC2, 0);

  if (i2c_.readChunkFromReg(data_section1, K_CALIBRATION_DATA_HEAD_SEC1,
                            K_CALIBRATION_DATA_LENGTH_SEC1) !=
      I2cInterface::Result::SUCCESS) {
    return false;
  }

  if (i2c_.readChunkFromReg(data_section2, K_CALIBRATION_DATA_HEAD_SEC2,
                            K_CALIBRATION_DATA_LENGTH_SEC2) !=
      I2cInterface::Result::SUCCESS) {
    return false;
  }

  comp_data_.dig_T1 =
      static_cast<uint16_t>((data_section1.at(1) << 8) | data_section1.at(0));
  comp_data_.dig_T2 = (data_section1.at(3) << 8) | data_section1.at(2);
  comp_data_.dig_T3 = (data_section1.at(5) << 8) | data_section1.at(4);

  comp_data_.dig_P1 = (data_section1.at(7) << 8) | data_section1.at(6);
  comp_data_.dig_P2 = (data_section1.at(9) << 8) | data_section1.at(8);
  comp_data_.dig_P3 = (data_section1.at(11) << 8) | data_section1.at(10);
  comp_data_.dig_P4 = (data_section1.at(13) << 8) | data_section1.at(12);
  comp_data_.dig_P5 = (data_section1.at(15) << 8) | data_section1.at(14);
  comp_data_.dig_P6 = (data_section1.at(17) << 8) | data_section1.at(16);
  comp_data_.dig_P7 = (data_section1.at(19) << 8) | data_section1.at(18);
  comp_data_.dig_P8 = (data_section1.at(21) << 8) | data_section1.at(20);
  comp_data_.dig_P9 = (data_section1.at(23) << 8) | data_section1.at(22);

  comp_data_.dig_H1 = data_section1[25];
  comp_data_.dig_H2 = (data_section2[1] << 8) | data_section2[0];
  comp_data_.dig_H3 = data_section2[2];
  comp_data_.dig_H4 = (data_section2[3] << 4) | (0x0F & data_section2[4]);
  comp_data_.dig_H5 =
      (data_section2[5] << 4) | ((data_section2[4] >> 4) & 0x0F);

  return true;
}

bool Bme280Extension::readEnvironmentalData() {
  constexpr uint8_t K_DATA_REGISTER_HEAD = 0xF7;
  constexpr size_t K_DATA_LENGTH = 8;

  std::vector<uint8_t> data(K_DATA_LENGTH, 0);

  if (i2c_.readChunkFromReg(data, K_DATA_REGISTER_HEAD, K_DATA_LENGTH) !=
      I2cInterface::Result::SUCCESS) {
    return false;
  }

  raw_pressure_ = (data[0] << 12) | (data[1] << 4) | (data[2] >> 4);
  raw_temperature_ = (data[3] << 12) | (data[4] << 4) | (data[5] >> 4);
  raw_humidity_ = (data[6] << 8) | data[7];

  return true;
}

bool Bme280Extension::processEnvironmentalData() {
  // Temperature (See section 4.2.3 of the datasheet)
  int32_t tvar1;
  int32_t tvar2;
  int32_t raw_temperature;
  tvar1 = ((((raw_temperature_ >> 3) - ((int32_t)comp_data_.dig_T1 << 1))) *
           ((int32_t)comp_data_.dig_T2)) >>
          11;
  tvar2 = (((((raw_temperature_ >> 4) - ((int32_t)comp_data_.dig_T1)) *
             ((raw_temperature_ >> 4) - ((int32_t)comp_data_.dig_T1))) >>
            12) *
           ((int32_t)comp_data_.dig_T3)) >>
          14;
  int32_t t_fine = tvar1 + tvar2;
  raw_temperature = (t_fine * 5 + 128) >> 8;
  temperature_ = (float)raw_temperature / 100.0;

  // Pressure (See section 4.2.3 of the datasheet)
  int64_t pvar1;
  int64_t pvar2;
  int64_t p;
  pvar1 = ((int64_t)t_fine) - 128000;
  pvar2 = pvar1 * pvar1 * (int64_t)comp_data_.dig_P6;
  pvar2 = pvar2 + ((pvar1 * (int64_t)comp_data_.dig_P5) << 17);
  pvar2 = pvar2 + (((int64_t)comp_data_.dig_P4) << 35);
  pvar1 = ((pvar1 * pvar1 * (int64_t)comp_data_.dig_P3) >> 8) +
          ((pvar1 * (int64_t)comp_data_.dig_P2) << 12);
  pvar1 = (((((int64_t)1) << 47) + pvar1)) * ((int64_t)comp_data_.dig_P1) >> 33;
  if (pvar1 == 0) {
    error(DiagnosticId::EXTENSION_bme280Div0);
    return false;
  }
  p = 1048576 - raw_pressure_;
  p = (((p << 31) - pvar2) * 3125) / pvar1;
  pvar1 = (((int64_t)comp_data_.dig_P9) * (p >> 13) * (p >> 13)) >> 25;
  pvar2 = (((int64_t)comp_data_.dig_P8) * p) >> 19;
  p = ((p + pvar1 + pvar2) >> 8) + (((int64_t)comp_data_.dig_P7) << 4);
  pressure_ = ((float)p / 256.0) / 100.0;

  // Humidity (See section 4.2.3 of the datasheet)
  int32_t h;
  h = (t_fine - ((int32_t)76800));
  h = (((((raw_humidity_ << 14) - (((int32_t)comp_data_.dig_H4) << 20) -
          (((int32_t)comp_data_.dig_H5) * h)) +
         ((int32_t)16384)) >>
        15) *
       (((((((h * ((int32_t)comp_data_.dig_H6)) >> 10) *
            (((h * ((int32_t)comp_data_.dig_H3)) >> 11) + ((int32_t)32768))) >>
           10) +
          ((int32_t)2097152)) *
             ((int32_t)comp_data_.dig_H2) +
         8192) >>
        14));
  h = (h -
       (((((h >> 15) * (h >> 15)) >> 7) * ((int32_t)comp_data_.dig_H1)) >> 4));
  h = (h < 0) ? 0 : h;
  h = (h > 419430400) ? 419430400 : h;
  humidity_ = (float)(h >> 12) / 1024.0;

  return true;
}

void Bme280Extension::resetSensor() {
  const uint8_t K_REGISTER_RESET = 0xE0;
  const uint8_t K_RESET_COMMAND = 0xB6;
  i2c_.writeByteToReg(K_RESET_COMMAND, K_REGISTER_RESET);
}

} // namespace extension