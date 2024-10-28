/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   calculated_data.hpp
 * @brief Calculates new data from the existing data.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-08-11
 * @copyright  2024 (license to be defined)
 */

#pragma once

#include <BoosterSeat/geo.hpp>
#include <BoosterSeat/rolling_average.hpp>
#include <BoosterSeat/time.hpp>
#include <BoosterSeat/timer.hpp>

#include "configuration.hpp"
#include "logger.hpp"
#include "shared_data.hpp"

namespace data_middleware {

/// @todo Name conflict with struct data::blocks::CalculatedData
/// @warning
class CalculatedData {
public:
  CalculatedData(data::SharedData &shared_data, cfg::Configuration &config);
  ~CalculatedData() = default;

  /// @brief Update the calculated data and store it in the shared data.
  /// @brief Needs to be called periodically. This will update the shared data
  /// with the calculated data.
  void updateCalculatedData();

  /// @brief Update positional data based on GPS data.
  /// @param gps_frame - A GPS frame that is both valid and unique.
  void addPositionalData(const data::GpsFrame &gps_frame);

  /// @brief Update the pressure altitude based on data from barometers
  /// providing pressure data.
  void addPressureData(double pressure_mbar);

  /// @brief Update battery data based on battery voltage.
  /// @param voltage_mv - The battery voltage in millivolts.
  void addBatteryVoltageData(uint32_t voltage_mv);

private:
  /// @brief Calculate average vertical and horizontal speeds. Called by
  /// updatePositionalData()
  /// @param gps_frame - Valid GPS data
  void calculateVelocityData(const data::GpsFrame &gps_frame);

  /// @brief Calculate the distance traveled and the distance from the launch.
  /// Called by updatePositionalData()
  /// @param gps_frame - Valid GPS data
  void calculateDistanceData(const data::GpsFrame &gps_frame);

  double battery_voltage_max_mv_ = 0;
  double battery_voltage_min_mv_ = 0;

  /**
   * @brief The last gps point. Used to calculate distance traveled.
   */
  bst::geo::Point last_gps_point_{};

  /// @brief True if we have at least one GPS point. This is needed as the
  /// launch position shouldn't be used in "distance traveled" calculations.
  bool have_initial_gps_point_ = false;

  /**
   * @brief The distance traveled in km.
   */
  double distance_traveled_ = 0.0;

  /// @brief If this timer expires, the battery voltage data is considered
  /// stale/invalid.
  bst::Timer battery_voltage_timeout_{30 * 1000};

  /// @brief If this timer expires, the pressure data is considered
  /// stale/invalid.
  bst::Timer pressure_data_timeout_{20 * 1000};

  /// @brief If this timer expires, the positional data is considered
  /// stale/invalid.
  bst::Timer positional_data_timeout_{30 * 1000};

  /// @brief Timer used to keep track of the time since we last had valid GPS
  /// data.
  bst::Timer time_since_last_valid_frame_{5 * 1000};

  /// @brief The time of the last GPS frame. Used to ensure that a frame is only
  /// used once.
  bst::clck::TimePoint previous_gps_frame_time_{};

  /**
   * @brief Rolling average of the horizontal speed. Once every 5 seconds,
   * 12 samples are averaged.
   * @todo This assumes that data is coming in at the correct rate. Implement a
   * check for this. Possibly use bst::RollingAverage::removeOldest().
   */
  bst::RollingAverage<double> average_horizontal_speed_{12};
  bst::RollingAverage<double> average_vertical_speed_{12};
  bst::Timer average_speed_timer_{5 * 1000};

  data::blocks::CalculatedData data_buffer_{};

  data::SharedData &shared_data_;

  cfg::Configuration &config_;

  giraffe::Logger logger_{shared_data_.streams.log,
                          node::Identification::DATA_MODULE, "calculated_data"};
};

} // namespace data_middleware