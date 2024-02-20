/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   shared_data.hpp
 * @brief  Contains the SharedData struct, used across all modules and
 * extensions.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-06-07
 * @copyright  2023 (license to be defined)
 */

#ifndef SHARED_DATA_HPP_
#define SHARED_DATA_HPP_

#include <BoosterSeat/clock.hpp>
#include <BoosterSeat/time.hpp>

#include "blocks.hpp"
#include "error_frame.hpp"
#include "flight_phase.hpp"
#include "frame.hpp"
#include "log_container.hpp"
#include "status_led.hpp"
#include "streams.hpp"

class FlightRunner;

namespace data {
struct Streams {
  DataStream data{};
  LogStream log{};
  GpsFrameStream gps{};
  ImuFrameStream imu{};

  /**
   * @brief The command stream. Parsed by the flight runner, not the data
   * module.
   */
  CommandStream command{};
};

struct SharedBlocks {
  blocks::Block<blocks::StreamsStats> stream_stats{};
  blocks::Block<blocks::ModulesStatuses> modules_statuses{};
  blocks::Block<blocks::ServerModuleStats> server_module_stats{};
  blocks::Block<blocks::DataLogStats> data_log_stats{};
  blocks::Block<blocks::SystemInfo> system_info{};
  blocks::Block<blocks::ExtensionModuleStats> extension_module_stats{};
  blocks::Block<blocks::LocationData> location_data{};
  blocks::Block<blocks::ImuData> imu_data{};
  blocks::Block<blocks::CameraImages> camera{};
  blocks::Block<blocks::TelemetryModuleStats> telemetry_module_stats{};
  blocks::Block<blocks::CalculatedData> calculated_data{};
  blocks::Block<blocks::FileSystemData> file_system_data{};
};

struct Frames {
  /**
   * @brief Data frame containing all active error messages.
   */
  ErrorFrame error_frame{};

  /**
   * @brief Temperature data from environmental extensions.
   */
  Frame<std::string, DataPacket> env_temp{};

  /**
   * @brief Pressure data from environmental extensions.
   */
  Frame<std::string, DataPacket> env_pres{};

  /**
   * @brief Humidity data from environmental extensions.
   */
  Frame<std::string, DataPacket> env_hum{};
};

class Misc {
public:
  int getUptimeSeconds() const {
    return bst::clck::secondsElapsed(start_time_);
  }

  /**
   * @brief Get the uptime represented as a string in the format "HH:MM:SS".
   * @return std::string The uptime string.
   */
  std::string getUptimeString() const {
    return bst::time::elapsedAsciiClock(start_time_);
  }

  /**
   * @brief Get the current flight phase.
   * @return FlightPhase The current flight phase.
   */
  FlightPhase getFlightPhase() const {
    return flight_phase;
  }

  /**
   * @brief This is here so that the flight runner alone can set the flight
   * phase.
   */
  friend FlightRunner;

private:
  /// @brief A time point representing the start time of the flight runner.
  const bst::clck::TimePoint start_time_ = bst::clck::now();

  /// @brief The current flight phase.
  std::atomic<FlightPhase> flight_phase = FlightPhase::UNKNOWN;
};

/**
 * @brief Data that is shared across all modules and extensions.
 */
struct SharedData {
  Streams streams = Streams();
  Frames frames = Frames();
  SharedBlocks blocks{};
  Misc misc = Misc();
  LogContainer log_container{};
  giraffe::StatusLedState status_led{};
};

Json toJson(data::Frame<std::string, data::DataPacket> &frame);

} // namespace data

#endif