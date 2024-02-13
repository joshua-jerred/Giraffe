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
#include "giraffe_exception.hpp"

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
    // parse format <pin1_unint,pin2_unit,pin3_unit>
    red_pin_str = pin_str.substr(0, pin_str.find(delimiter));
    pin_str.erase(0, pin_str.find(delimiter) + delimiter.length());
    green_pin_str = pin_str.substr(0, pin_str.find(delimiter));
    pin_str.erase(0, pin_str.find(delimiter) + delimiter.length());
    blue_pin_str = pin_str;

    // Check for extra arguments
    if (pin_str.length() > 2 || red_pin_str.empty() || green_pin_str.empty() ||
        blue_pin_str.empty()) {
      raiseFault(DiagnosticId::EXT_FAULT_rgbStatusLedExtraArgs,
                 "0: " + pin_str + "," + red_pin_str + "," + green_pin_str +
                     "," + blue_pin_str);
      return;
    }
  } catch (const std::exception &e) {
    /// @todo Add extension fault
    raiseFault(DiagnosticId::EXT_FAULT_rgbStatusLedExtraArgs, "1");
    return;
  }

  try {
    // Convert the pin strings to numbers
    uint8_t red_pin_num = static_cast<uint8_t>(std::stoi(red_pin_str));
    uint8_t green_pin_num = static_cast<uint8_t>(std::stoi(green_pin_str));
    uint8_t blue_pin_num = static_cast<uint8_t>(std::stoi(blue_pin_str));

    // Setup the pins
    constexpr giraffe::Gpio::PinMode K_PIN_MODE =
        giraffe::Gpio::PinMode::OUTPUT;
    constexpr bool K_DEFAULT_STATE = false;
    red_pin_ = giraffe::Gpio::Pin(red_pin_num, K_PIN_MODE, K_DEFAULT_STATE);
    green_pin_ = giraffe::Gpio::Pin(green_pin_num, K_PIN_MODE, K_DEFAULT_STATE);
    blue_pin_ = giraffe::Gpio::Pin(blue_pin_num, K_PIN_MODE, K_DEFAULT_STATE);
  } catch (const std::exception &e) {
    /// @todo Add extension fault
    raiseFault(DiagnosticId::EXT_FAULT_rgbStatusLedExtraArgs,
               "2 " + std::string(e.what()));
    return;
  }

  debug("RGB Status LED Pins Set: " + red_pin_str + "," + green_pin_str + "," +
        blue_pin_str);
}

void RgbStatusLed::startup() {
  // Take ownership of the pins
  try {
    gpio_.setupPin(red_pin_);
    gpio_.setupPin(green_pin_);
    gpio_.setupPin(blue_pin_);
  } catch (const GiraffeException &e) {
    raiseFault(DiagnosticId::EXT_FAULT_rgbStatusLedFailedToSetupPins,
               std::string(e.what()));
  }
}

void RgbStatusLed::loop() {
  auto red_state = status_led_.getRedState();
  auto green_state = status_led_.getGreenState();
  auto blue_state = status_led_.getBlueState();

  // Check for status led states and set the pins accordingly
  // clang-format off

  // red LED logic
  if (
    red_state == giraffe::StatusLedState::State::ON
    ||
    (red_state == giraffe::StatusLedState::State::BLINK && !current_red_state_)
    ) {
    gpio_.write(red_pin_, true);
    current_red_state_ = true;
  } else {
    gpio_.write(red_pin_, false);
    current_red_state_ = false;
  }

  // green LED logic
  if (
    green_state == giraffe::StatusLedState::State::ON
    ||
    (green_state == giraffe::StatusLedState::State::BLINK && !current_green_state_)
    ) {
    gpio_.write(green_pin_, true);
    current_green_state_ = true;
  } else {
    gpio_.write(green_pin_, false);
    current_green_state_ = false;
  }

  // blue LED logic
  if (
    blue_state == giraffe::StatusLedState::State::ON
    ||
    (blue_state == giraffe::StatusLedState::State::BLINK && !current_blue_state_)
    ) {
    gpio_.write(blue_pin_, true);
    current_blue_state_ = true;
  } else {
    gpio_.write(blue_pin_, false);
    current_blue_state_ = false;
  }
  // clang-format on
}

void RgbStatusLed::shutdown() {
  try {
    gpio_.write(red_pin_, false);
    gpio_.write(green_pin_, false);
    gpio_.write(blue_pin_, false);
    gpio_.releasePin(red_pin_);
    gpio_.releasePin(green_pin_);
    gpio_.releasePin(blue_pin_);
  } catch (const GiraffeException &e) {
    raiseFault(DiagnosticId::EXT_FAULT_rgbStatusLedShutdownFailed,
               std::string(e.what()));
  }
}

} // namespace extension