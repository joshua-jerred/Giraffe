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

  /**
   * @brief ADC data frame. std::string is the label of the ADC channel, and
   * the double is the interpolated value.
   */
  Frame<std::string, double> adc{};
};

class FlightData {
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
   * @brief Get the system time in UTC.
   * @return std::string hh:mm:ss
   */
  std::string getSystemTimeUtc() const {
    return bst::time::timeString(bst::time::TimeZone::UTC);
  }

  /**
   * @brief Get the current flight phase.
   * @return FlightPhase The current flight phase.
   */
  FlightPhase getFlightPhase() const {
    return flight_phase;
  }

  void setLaunchPosition(double latitude, double longitude, double altitude) {
    launch_position_latitude_ = latitude;
    launch_position_longitude_ = longitude;
    launch_altitude_ = altitude;
    have_launch_position_ = true;
  }

  bool getLaunchPosition(double &latitude, double &longitude,
                         double &altitude) {
    if (have_launch_position_) {
      latitude = launch_position_latitude_;
      longitude = launch_position_longitude_;
      altitude = launch_altitude_;
      return true;
    }
    return false;
  }

  void setPhasePrediction(double launch, double ascent, double descent,
                          double landing, double recovery) {
    launch_prediction_ = launch;
    ascent_prediction_ = ascent;
    descent_prediction_ = descent;
    recovery_prediction_ = recovery;
  }

  /**
   * @brief This is here so that the flight runner alone can set the flight
   * phase and mission clock.
   */
  friend FlightRunner;

  Json toJson() {
    return Json({{"uptime", getUptimeString()},
                 {"system_time_utc", getSystemTimeUtc()},
                 {"flight_phase", util::to_string(flight_phase)},
                 {"launch_position",
                  {
                      {"valid", have_launch_position_.load()},
                      {"latitude", launch_position_latitude_.load()},
                      {"longitude", launch_position_longitude_.load()},
                      {"altitude", launch_altitude_.load()},
                  }},
                 {"phase_predictions",
                  {
                      {"launch", launch_prediction_.load()},
                      {"ascent", ascent_prediction_.load()},
                      {"descent", descent_prediction_.load()},
                      {"recovery", recovery_prediction_.load()},
                  }}});
  }

private:
  /// @brief A time point representing the start time of the flight runner.
  const bst::clck::TimePoint start_time_ = bst::clck::now();

  /// @brief The current flight phase.
  std::atomic<FlightPhase> flight_phase = FlightPhase::UNKNOWN;

  /// @brief True if the launch position has been set.
  std::atomic<bool> have_launch_position_ = false;
  /// @brief The launch position.
  std::atomic<double> launch_altitude_ = 0.0;
  std::atomic<double> launch_position_latitude_ = 0.0;
  std::atomic<double> launch_position_longitude_ = 0.0;

  std::atomic<double> launch_prediction_ = 0.0;
  std::atomic<double> ascent_prediction_ = 0.0;
  std::atomic<double> descent_prediction_ = 0.0;
  std::atomic<double> recovery_prediction_ = 0.0;
};

/**
 * @brief Data that is shared across all modules and extensions.
 */
struct SharedData {
  FlightData flight_data = FlightData();
  Streams streams = Streams();
  Frames frames = Frames();
  SharedBlocks blocks{};
  LogContainer log_container{};
  giraffe::StatusLedState status_led{};
};

Json toJson(data::Frame<std::string, data::DataPacket> &frame);

} // namespace data

#endif