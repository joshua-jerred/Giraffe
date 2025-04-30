/// =*========GIRAFFE========*=
/// A Unified Flight Command and Control System
/// https://github.com/joshua-jerred/Giraffe
/// https://giraffe.joshuajer.red/
/// =*=======================*=
///
/// @file   agent_data.cpp
///
/// =*=======================*=
/// @author     Joshua Jerred (https://joshuajer.red)
/// @date       2024-11-23
/// @copyright  2024 (license to be defined)

#include "agent_data.hpp"

namespace command_line_interface {

std::string to_string(const FlightSoftwareStatus status) {
  switch (status) {
  case FlightSoftwareStatus::UNKNOWN:
    return "UNKNOWN";
  case FlightSoftwareStatus::STOPPED:
    return "STOPPED";
  case FlightSoftwareStatus::STARTING:
    return "STARTING";
  case FlightSoftwareStatus::RUNNING:
    return "RUNNING";
  case FlightSoftwareStatus::STOPPING:
    return "STOPPING";
  case FlightSoftwareStatus::RESTARTING:
    return "RESTARTING";
  case FlightSoftwareStatus::UPDATING:
    return "UPDATING";
  }
  return "UNKNOWN";
}

void AgentData::getStatusJson(Json &json) const {
  // clang-format off
  json = {
    {"flight_software_status", to_string(flight_software_status_)},
    {"agent_uptime_s", agent_uptime_stopwatch_.elapsedSeconds()}
  };
  // clang-format on
}

} // namespace command_line_interface