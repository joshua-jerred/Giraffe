/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   status_led.hpp
 * @brief  RGB Status LED extension header file.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-01-24
 * @copyright  2024 (license to be defined)
 */

#ifndef RGB_STATUS_LED_HPP_
#define RGB_STATUS_LED_HPP_

#include "extension_base.hpp"
#include "gpio.hpp"
#include "status_led.hpp"

namespace extension {

/**
 * @brief A simple extension to control an RGB status LED.
 * @details This extension controls an RGB status LED via GPIO pins. The pin
 * numbers should be passed in via the extra_args field of the extension config.
 * These are passed in as a comma separated list of integers. For example:
 * "extra_args": "1,2,3"
 */
class RgbStatusLed : public Extension {
public:
  RgbStatusLed(ExtensionResources &resources, cfg::ExtensionMetadata metadata);
  ~RgbStatusLed() = default;

  void startup() override;
  void loop() override;

private:
  giraffe::StatusLed &status_led_;

  giraffe::Gpio gpio_{};
  giraffe::Gpio::Pin red_pin_{};
  giraffe::Gpio::Pin green_pin_{};
  giraffe::Gpio::Pin blue_pin_{};
};

} // namespace extension

#endif /* RGB_STATUS_LED_HPP_ */