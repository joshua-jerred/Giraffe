/// =*========GIRAFFE========*=
/// A Unified Flight Command and Control System
/// https://github.com/joshua-jerred/Giraffe
/// https://giraffe.joshuajer.red/
/// =*=======================*=
///
/// @file   sd_card.hpp
///
/// =*=======================*=
/// @author     Joshua Jerred (https://joshuajer.red)
/// @date       2025-03-17
/// @copyright  2025 (license to be defined)

#pragma once

#include "periodic_processor.hpp"

namespace power_board {

class SdCard : public PeriodicProcessor {
public:
  SdCard(uint64_t module_process_interval_ms = 1000)
      : PeriodicProcessor(module_process_interval_ms) {
  }

  ~SdCard() {
  }

private:
  void processImpl() override {
    // printf("SD Card processing...\n");
  }
};

} // namespace power_board