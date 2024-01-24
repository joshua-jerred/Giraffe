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

/**
 * @brief This class is used to interface extensions with the BCM Gpio pins.
 * @details Currently this only supports the Raspberry Pi. It has not been
 * thoroughly tested.
 * @todo Unit Tests
 * @bug If Gpio::Close() is called, instances of Gpio may have weird behavior.
 */
class Gpio {
public:
  /// @brief The pin modes. Uninitialized is only used internally.
  enum class PinMode { INPUT, OUTPUT, UNINITIALIZED };

  /// @brief  A pin struct that contains the pin number, mode, and default
  struct Pin {
    Pin() = default;

    /**
     * @brief Create a new pin struct.
     * @param pin_number - The pin number. (BCM Numbering)
     * @param mode - The pin mode.
     * @param default_state - *Optional* Used only for OUTPUT pins, sets a
     * failsafe state so if close is called when the program is terminated, the
     * pin will be set to the this state. True for HIGH, false for LOW. Defaults
     * to LOW.
     */
    Pin(uint8_t pin_number, PinMode mode, bool default_state = false);

    uint8_t pin_number = 0;
    PinMode mode = PinMode::UNINITIALIZED;
    bool default_state = false;
  };

  /**
   * @brief Static initialization of the BCM registers.
   * @details This function is called once, it maps the BCM registers to
   * gpio_memory_map_. I decided to go with the simple approach of using
   * `/dev/gpiomem` instead of the others as I currently don't need any
   * features past basic digital IO.
   * @exception GpioException
   */
  static void initialize();

  /**
   * @brief Static function to close the BCM registers and reset the reserved
   * pins.
   * @details This deallocates the memory map and resets the reserved pins
   * array.
   * @todo Set pins to their 'initial_state_' before closing.
   */
  static void deInitialize();

  /**
   * @brief Check if the GPIO registers are mapped.
   * @return true - The registers are mapped.
   * @return false - The registers are not mapped.
   */
  static bool isInitialized();

  /**
   * @brief A function to setup a pin for input or output.
   *
   * @param pin The pin to setup.
   * @exception GpioException
   */
  void setupPin(Pin pin);

  /**
   * @brief Write a value to an output pin pin (HIGH or LOW)
   * @details The pin must be reserved and owned by the current instance before
   * writing to it. The pin must also be set as an output pin.
   *
   * @param pin The pin to write to.
   * @param on The value to write to the pin, true for HIGH, false for LOW.
   *
   * @exception GpioException
   *
   * @todo Verify that the pin is owned by the current instance.
   * @todo Can a user change the mode of a pin after it has been reserved to
   * write to it?
   */
  void write(Pin pin, bool high);

  /**
   * @brief Read the value of an input pin.
   * @details The pin must be reserved and owned by the current instance before
   * reading from it. The pin must also be set as an input pin.
   *
   * @param pin The pin to read from.
   * @return true The pin is HIGH.
   * @return false The pin is LOW.
   *
   * @exception GpioException
   */
  bool read(Pin pin);

private:
  /**
   * @brief A function to check if a pin has been reserved.
   * @details This function is not thread safe, the caller must lock the mutex
   * prior to calling this function.
   * @return true The pin has been initialized.
   * @return false The pin has not been initialized.
   */
  bool isPinReserved(const Pin &pin);

  /**
   * @brief A function to reserve a pin for use.
   * @details This function is not thread safe, the caller must lock the mutex.
   * This should be called after the pin has been initialized. Parameter
   * safety is not checked.
   * @param pin The pin to reserve.
   */
  void reservePin(const Pin &pin);

  /**
   * @brief A function to check if the current instance owns a reserved pin.
   * @details This function accesses the pins_owned_ variable, which is not
   * static.
   * @param pin The pin to check.
   * @return true This instance owns the pin.
   * @return false This instance does not own the pin.
   */
  bool isOwner(const Pin &pin);

  /**
   * @brief A function to set the owner of a pin.
   * @details Accesses non-static variable pins_owned_. This should only be
   * called if the pin is not reserved by another instance.
   *
   * @param pin The pin to become the owner of.
   */
  void setOwner(const Pin &pin);

  static std::mutex gpio_lock_;

  /**
   * @brief This array is used to keep track of which pins are reserved.
   * @details Initialized in Gpio::Initialize(), all pins are initially
   * set with mode PinMode::UNINITIALIZED and a pin number of -1.
   * @see Gpio::reservePin()
   * @see Gpio::isPinReserved()
   * @see Gpio::deInitialize()
   */
  static std::array<Pin, 28> reserved_pins_;

  /**
   * @brief Contains the pins owned by a particular instance of Gpio.
   * @details This is a bitmask, where each bit corresponds to a pin.
   * @see Gpio::setOwner()
   * @see Gpio::isOwner()
   */
  uint32_t pins_owned_ = 0;
};

} // namespace giraffe

#endif /* GPIO_INTERFACE_HPP_ */