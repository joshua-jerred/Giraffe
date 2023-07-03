/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   i2c_interface.cpp
 * @brief  A wrapper around I2C communication.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-07-02
 * @copyright  2023 (license to be defined)
 */

#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

extern "C" {
#include <i2c/smbus.h>
#include <linux/i2c-dev.h>
}

#include "i2c_interface.hpp"

I2cInterface::I2cInterface(cfg::gEnum::I2CBus bus, uint8_t address,
                           std::mutex &bus_lock)
    : bus_(bus), address_(address), bus_lock_(bus_lock) {

  switch (bus) {
  case cfg::gEnum::I2CBus::I2C_1:
    file_name_ = "/dev/i2c-1";
    break;
  default:
    giraffe_assert(false);
  }
}

I2cInterface::~I2cInterface() {
  disconnect();
}

I2cInterface::Result I2cInterface::connect() {
  std::lock_guard<std::mutex> lock(bus_lock_);

  // Open the bus and check for errors
  // Possibly add a more specific check for error types
  i2c_fd_ = open(file_name_.c_str(), O_RDWR);
  if (i2c_fd_ < 0) {
    return Result::BUS_OPEN_ERROR;
  }

  // Configure the address and check for errors
  // SLAVE_FORCE incase a kernel driver is already using the device
  if (ioctl(i2c_fd_, I2C_SLAVE_FORCE, address_) < 0) {
    return Result::ADDRESS_SET_ERROR;
  }

  return Result::SUCCESS;
}

I2cInterface::Result I2cInterface::disconnect() {
  std::lock_guard<std::mutex> lock(bus_lock_);

  if (!isConnected()) { // Check if the bus is open
    return Result::FD_NOT_OPEN;
  }

  if (close(i2c_fd_) == 0) {
    return Result::SUCCESS;
  }

  return Result::DISCONNECT_CLOSE_ERROR;
}

uint8_t I2cInterface::getAddress() const {
  return address_;
}

I2cInterface::Result I2cInterface::writeByte(uint8_t data_byte) {
  std::lock_guard<std::mutex> lock(bus_lock_);

  if (!isConnected()) {
    return Result::FD_NOT_OPEN;
  }

  int32_t result = i2c_smbus_write_byte(i2c_fd_, data_byte);

  if (result < 0) {
    return Result::WRITE_ERROR;
  }

  return Result::SUCCESS;
}

I2cInterface::Result I2cInterface::writeByteToReg(uint8_t data_byte,
                                                  uint8_t register_address) {
  std::lock_guard<std::mutex> lock(bus_lock_);

  if (!isConnected()) {
    return Result::FD_NOT_OPEN;
  }

  int32_t result =
      i2c_smbus_write_byte_data(i2c_fd_, register_address, data_byte);

  if (result < 0) {
    return Result::WRITE_ERROR;
  }

  return Result::SUCCESS;
}

I2cInterface::Result I2cInterface::writeChunk(std::vector<uint8_t> &data) {
  std::lock_guard<std::mutex> lock(bus_lock_);

  if (!isConnected()) {
    return Result::FD_NOT_OPEN;
  }

  int length = data.size();

  // Passing an empty vector as a C array is undefined behavior.
  if (length == 0) {
    return Result::DATA_SIZE_ERROR;
  }

  int bytes_written = write(i2c_fd_, &data[0], length);

  if (bytes_written < 0) {
    return Result::WRITE_ERROR;
  } else if (bytes_written != length) {
    return Result::DATA_SIZE_ERROR;
  }

  return Result::SUCCESS;
}

I2cInterface::Result I2cInterface::readByte(uint8_t &data_byte) {
  std::lock_guard<std::mutex> lock(bus_lock_);

  if (!isConnected()) {
    return Result::FD_NOT_OPEN;
  }

  volatile int result = i2c_smbus_read_byte(i2c_fd_);

  if (result < 0) {
    return Result::READ_ERROR;
  }

  data_byte = static_cast<uint8_t>(result);
  return Result::SUCCESS;
}

I2cInterface::Result I2cInterface::readByteFromReg(uint8_t &data_byte,
                                                   uint8_t register_address) {
  std::lock_guard<std::mutex> lock(bus_lock_);

  if (!isConnected()) {
    return Result::FD_NOT_OPEN;
  }

  volatile int result = i2c_smbus_read_byte_data(i2c_fd_, register_address);

  if (result < 0) {
    return Result::READ_ERROR;
  }

  data_byte = static_cast<uint8_t>(result);
  return Result::SUCCESS;
}

I2cInterface::Result I2cInterface::readChunkFromReg(std::vector<uint8_t> &data,
                                                    uint8_t register_address,
                                                    size_t num_bytes) {
  std::lock_guard<std::mutex> lock(bus_lock_);

  if (!isConnected()) {
    return Result::FD_NOT_OPEN;
  }

  data.clear();
  data.resize(num_bytes);

  // write the register address
  int status = write(i2c_fd_, &register_address, 1);
  if (status < 0) {
    return Result::READ_ERROR_WRITE_REG;
  }

  // read the data
  int num_bytes_read = read(i2c_fd_, &data[0], num_bytes);
  if (num_bytes_read < 0) { // errorno
    return Result::READ_ERROR;
  } else if (static_cast<size_t>(num_bytes_read) != num_bytes) {
    return Result::READ_ERROR_NUM_BYTES;
  }

  return Result::SUCCESS;
}

I2cInterface::Result I2cInterface::readChunk(std::vector<uint8_t> &data,
                                             size_t num_bytes) {
  std::lock_guard<std::mutex> lock(bus_lock_);

  if (!isConnected()) {
    return Result::FD_NOT_OPEN;
  }

  int bytes_read = read(i2c_fd_, &data[0], num_bytes);

  if (bytes_read < 0) {
    return Result::READ_ERROR;
  } else if (static_cast<size_t>(bytes_read) != num_bytes) {
    return Result::READ_ERROR_NUM_BYTES;
  }

  return Result::SUCCESS;
}

bool I2cInterface::isConnected() const {
  return i2c_fd_ >= 0;
}