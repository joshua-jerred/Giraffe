/**
 * @file gpio.h
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief Low level control of Raspberry Pi BCM pins
 * @date 2023-03-18
 * @copyright Copyright (c) 2023
 * @version 0.4
 */

#ifndef GPIO_H
#define GPIO_H

#include <exception>
#include <string>

namespace gpio {
enum class PinMode { INPUT, OUTPUT };

struct Pin {
  const uint8_t bcm_pin_number;
  const PinMode mode;
  Pin(uint8_t bcm_pin_number, PinMode mode);
};

void Initialize();
void Close();

void SetupPin(Pin pin);
void Write(Pin pin, bool on);
bool Read(Pin pin);

class GpioException : public std::exception {
 public:
  GpioException(std::string message) : message_(message) {}
  const char* what() const throw() { return message_.c_str(); }

 private:
  std::string message_;
};

}  // namespace gpio
#endif