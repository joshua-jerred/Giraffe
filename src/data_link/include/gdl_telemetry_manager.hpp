/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   gdl_telemetry.hpp
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-09-27
 * @copyright  2024 (license to be defined)
 */

#pragma once

#include <SignalEasel/aprs/telemetry_data.hpp>

#include "gdl_config_and_stats.hpp"
#include "gdl_message.hpp"

namespace giraffe::gdl {

class AprsTelemetryManager {
public:
  using TelemetryData = signal_easel::aprs::telemetry::TelemetryData;

  AprsTelemetryManager(Config &gdl_config) : config_(gdl_config) {
  }
  ~AprsTelemetryManager() = default;

  void addMessage(const Message &message) {
    (void)message;
    std::cout << "Got telemetry message" << std::endl;
  }

  void addData(const TelemetryData &telemetry_data) {
    (void)telemetry_data;
  }

private:
  Config &config_;
};

} // namespace giraffe::gdl