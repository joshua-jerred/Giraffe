/// =*========GIRAFFE========*=
/// A Unified Flight Command and Control System
/// https://github.com/joshua-jerred/Giraffe
/// https://giraffe.joshuajer.red/
/// =*=======================*=
///
/// @file   agent_data.hpp
///
/// =*=======================*=
/// @author     Joshua Jerred (https://joshuajer.red)
/// @date       2024-11-23
/// @copyright  2024 (license to be defined)

#pragma once

#include <string>

#include <BoosterSeat/stopwatch.hpp>

#include "json.hpp"

namespace flight_system_agent {

enum class FlightSoftwareStatus : uint8_t {
  UNKNOWN = 0,
  STOPPED = 1,
  STARTING = 2,
  RUNNING = 3,
  STOPPING = 4,
  RESTARTING = 5,
  UPDATING = 6
};

std::string to_string(const FlightSoftwareStatus status);

/// @brief Contains all data for the flight system agent
class AgentData {
public:
  AgentData() {
    agent_uptime_stopwatch_.start();
  }

  ~AgentData() = default;

  AgentData(const AgentData &) = delete;
  AgentData &operator=(const AgentData &) = delete;
  AgentData(AgentData &&) = delete;

  void getStatusJson(Json &json) const;

  void setAgentStopFlag(bool stop_agent) {
    agent_stop_flag_ = stop_agent;
  }

  bool isAgentStopRequested() const {
    return agent_stop_flag_;
  }

private:
  bool agent_stop_flag_ = false;

  FlightSoftwareStatus flight_software_status_ = FlightSoftwareStatus::UNKNOWN;
  bst::Stopwatch agent_uptime_stopwatch_{};
};

} // namespace flight_system_agent