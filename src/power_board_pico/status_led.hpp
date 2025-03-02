/// =*========GIRAFFE========*=
/// A Unified Flight Command and Control System
/// https://github.com/joshua-jerred/Giraffe
/// https://giraffe.joshuajer.red/
/// =*=======================*=
///
/// @file   led_manager.hpp
///
/// =*=======================*=
/// @author     Joshua Jerred (https://joshuajer.red)
/// @date       2025-03-01
/// @copyright  2025 (license to be defined)

#pragma once

#include "pico/stdlib.h"

namespace power_board {

class StatusLed {
public:
  static const uint LED_PIN = 25;

  StatusLed(uint64_t blink_interval_ms = 2000,
            uint64_t error_blink_interval_ms = 100)
      : normal_blink_interval_us_(blink_interval_ms * 1000),
        error_blink_interval_us_(error_blink_interval_ms * 1000) {
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
  }

  ~StatusLed() {
    gpio_put(LED_PIN, 0);
  }

  void process() {
    const uint64_t us_since_last_toggle = time_us_64() - last_toggle_time_;
    if (us_since_last_toggle > current_blink_interval_us_) {
      toggle();
      last_toggle_time_ = time_us_64();
    }
  }

  void toggle() {
    if (led_on_) {
      gpio_put(LED_PIN, 0);
      led_on_ = false;
    } else {
      gpio_put(LED_PIN, 1);
      led_on_ = true;
    }
  }

  void setErrorState(bool error_state) {
    error_state_ = error_state;
    current_blink_interval_us_ =
        error_state ? error_blink_interval_us_ : normal_blink_interval_us_;
  }

private:
  const uint64_t normal_blink_interval_us_;
  const uint64_t error_blink_interval_us_;

  uint64_t current_blink_interval_us_ = normal_blink_interval_us_;
  uint64_t last_toggle_time_ = 0;
  bool error_state_ = false;
  bool led_on_ = false;
};

}; // namespace power_board