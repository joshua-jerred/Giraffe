/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   flight_data.hpp
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-09-11
 * @copyright  2024 (license to be defined)
 */

#pragma once

#include <atomic>
#include <string>

#include <BoosterSeat/clock.hpp>
#include <BoosterSeat/time.hpp>

#include "flight_phase.hpp"
#include "i_built_in_test.hpp"
#include "json.hpp"

class FlightPhaseManager;

namespace data {

class FlightData {
public:
  /// @brief The number of seconds since the flight runner was started.
  /// @return int The number of seconds. This value should always be positive.
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
    return flight_phase_;
  }

  /**
   * @brief Get the predicted flight phase.
   * @return FlightPhase The predicted flight phase.
   */
  FlightPhase getPredictedPhase() const {
    return predicted_phase_;
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

  /// @brief Get the mission clock time in seconds. May be negative or positive.
  /// A negative value is in the future.
  /// @param[out] seconds - The mission clock time in seconds.
  /// @return bool - True if the mission clock is running, false when it's
  /// paused.
  /// @todo Implement the mission clock from the JS side into the flight runner.
  bool getMissionClockSeconds(int32_t &seconds) {
    seconds = 0;
    return false;
  }

  // friend FlightRunner;

  Json toJson() {
    return Json({{"uptime", getUptimeString()},
                 {"system_time_utc", getSystemTimeUtc()},
                 {"flight_phase", util::to_string(flight_phase_)},
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

  /**
   * @brief This is here so that the flight phase manager can set the flight
   * phase.
   */
  friend FlightPhaseManager;

  Json getBitTestData() {
    std::lock_guard<std::mutex> lock(bit_test_buffer_mutex_);
    return bit_test_buffer_;
  }

  void setBitTestData(Json &data) {
    std::lock_guard<std::mutex> lock(bit_test_buffer_mutex_);
    bit_test_buffer_ = data;
  }

private:
  void setFlightPhase(FlightPhase new_phase) {
    flight_phase_ = new_phase;
  }

  void setPhasePrediction(FlightPhase predicted_phase, double launch,
                          double ascent, double descent, double recovery,
                          double data_quality) {
    predicted_phase_ = predicted_phase;
    launch_prediction_ = launch;
    ascent_prediction_ = ascent;
    descent_prediction_ = descent;
    recovery_prediction_ = recovery;
    prediction_quality_ = data_quality;
  }

  /// @brief A time point representing the start time of the flight runner.
  const bst::clck::TimePoint start_time_ = bst::clck::now();

  /// @brief The current flight phase.
  std::atomic<FlightPhase> flight_phase_ = FlightPhase::UNKNOWN;

  /// @brief The predicted flight phase. Used by the flight phase manager to
  /// determine the current phase.
  std::atomic<FlightPhase> predicted_phase_ = FlightPhase::UNKNOWN;

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
  std::atomic<double> prediction_quality_ = 0.0;

  std::mutex bit_test_buffer_mutex_{};
  Json bit_test_buffer_{};
};

} // namespace data