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

namespace command_line_interface {

class GroundStationAgent {
public:
  GroundStationAgent(AgentData &agent_data) : agent_data_{agent_data} {
  }
  ~GroundStationAgent() = default;

  void process() {
    checkForUpdateFiles();
  }

private:
  void checkForUpdateFiles() {
    static constexpr int UPDATE_INTERVAL_MS = 3000;
    static bst::Timer timer{UPDATE_INTERVAL_MS};

    if (!timer.isDone()) {
      return;
    }
    timer.reset();

    std::cout << "checking " << data_dir_path_ << std::endl;
  }

  AgentData &agent_data_;

  const std::string data_dir_path_{
      giraffe::file_paths::getFlightSystemAgentDirPath() + "/software"};
};

} // namespace command_line_interface