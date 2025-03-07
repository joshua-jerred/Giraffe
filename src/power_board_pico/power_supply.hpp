/// =*========GIRAFFE========*=
/// A Unified Flight Command and Control System
/// https://github.com/joshua-jerred/Giraffe
/// https://giraffe.joshuajer.red/
/// =*=======================*=
///
/// @file   power_supply.hpp
///
/// =*=======================*=
/// @author     Joshua Jerred (https://joshuajer.red)
/// @date       2025-03-07
/// @copyright  2025 (license to be defined)

#pragma once

#include "hardware/adc.h"

#include "periodic_processor.hpp"

namespace power_board {

class PowerSupply : public PeriodicProcessor {
public:
  PowerSupply(uint64_t module_process_interval_ms)
      : PeriodicProcessor(module_process_interval_ms) {
    bi_decl(bi_1pin_with_name(PRIMARY_POWER_PIN, "Primary Power Pin"));
    initPrimaryPower();
  }

private:
  /// @brief Float to enable, sink to disable
  static constexpr uint8_t PRIMARY_POWER_PIN = 13; // temporary

  void processImpl() {
    // printf("Power Supply\n");
  }

  void initPrimaryPower() {
    gpio_init(PRIMARY_POWER_PIN);
    gpio_set_dir(PRIMARY_POWER_PIN, GPIO_OUT);
    gpio_pull_down(PRIMARY_POWER_PIN);

    turnOnPrimaryPower();
  }

  void turnOnPrimaryPower() {
    primary_power_commanded_on_ = true;
    gpio_put(PRIMARY_POWER_PIN, 1);
  }

  void turnOffPrimaryPower() {
    primary_power_commanded_on_ = false;
    gpio_put(0, 0);
  }

  bool primary_power_commanded_on_ = false;
};

} // namespace power_board