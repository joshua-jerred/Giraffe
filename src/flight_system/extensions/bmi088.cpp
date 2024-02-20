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

#include <BoosterSeat/science.hpp>

#include "bmi088.hpp"

const giraffe::SpiInterface::Config SPI_CONFIG{
    .bits_per_word = 8, .speed_hz = 10000, .delay_usecs = 1};

namespace extension {

inline constexpr uint32_t HANDSHAKE_ATTEMPTS = 3;
inline constexpr uint32_t HANDSHAKE_RETRY_DELAY_MS = 100;
// inline constexpr int TIMEOUT_MS = 500;

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

inline constexpr Bmi088::AccOutputDataRate K_BMI088_ACC_ODR = Bmi088::ODR_100;
inline constexpr Bmi088::AccBwp K_BMI088_ACC_BWP = Bmi088::BWP_NORMAL;
inline constexpr Bmi088::AccRange K_BMI088_ACC_RANGE = Bmi088::RANGE_12G;

// inline constexpr

Bmi088::Bmi088(ExtensionResources &resources, cfg::ExtensionMetadata metadata)
    : ImuExtensionBase(resources, metadata), spi_accel_(SPI_CONFIG),
      spi_gyro_(SPI_CONFIG) {
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

void Bmi088::imuStartup() {
  // open the SPI devices
  if (!spi_accel_.openDevice(accel_address_)) {
    raiseFault(DiagnosticId::EXT_FAULT_bmi088AccelOpenError);
    return;
  }
  if (!spi_gyro_.openDevice(gyro_address_)) {
    raiseFault(DiagnosticId::EXT_FAULT_bmi088GyroOpenError);
    return;
  }

  // verify chip IDs
  if (!accHandshake()) {
    raiseFault(DiagnosticId::EXT_FAULT_bmi088AccelHandshakeError);
  }
  if (!gyroHandshake()) {
    raiseFault(DiagnosticId::EXT_FAULT_bmi088GyroHandshakeError);
  }

  // configure the IMU
  if (!configureAccelerometer()) {
    raiseFault(DiagnosticId::EXT_FAULT_bmi088AccelConfigError);
  }
  if (!configureGyro()) {
    raiseFault(DiagnosticId::EXT_FAULT_bmi088GyroConfigError);
  }
}

bool Bmi088::readImu() {
  if (readAccelData() && readGyroData()) {
    return true;
  }
  return false;
}

bool Bmi088::accHandshake() {
  constexpr uint8_t BMI088_ACCEL_CHIP_ID_ADDR = 0x00;
  constexpr uint8_t BMI088_ACCEL_EXP_CHIP_ID = 0x1E;

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

void Bmi088::accSoftReset() {
  constexpr uint8_t BMI088_SOFT_RESET_ADDR = 0x7E;
  constexpr uint8_t BMI088_SOFT_RESET_VAL = 0xB6;
  accWriteByte(BMI088_SOFT_RESET_ADDR, BMI088_SOFT_RESET_VAL);
}

void Bmi088::accSetRange(AccRange range) {
  constexpr uint8_t BMI088_ACC_RANGE_ADDR = 0x41;
  accWriteByte(BMI088_ACC_RANGE_ADDR, static_cast<uint8_t>(range));
  acc_range_ = range;
  switch (acc_range_) {
    // see section 5.3.4 of the BMI088 datasheet
  case AccRange::RANGE_3G:
    acc_range_value_ = 3000;
    // acc_range_value_ = 2000; // 1000 * 2^(0x00 + 1)
    break;
  case AccRange::RANGE_6G:
    acc_range_value_ = 6000; // 1000 * 2^(0x01 + 1)
    // acc_range_value_ = 4000; // 1000 * 2^(0x01 + 1)
    break;
  case AccRange::RANGE_12G:
    acc_range_value_ = 12000; // 1000 * 2^(0x02 + 1)
    // acc_range_value_ = 8000; // 1000 * 2^(0x02 + 1)
    break;
  case AccRange::RANGE_24G:
    acc_range_value_ = 24000; // 1000 * 2^(0x03 + 1)
    // acc_range_value_ = 16000; // 1000 * 2^(0x03 + 1)
    break;
  default:
    acc_range_ = AccRange::RANGE_3G;
    acc_range_value_ = 3000;
    break;
  }
}

void Bmi088::accSetConf(AccOutputDataRate odr, AccBwp bwp) {
  constexpr uint8_t BMI088_ACC_CONF_ADDR = 0x40;
  accWriteByte(BMI088_ACC_CONF_ADDR,
               static_cast<uint8_t>(odr) | static_cast<uint8_t>(bwp));
}

void Bmi088::accSetPower(bool on) {
  constexpr uint8_t BMI088_ACC_PWR_CTRL_ADDR = 0x7D;
  accWriteByte(BMI088_ACC_PWR_CTRL_ADDR,
               on ? static_cast<uint8_t>(AccPowerCtrl::ACC_ON)
                  : static_cast<uint8_t>(AccPowerCtrl::ACC_OFF));

  constexpr uint8_t BMI088_ACC_PWR_CONF_ADDR = 0x7C;
  accWriteByte(BMI088_ACC_PWR_CONF_ADDR,
               static_cast<uint8_t>(AccPowerConf::ACC_ACTIVE_MODE));
}

bool Bmi088::configureAccelerometer() {
  accSoftReset();
  extSleep(5); // datasheet says to wait 1 ms, being overly cautious

  if (!accHandshake()) {
    return false;
  }

  accSetRange(K_BMI088_ACC_RANGE);
  accSetConf(K_BMI088_ACC_ODR, K_BMI088_ACC_BWP);
  accSetPower(true);

  return true;
}

bool Bmi088::readAccelData() {
  constexpr uint8_t ACC_X_LSB = 0x12;

  constexpr size_t TRANSFER_SIZE = 7;
  uint8_t tx_buffer[TRANSFER_SIZE] = {
      BMI088_READ_FLAG | ACC_X_LSB, 0, 0, 0, 0, 0, 0};
  uint8_t rx_buffer[TRANSFER_SIZE] = {0, 0, 0, 0, 0, 0, 0};
  bool res = spi_accel_.transfer(tx_buffer, rx_buffer, TRANSFER_SIZE);

  if (!res) {
    return false;
  }

  // in units of LSB
  int16_t x_raw = (rx_buffer[2] << 8) | rx_buffer[1];
  int16_t y_raw = (rx_buffer[4] << 8) | rx_buffer[3];
  int16_t z_raw = (rx_buffer[6] << 8) | rx_buffer[5];

  // convert to units of mg
  constexpr double K_INT16_DIVISOR = 32768.0;
  double x = static_cast<double>(x_raw) / K_INT16_DIVISOR * acc_range_value_ *
             bst::K_GRAVITY * 0.001;
  double y = static_cast<double>(y_raw) / K_INT16_DIVISOR * acc_range_value_ *
             bst::K_GRAVITY * 0.001;
  double z = static_cast<double>(z_raw) / K_INT16_DIVISOR * acc_range_value_ *
             bst::K_GRAVITY * 0.001;
  // x /= 1000.0 * bst::K_GRAVITY;
  // y /= 1000.0 * bst::K_GRAVITY;
  // z /= 1000.0 * bst::K_GRAVITY;

  std::cout << "acc_range_value_: " << acc_range_value_ << "\n";
  std::cout << "x: " << x << " y: " << y << " z: " << z << std::endl;
  std::cout << "x2: " << acc_range_value_ * x_raw / 32768000 * bst::K_GRAVITY
            << " y2: " << acc_range_value_ * y_raw / 32768000 * bst::K_GRAVITY
            << " z2: " << acc_range_value_ * z_raw / 32768000 * bst::K_GRAVITY
            << std::endl;

  reportAccelData(static_cast<double>(x_raw), static_cast<double>(y_raw),
                  static_cast<double>(z_raw));

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

  reportGyroData(static_cast<double>(x_raw) / BMI088_GYRO_RANGE,
                 static_cast<double>(y_raw) / BMI088_GYRO_RANGE,
                 static_cast<double>(z_raw) / BMI088_GYRO_RANGE);
  return true;
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

bool Bmi088::accWriteByte(uint8_t register_addr, uint8_t value) {
  constexpr size_t TRANSFER_SIZE = 2;
  uint8_t tx_buffer[TRANSFER_SIZE] = {0, 0};
  uint8_t rx_buffer[TRANSFER_SIZE] = {0, 0};

  tx_buffer[0] = BMI088_WRITE_FLAG | register_addr;
  tx_buffer[1] = value;
  rx_buffer[0] = 0;
  rx_buffer[1] = 0;

  if (!spi_gyro_.transfer(tx_buffer, rx_buffer, TRANSFER_SIZE)) {
    return false;
  }
  return true;
}

bool Bmi088::gyroWriteByte(uint8_t register_addr, uint8_t value) {
  constexpr size_t TRANSFER_SIZE = 2;
  uint8_t tx_buffer[TRANSFER_SIZE] = {0, 0};
  uint8_t rx_buffer[TRANSFER_SIZE] = {0, 0};

  tx_buffer[0] = BMI088_WRITE_FLAG | register_addr;
  tx_buffer[1] = value;
  rx_buffer[0] = 0;
  rx_buffer[1] = 0;

  if (!spi_gyro_.transfer(tx_buffer, rx_buffer, TRANSFER_SIZE)) {
    return false;
  }
  return true;
}

} // namespace extension