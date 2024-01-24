/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   gpio.cpp
 * @brief  GPIO interface source file
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-01-24
 * @copyright  2024 (license to be defined)
 */

#include "gpio.hpp"
#include "giraffe_exception.hpp"

namespace giraffe {

inline constexpr uint8_t MAX_PIN_NUMBER = 27;

Gpio::Pin::Pin(uint8_t pin_number, PinMode mode, bool default_state)
    : pin_number(pin_number), mode(mode), default_state(default_state) {
  if (pin_number > MAX_PIN_NUMBER) {
    throw GiraffeException(DiagnosticId::INTERFACE_invalidGpioPin,
                           std::to_string(pin_number));
  }
  if (mode == PinMode::UNINITIALIZED) {
    throw GiraffeException(DiagnosticId::INTERFACE_invalidGpioPinMode,
                           std::to_string(pin_number));
  }
  if (mode == PinMode::INPUT && default_state) {
    throw GiraffeException(DiagnosticId::INTERFACE_initialStateInputPin,
                           std::to_string(pin_number));
  }
}

}; // namespace giraffe