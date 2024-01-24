/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   gpio.hpp
 * @brief  GPIO interface header file
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-01-24
 * @copyright  2024 (license to be defined)
 */

#ifndef GPIO_INTERFACE_HPP_
#define GPIO_INTERFACE_HPP_

#include <array>
#include <cstdint>
#include <mutex>

namespace giraffe {

class Gpio {
public:
  enum class PinMode { INPUT, OUTPUT, UNINITIALIZED };

  struct Pin {
    Pin() = default;
    Pin(uint8_t pin_number, PinMode mode, bool default_state);

    uint8_t pin_number = 0;
    PinMode mode = PinMode::UNINITIALIZED;
    bool default_state = false;
  };

  static void initialize();
  static bool isInitialized();
  static void close();

  bool setupPin(Pin pin);
  void write(Pin pin, bool high);
  bool read(Pin pin);

private:
  static std::array<Pin, 28> reserved_pins_;

  /**
   * @brief This mutex is used to ensure that only one instance of Gpio can
   * access the BCM registers at a time along with static members.
   */
  static std::mutex gpio_lock_;
};

} // namespace giraffe

#endif /* GPIO_INTERFACE_HPP_ */