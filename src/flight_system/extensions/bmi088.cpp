/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   bmi088.cpp
 * @brief  Implementation of the BMI088 extension.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-01-26
 * @copyright  2024 (license to be defined)
 */

#include "bmi088.hpp"

inline constexpr uint32_t HANDSHAKE_ATTEMPTS = 3;
inline constexpr uint32_t HANDSHAKE_RETRY_DELAY_MS = 100;
inline constexpr int TIMEOUT_MS = 500;

inline constexpr uint8_t BMI088_READ_FLAG = 0x80;
inline constexpr uint8_t BMI088_WRITE_FLAG = 0x00;

inline constexpr uint8_t BMI088_GYRO_CHIP_ID_ADDR = 0x00;
inline constexpr uint8_t BMI088_GYRO_EXP_CHIP_ID = 0x0F;
inline constexpr uint8_t BMI088_GYRO_RANGE_ADDR = 0x0F;
inline constexpr uint8_t BMI088_GYRO_RANGE_VAL = 0x00; // 2000 deg/s
inline constexpr double BMI088_GYRO_RANGE = 2000.0;
inline constexpr uint8_t BMI088_GYRO_BW_ADDR = 0x10;
inline constexpr uint8_t BMI088_GYRO_BW_VAL = 0x07; // 32 Hz
inline constexpr uint8_t BMI088_READ_GYRO_DATA_ADDR = 0x02;

inline constexpr uint8_t BMI088_ACCEL_CHIP_ID_ADDR = 0x00;
inline constexpr uint8_t BMI088_ACCEL_EXP_CHIP_ID = 0x1E;
inline constexpr uint8_t BMI088_ACC_PWR_CTRL_ADDR = 0x7D;
inline constexpr uint8_t BMI088_ACC_PWR_CTRL_VAL = 0x04;
inline constexpr uint8_t BMI088_ACC_PWR_CONF_ADDR = 0x7C;
inline constexpr uint8_t BMI088_ACC_PWR_CONF_VAL = 0x00;
inline constexpr uint8_t BMI088_READ_ACCEL_DATA_ADDR = 0x12;

const giraffe::SpiInterface::Config SPI_CONFIG{
    .bits_per_word = 8, .speed_hz = 10000, .delay_usecs = 3};

