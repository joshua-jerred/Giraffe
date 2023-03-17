/**
 * @file extension-interface.h
 * @author Joshua Jerred (https://joshuajer.red/)
 * @brief Header for interfacing with extensions via I2C and 1-Wire.
 *
 * @version 0.3
 * @date 2023-01-06
 * @copyright Copyright (c) 2023
 *
 * @todo Add SPI and UART support
 */

#ifndef EXTENSION_INTERFACE_H_
#define EXTENSION_INTERFACE_H_

#include <mutex>
#include <string>

#include "configurables.h"

namespace extension_interface {

enum class I2C_STATUS {
  BUS_LOCK_CONFIG_ERROR,  // Bus lock pointer is null
  NOT_CONNECTED,
  OK,
  CONFIG_ERROR_BUS,
  CONFIG_ERROR_ADDRESS,
  BUS_ERROR,
  ADDRESS_ERROR,
  READ_ERROR,
  WRITE_ERROR,
  UNKNOWN_ERROR
};

enum class ONEWIRE_STATUS { OK, NOT_FOUND, READ_ERROR, UNKNOWN_ERROR };

class I2C {
 public:
  I2C(int bus_number, int address, std::mutex& bus_lock);
  ~I2C();

  int connect();
  int disconnect();

  I2C_STATUS status();

  uint8_t getAddress();

  int writeByte(uint8_t data);
  int writeByteToReg(uint8_t reg_address, uint8_t data);
  int writeChunk(uint8_t* data, uint8_t length);

  int readByte();
  int readByteFromReg(uint8_t reg_address);
  int readChunkFromReg(uint8_t* data, int length, uint8_t reg_address);
  int readChunk(uint8_t* data, int length);

 private:
  I2C_STATUS status_;

  int bus_number_;
  int address_;
  char file_name_[20];
  int i2c_fd_;
  std::mutex& bus_lock_;
};

class OneWire {
 public:
  OneWire(std::string device_id);
  ~OneWire();

  ONEWIRE_STATUS status();

  std::string read_w1_slave();
  std::string read_temperature();

 private:
  int checkDevice();
  std::string path_;  // path to the device directory
  ONEWIRE_STATUS status_;
};

class Serial {
 public:
  Serial(std::string device_path, int baud_rate);
  ~Serial();

  int Connect();
  int Disconnect();

  std::string Read();
  int Write(std::string data);

 private:
  std::string device_path_;
  int baud_rate_;
  int fd_;
  bool connected_;
};

}  // namespace extension_interface

#endif /* UTILITY_INTERFACE_CONTROL_H_ */