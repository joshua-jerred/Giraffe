/**
 * @file interface.h
 * @author Joshua Jerred (https://joshuajer.red/)
 * @brief Header for interfacing with extensions via I2C and 1-Wire.
 *
 * @version 0.3
 * @date 2023-01-06
 * @copyright Copyright (c) 2023
 *
 * @todo Add SPI
 * @todo Documentation
 * @todo Unit Tests
 */

#ifndef EXTENSION_INTERFACE_H_
#define EXTENSION_INTERFACE_H_

#include <exception>
#include <mutex>
#include <string>

#include "configurables.h"

namespace interface {

enum class I2C_STATUS {
  BUS_LOCK_CONFIG_ERROR, // Bus lock pointer is null
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
  I2C(int bus_number, int address, std::mutex &bus_lock);
  ~I2C();

  int connect();
  int disconnect();

  I2C_STATUS status();

  uint8_t getAddress();

  int writeByte(uint8_t data);
  int writeByteToReg(uint8_t reg_address, uint8_t data);
  int writeChunk(uint8_t *data, uint8_t length);

  int readByte();
  int readByteFromReg(uint8_t reg_address);
  int readChunkFromReg(uint8_t *data, int length, uint8_t reg_address);
  int readChunk(uint8_t *data, int length);

private:
  I2C_STATUS status_;

  int bus_number_;
  int address_;
  char file_name_[20];
  int i2c_fd_;
  std::mutex &bus_lock_;
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
  std::string path_; // path to the device directory
  ONEWIRE_STATUS status_;
};

class Serial {
public:
  enum class BaudRate { // To be expanded as needed
    BR9600 = 9600
  };
  Serial(const std::string device_path, const int baud_rate,
         const int timeout = 10);
  Serial(const std::string device_path, const Serial::BaudRate baud_rate,
         const int timeout = 10);
  ~Serial();

  void Connect();
  void Disconnect();
  bool IsConnected() const;

  std::string ReadLine();
  int Write(std::string data);

private:
  const std::string device_path_;
  const Serial::BaudRate baud_rate_;
  const int timeout_; // in tenths of a second

  int fd_ = -1;
  bool connected_ = false;
};

// interface::Exception
class SerialException : public std::exception {
public:
  enum class Type {
    OPEN = 0,
    GET_ATTRIBUTE = 1,
    SET_BAUD = 2,
    SET_ATTRIBUTE = 3,
    WRITE = 4,
    READ = 5,
    CLOSE = 6,
    NOT_CONNECTED = 7,
    ALREADY_CONNECTED = 8,
    UNKNOWN = 9,
    TIMEOUT = 10
  }; // To be expanded

  SerialException(SerialException::Type type, int errno_value,
                  std::string message)
      : type_(type), errno_value_(errno_value), message_(message) {
  }
  SerialException::Type type() const throw() {
    return type_;
  }
  int errno_value() const throw() {
    return errno_value_;
  }
  const char *what() const throw() {
    return message_.c_str();
  }

private:
  SerialException::Type type_;
  int errno_value_;
  std::string message_;
};

} // namespace interface

#endif /* UTILITY_INTERFACE_CONTROL_H_ */