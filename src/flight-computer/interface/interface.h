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
  enum class BaudRate {  // To be expanded as needed
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
  const int timeout_;  // in tenths of a second

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
  };  // To be expanded

  SerialException(SerialException::Type type, int errno_value,
                  std::string message)
      : type_(type), errno_value_(errno_value), message_(message) {}
  SerialException::Type type() const throw() { return type_; }
  int errno_value() const throw() { return errno_value_; }
  const char* what() const throw() { return message_.c_str(); }

 private:
  SerialException::Type type_;
  int errno_value_;
  std::string message_;
};

/**
 * @brief This class is used to interface extensions with the BCM Gpio pins.
 * @details Currently this only supports the Raspberry Pi. It has not been
 * thoroughly tested.
 *
 * Detailed Documentation of the methods can be found in gpio.cpp
 *
 * @todo Unit Tests
 * @bug If Gpio::Close() is called, instances of Gpio may have weird behavior.
 */
class Gpio {
 public:
  /**
   * @brief PinMode is used to specify the mode of a pin, i.e. input or output.
   * @details UNINITIALIZED is only used internally, using it will result in a
   * GpioException.
   */
  enum class PinMode { INPUT, OUTPUT, UNINITIALIZED };

  /**
   * @brief Pin is used to safely pass pin information to the Gpio class.
   * @todo Make this struct immutable.
   */
  struct Pin {
    Pin(uint8_t bcm_pin_number, PinMode mode, bool initial_state = false);
    Pin()
        : pin_number_(-1),
          mode_(PinMode::UNINITIALIZED),
          initial_state_(false) {}

    std::string ToString() const;

    uint8_t pin_number_ = 0;
    PinMode mode_ = PinMode::UNINITIALIZED;
    bool initial_state_ = false;
  };

  static void Initialize();
  static void Close();

  static bool IsInitialized();

  void SetupPin(Pin pin);
  void Write(Pin pin, bool on);
  bool Read(Pin pin);

  /**
   * @brief GpioException is used to throw exceptions related to the Gpio class.
   * @details Currently this only contains a message.
   */
  class GpioException : public std::exception {
   public:
    GpioException(std::string message) : message_(message) {}
    const char* what() const throw() { return message_.c_str(); }

   private:
    std::string message_;
  };

 private:
  // gpio_lock_ must  be held before calling these functions
  bool VerifyInitialized();
  void ReservePin(Pin pin);
  bool IsPinReserved(const Pin& pin);

  bool IsOwner(const Pin& pin);
  void SetOwner(const Pin& pin);

  uint32_t ReadWithBarrier(volatile uint32_t* addr);
  void WriteWithBarrier(volatile uint32_t* addr, uint32_t value);

  volatile uint32_t* CalculateAddress(uint8_t pin_number, uint8_t offset);

  /**
   * @brief The Gpio memory map is used store the mapping to the BCM registers.
   * @details Initialized in Gpio::Initialize().
   */
  static volatile uint32_t* gpio_memory_map_;

  /**
   * @brief This array is used to keep track of which pins are reserved.
   * @details Initialized in Gpio::Initialize(), all pins are initially
   * set with mode PinMode::UNINITIALIZED and a pin number of -1.
   * 
   * @see Gpio::ReservePin()
   * @see Gpio::IsPinReserved()
   * @see Gpio::Close()
   */
  static std::array<Pin, 28> reserved_pins_;

  /**
   * @brief Contains the pins owned by a particular instance of Gpio.
   * @details This is a bitmask, where each bit corresponds to a pin.
   * 
   * @see Gpio::SetOwner()
   * @see Gpio::IsOwner()
   */
  uint32_t pins_owned_ = 0;  // Bitmask of pins owned by this instance

  /**
   * @brief This mutex is used to ensure that only one instance of Gpio can
   * access the BCM registers at a time along with static members.
   */
  static std::mutex gpio_lock_;

  /**
   * @brief Size of the memory map, one page.
   * @todo I need to find a source for this.
   */
  static const uint32_t kGpioMemoryMapSize = 0x1000;
};

}  // namespace interface

#endif /* UTILITY_INTERFACE_CONTROL_H_ */