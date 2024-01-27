#include <fcntl.h>
#include <linux/spi/spidev.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <time.h>
#include <unistd.h>

#include <array>
#include <iostream>
#include <string>
#include <thread>

inline constexpr uint8_t SPI_BITS_PER_WORD = 8;
inline constexpr uint32_t SPI_SPEED_HZ = 100000;
inline constexpr uint16_t SPI_DELAY_USECS = 0;

inline constexpr uint16_t SPI_MAX_TRANSFER_SIZE = 10;

inline constexpr uint8_t BMI088_READ_FLAG = 0x80;

inline constexpr uint8_t BMI088_GYRO_CHIP_ID_ADDR = 0x00;
inline constexpr uint8_t BMI088_GYRO_EXP_CHIP_ID = 0x0F;
inline constexpr uint8_t BMI088_READ_GYRO_DATA_ADDR = 0x02;

inline constexpr uint8_t BMI088_ACCEL_CHIP_ID_ADDR = 0x00;
inline constexpr uint8_t BMI088_ACCEL_EXP_CHIP_ID = 0x1E;
inline constexpr uint8_t BMI088_READ_ACCEL_DATA_ADDR = 0x12;

class Bmi088 {
public:
  Bmi088(const std::string &gyro_address, const std::string &accel_address)
      : gyro_address_(gyro_address), accel_address_(accel_address) {
  }

  ~Bmi088() {
  }

  bool openDevice() {
    if ((spi_gyro_fd_ = open(gyro_address_.c_str(), O_RDWR)) < 0) {
      std::cout << "Error opening " << gyro_address_ << std::endl;
      return false;
    }
    std::cout << "Successfully opened " << gyro_address_ << std::endl;
    if ((spi_accel_fd_ = open(accel_address_.c_str(), O_RDWR)) < 0) {
      std::cout << "Error opening " << accel_address_ << std::endl;
      return false;
    }
    std::cout << "Successfully opened " << accel_address_ << std::endl;
    return true;
  }

  void closeDevice() {
    if (spi_gyro_fd_ != -1) {
      close(spi_gyro_fd_);
      spi_gyro_fd_ = -1;
    }
    if (spi_accel_fd_ != -1) {
      close(spi_accel_fd_);
      spi_accel_fd_ = -1;
    }
  }

  bool handshakeGyro() {
    constexpr uint8_t TRANSFER_SIZE = 2;
    prepareSpiTransfer(TRANSFER_SIZE);
    tx_buffer_[0] = BMI088_READ_FLAG | BMI088_GYRO_CHIP_ID_ADDR;

    bool transfer_success =
        ioctl(spi_gyro_fd_, SPI_IOC_MESSAGE(1), &spi_ioc_transfer_) >= 0;

    return transfer_success && rx_buffer_[1] == BMI088_GYRO_EXP_CHIP_ID;
  }

  bool handshakeAccel() {
    constexpr uint8_t TRANSFER_SIZE = 2;
    prepareSpiTransfer(TRANSFER_SIZE);
    tx_buffer_[0] = BMI088_READ_FLAG | BMI088_ACCEL_CHIP_ID_ADDR;

    bool transfer_success =
        ioctl(spi_accel_fd_, SPI_IOC_MESSAGE(1), &spi_ioc_transfer_) >= 0;

    std::cout << "Accel: " << std::hex << (int)rx_buffer_[0] << std::endl;
    std::cout << std::hex << (int)rx_buffer_[1] << std::endl;

    return transfer_success && rx_buffer_[1] == BMI088_ACCEL_EXP_CHIP_ID;
  }

  bool readGyroData() {
    /**
     * @brief Read 7 bytes. The first byte is a "dummy byte", the next 6 bytes
     * are the actual gyro data.
     */
    constexpr uint8_t TRANSFER_SIZE = 7;
    prepareSpiTransfer(TRANSFER_SIZE);
    tx_buffer_[0] = BMI088_READ_FLAG | BMI088_READ_GYRO_DATA_ADDR;

    bool success =
        ioctl(spi_gyro_fd_, SPI_IOC_MESSAGE(1), &spi_ioc_transfer_) >= 0;

    gyro_x_ = (rx_buffer_[2] << 8) | rx_buffer_[1];
    gyro_y_ = (rx_buffer_[4] << 8) | rx_buffer_[3];
    gyro_z_ = (rx_buffer_[6] << 8) | rx_buffer_[5];

    std::cout << "Gyro: " << gyro_x_ << " " << gyro_y_ << " " << gyro_z_
              << std::endl;

    return success;
  }

  bool readAccelData() {
    /// @brief 1 dummy byte + 6 bytes of data
    constexpr uint8_t TRANSFER_SIZE = 7;
    prepareSpiTransfer(TRANSFER_SIZE);
    tx_buffer_[0] = BMI088_READ_FLAG | BMI088_READ_ACCEL_DATA_ADDR;

    bool success =
        ioctl(spi_accel_fd_, SPI_IOC_MESSAGE(1), &spi_ioc_transfer_) >= 0;

    accel_x_ = (rx_buffer_[2] << 8) | rx_buffer_[1];
    accel_y_ = (rx_buffer_[4] << 8) | rx_buffer_[3];
    accel_z_ = (rx_buffer_[6] << 8) | rx_buffer_[5];

    std::cout << "Accel: " << accel_x_ << " " << accel_y_ << " " << accel_z_
              << std::endl;

    return success;
  }

private:
  void prepareSpiTransfer(uint8_t transfer_size) {
    for (int i = 0; i < SPI_MAX_TRANSFER_SIZE; i++) {
      rx_buffer_[i] = 0;
      tx_buffer_[i] = 0;
    }
    memset(&spi_ioc_transfer_, 0, sizeof(spi_ioc_transfer_));
    spi_ioc_transfer_.rx_buf = (unsigned long)rx_buffer_;
    spi_ioc_transfer_.tx_buf = (unsigned long)tx_buffer_;
    spi_ioc_transfer_.len = transfer_size;
    spi_ioc_transfer_.delay_usecs = SPI_DELAY_USECS;
    spi_ioc_transfer_.speed_hz = SPI_SPEED_HZ;
    spi_ioc_transfer_.bits_per_word = SPI_BITS_PER_WORD;
  }

  const std::string gyro_address_;
  int spi_gyro_fd_ = -1;

  const std::string accel_address_;
  int spi_accel_fd_ = -1;

  spi_ioc_transfer spi_ioc_transfer_{};

  uint8_t rx_buffer_[SPI_MAX_TRANSFER_SIZE] = {0};
  uint8_t tx_buffer_[SPI_MAX_TRANSFER_SIZE] = {0};

  int16_t gyro_x_ = 0;
  int16_t gyro_y_ = 0;
  int16_t gyro_z_ = 0;

  int16_t accel_x_ = 0;
  int16_t accel_y_ = 0;
  int16_t accel_z_ = 0;
};

int main() {
  Bmi088 spi_device("/dev/spidev0.1", "/dev/spidev0.0");
  spi_device.openDevice();

  // if (spi_device.handshakeGyro()) {
  // std::cout << "Gyro handshake successful" << std::endl;
  // } else {
  // std::cout << "Gyro handshake failed" << std::endl;
  // }

  if (spi_device.handshakeAccel()) {
    std::cout << "Accel handshake successful" << std::endl;
  } else {
    std::cout << "Accel handshake failed" << std::endl;
  }

  // while (1) {
  // spi_device.readGyroData();
  // spi_device.readAccelData();
  // std::this_thread::sleep_for(std::chrono::milliseconds(100));
  // }

  return 0;
}
