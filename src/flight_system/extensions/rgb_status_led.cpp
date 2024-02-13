/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   rgb_status_led.cpp
 * @brief  RGB Status LED extension source file.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-01-24
 * @copyright  2024 (license to be defined)
 */

#include "rgb_status_led.hpp"

namespace extension {

RgbStatusLed::RgbStatusLed(ExtensionResources &resources,
                           cfg::ExtensionMetadata metadata)
    : Extension(resources, metadata), status_led_(resources.status_led) {
  size_t pos = 0;
  static const std::string delimiter = ",";

  std::string pin_str = metadata.extra_args;
  std::string red_pin_str;
  std::string green_pin_str;
  std::string blue_pin_str;

  try {

    while ((pos = pin_str.find(delimiter)) != std::string::npos) {
      std::string token = pin_str.substr(0, pos);
      pin_str.erase(0, pos + delimiter.length());

      if (red_pin_str.empty()) {
        red_pin_str = token;
      } else if (green_pin_str.empty()) {
        green_pin_str = token;
      } else if (blue_pin_str.empty()) {
        blue_pin_str = token;
      }
    }
  } catch (const std::exception &e) {
    /// @todo Add extension fault
    raiseFault(DiagnosticId::EXT_FAULT_rgbStatusLedExtraArgs);
    return;
  }
}

void RgbStatusLed::startup() {
}

void RgbStatusLed::loop() {
}

} // namespace extension