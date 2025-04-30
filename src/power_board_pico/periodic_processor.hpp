/// =*========GIRAFFE========*=
/// A Unified Flight Command and Control System
/// https://github.com/joshua-jerred/Giraffe
/// https://giraffe.joshuajer.red/
/// =*=======================*=
///
/// @file   periodic_processor.hpp
///
/// =*=======================*=
/// @author     Joshua Jerred (https://joshuajer.red)
/// @date       2025-03-02
/// @copyright  2025 (license to be defined)

#pragma once

#include <cstdint>

#include "pico/time.h"

namespace power_board {

class PeriodicProcessor {
public:
  PeriodicProcessor(uint64_t interval_ms) : interval_us_(interval_ms * 1000) {
    last_process_time_ = time_us_64();
  }

  void process() {
    const uint64_t us_since_last_process = time_us_64() - last_process_time_;
    if (us_since_last_process > interval_us_) {
      processImpl();
      last_process_time_ = time_us_64();
    }
  }

protected:
  virtual void processImpl() = 0;

private:
  const uint64_t interval_us_;
  uint64_t last_process_time_ = 0;
};

} // namespace power_board