namespace extension {

// inline constexpr

Bmi088::Bmi088(ExtensionResources &resources, cfg::ExtensionMetadata metadata)
    : Extension(resources, metadata), spi_accel_(SPI_CONFIG),
      spi_gyro_(SPI_CONFIG),
      read_timeout_(metadata_.update_interval + TIMEOUT_MS) {
  std::string args = metadata_.extra_args;

  std::string delimiter = ",";
  size_t pos = 0;
  if ((pos = args.find(delimiter)) == std::string::npos) {
    raiseFault(DiagnosticId::EXT_FAULT_bmi088AddressError, args);
  }
  try {
    accel_address_ = args.substr(0, pos);
    gyro_address_ = args.substr(pos + delimiter.length(), args.length());
  } catch (std::exception &e) {
    raiseFault(DiagnosticId::EXT_FAULT_bmi088AddressError, "parse_fault");
  }
}

Bmi088::~Bmi088() {
  spi_accel_.closeDevice();
  spi_gyro_.closeDevice();
}

void Bmi088::startup() {
  if (!spi_accel_.openDevice(accel_address_)) {
    raiseFault(DiagnosticId::EXT_FAULT_bmi088AccelOpenError);
    return;
  }
  if (!spi_gyro_.openDevice(gyro_address_)) {
    raiseFault(DiagnosticId::EXT_FAULT_bmi088GyroOpenError);
    return;
  }

  while (!accelerometerHandshake()) {
    raiseFault(DiagnosticId::EXT_FAULT_bmi088AccelHandshakeError);
  }

  if (!gyroHandshake()) {
    raiseFault(DiagnosticId::EXT_FAULT_bmi088GyroHandshakeError);
  }

  if (!configureAccelerometer()) {
    raiseFault(DiagnosticId::EXT_FAULT_bmi088AccelConfigError);
  }
  if (!configureGyro()) {
    raiseFault(DiagnosticId::EXT_FAULT_bmi088GyroConfigError);
  }

  read_timeout_.reset();
}

void Bmi088::loop() {
  if (read_timeout_.isDone()) {
    raiseFault(DiagnosticId::EXT_FAULT_bmi088ReadTimeout);
  }

  if (readAccelData() && readGyroData()) {
    read_timeout_.reset();
    data(imu_data_);
  }
}

bool Bmi088::accelerometerHandshake() {
  constexpr size_t TRANSFER_SIZE = 2;
  uint32_t attempts = 0;

  while (attempts < HANDSHAKE_ATTEMPTS) {
    attempts++;

    uint8_t tx_buffer[TRANSFER_SIZE] = {
        BMI088_READ_FLAG | BMI088_ACCEL_CHIP_ID_ADDR, 0};
    uint8_t rx_buffer[TRANSFER_SIZE] = {0, 0};
    bool res = spi_accel_.transfer(tx_buffer, rx_buffer, TRANSFER_SIZE);

    if (res && rx_buffer[1] == BMI088_ACCEL_EXP_CHIP_ID) {
      return true;
    }

    extSleep(HANDSHAKE_RETRY_DELAY_MS);
  }

  return false;
}

bool Bmi088::gyroHandshake() {
  constexpr size_t TRANSFER_SIZE = 2;
  uint32_t attempts = 0;

  while (attempts < HANDSHAKE_ATTEMPTS) {
    attempts++;

    uint8_t tx_buffer[TRANSFER_SIZE] = {
        BMI088_READ_FLAG | BMI088_GYRO_CHIP_ID_ADDR, 0};
    uint8_t rx_buffer[TRANSFER_SIZE] = {0, 0};
    bool res = spi_gyro_.transfer(tx_buffer, rx_buffer, TRANSFER_SIZE);

    if (res && rx_buffer[1] == BMI088_GYRO_EXP_CHIP_ID) {
      return true;
    }

    extSleep(HANDSHAKE_RETRY_DELAY_MS);
  }

  return false;
}

bool Bmi088::configureAccelerometer() {
  constexpr size_t TRANSFER_SIZE = 2;
  uint8_t tx_buffer[TRANSFER_SIZE] = {0, 0};
  uint8_t rx_buffer[TRANSFER_SIZE] = {0, 0};

  tx_buffer[0] = BMI088_WRITE_FLAG | BMI088_ACC_PWR_CONF_ADDR;
  tx_buffer[1] = BMI088_ACC_PWR_CONF_VAL;
  rx_buffer[0] = 0;
  rx_buffer[1] = 0;

  if (!spi_accel_.transfer(tx_buffer, rx_buffer, TRANSFER_SIZE)) {
    return false;
  }

  tx_buffer[0] = BMI088_WRITE_FLAG | BMI088_ACC_PWR_CTRL_ADDR;
  tx_buffer[1] = BMI088_ACC_PWR_CTRL_VAL;
  rx_buffer[0] = 0;
  rx_buffer[1] = 0;

  if (!spi_accel_.transfer(tx_buffer, rx_buffer, TRANSFER_SIZE)) {
    return false;
  }

  return true;
}

bool Bmi088::configureGyro() {
  constexpr size_t TRANSFER_SIZE = 2;
  uint8_t tx_buffer[TRANSFER_SIZE] = {0, 0};
  uint8_t rx_buffer[TRANSFER_SIZE] = {0, 0};

  tx_buffer[0] = BMI088_WRITE_FLAG | BMI088_GYRO_RANGE_ADDR;
  tx_buffer[1] = BMI088_GYRO_RANGE_VAL;
  rx_buffer[0] = 0;
  rx_buffer[1] = 0;

  if (!spi_gyro_.transfer(tx_buffer, rx_buffer, TRANSFER_SIZE)) {
    return false;
  }

  tx_buffer[0] = BMI088_WRITE_FLAG | BMI088_GYRO_BW_ADDR;
  tx_buffer[1] = BMI088_GYRO_BW_VAL;
  rx_buffer[0] = 0;
  rx_buffer[1] = 0;

  if (!spi_gyro_.transfer(tx_buffer, rx_buffer, TRANSFER_SIZE)) {
    return false;
  }

  return true;
}

bool Bmi088::readGyroData() {
  constexpr size_t TRANSFER_SIZE = 7;
  uint8_t tx_buffer[TRANSFER_SIZE] = {
      BMI088_READ_FLAG | BMI088_READ_GYRO_DATA_ADDR, 0, 0, 0, 0, 0, 0};
  uint8_t rx_buffer[TRANSFER_SIZE] = {0, 0, 0, 0, 0, 0, 0};
  bool res = spi_gyro_.transfer(tx_buffer, rx_buffer, TRANSFER_SIZE);

  if (!res) {
    return false;
  }

  int16_t x_raw = (rx_buffer[2] << 8) | rx_buffer[1];
  int16_t y_raw = (rx_buffer[4] << 8) | rx_buffer[3];
  int16_t z_raw = (rx_buffer[6] << 8) | rx_buffer[5];

  imu_data_.x_angular_velocity = x_raw / BMI088_GYRO_RANGE;
  imu_data_.y_angular_velocity = y_raw / BMI088_GYRO_RANGE;
  imu_data_.z_angular_velocity = z_raw / BMI088_GYRO_RANGE;

  return true;
}

bool Bmi088::readAccelData() {
  constexpr size_t TRANSFER_SIZE = 7;
  uint8_t tx_buffer[TRANSFER_SIZE] = {
      BMI088_READ_FLAG | BMI088_READ_ACCEL_DATA_ADDR, 0, 0, 0, 0, 0, 0};
  uint8_t rx_buffer[TRANSFER_SIZE] = {0, 0, 0, 0, 0, 0, 0};
  bool res = spi_gyro_.transfer(tx_buffer, rx_buffer, TRANSFER_SIZE);

  if (!res) {
    return false;
  }

  int16_t x_raw = (rx_buffer[2] << 8) | rx_buffer[1];
  int16_t y_raw = (rx_buffer[4] << 8) | rx_buffer[3];
  int16_t z_raw = (rx_buffer[6] << 8) | rx_buffer[5];

  imu_data_.x_acceleration = x_raw;
  imu_data_.y_acceleration = y_raw;
  imu_data_.z_acceleration = z_raw;

  return true;
}

} // namespace extension