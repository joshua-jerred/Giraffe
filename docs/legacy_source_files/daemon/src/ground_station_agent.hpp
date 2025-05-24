/// =*========GIRAFFE========*=
/// A Unified Flight Command and Control System
/// https://github.com/joshua-jerred/Giraffe
/// https://giraffe.joshuajer.red/
/// =*=======================*=
///
/// @file   ground_station_agent.hpp
///
/// =*=======================*=
/// @author     Joshua Jerred (https://joshuajer.red)
/// @date       2024-12-18
/// @copyright  2024 (license to be defined)

#pragma once

// Internal Library
#include <BoosterSeat/timer.hpp>

// Common
#include "giraffe_file_paths.hpp"

// Local
#include "agent_data.hpp"
#include "agent_settings.hpp"
#include "ground_software_manager.hpp"

namespace command_line_interface {

class GroundStationAgent {
public:
  GroundStationAgent(AgentData &agent_data) : agent_data_{agent_data} {
  }
  ~GroundStationAgent() = default;

  void process() {
    if (software_manager_timer_.isDone()) {
      software_manager_timer_.reset();
      software_manager_.process();
    }

    // Check for new commands
  }

private:
  AgentData &agent_data_;

  GroundSoftwareManager software_manager_{};
  static constexpr int SOFTWARE_MANAGER_UPDATE_INTERVAL_MS = 2000;
  bst::Timer software_manager_timer_{SOFTWARE_MANAGER_UPDATE_INTERVAL_MS};
};

} // namespace command_line_interface