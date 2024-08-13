/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   calculated_data.cpp
 * @brief
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-08-11
 * @copyright  2024 (license to be defined)
 */

#include "calculated_data.hpp"

namespace data_middleware {

CalculatedData::CalculatedData(data::SharedData &shared_data)
    : shared_data_(shared_data) {
  // Initialize the data buffer
  data_buffer_.pressure_altitude_valid = false;
  data_buffer_.average_speed_valid = false;
  data_buffer_.distance_traveled_valid = false;
  data_buffer_.distance_from_launch_valid = false;
  data_buffer_.max_speed_valid = false;

  pressure_data_timeout_.reset();
  positional_data_timeout_.reset();
}

void CalculatedData::updateCalculatedData() {
  if (pressure_data_timeout_.isDone()) {
    // Data is stale
    data_buffer_.pressure_altitude_valid = false;
  }

  if (positional_data_timeout_.isDone()) {
    // Data is stale
    data_buffer_.distance_traveled_valid = false;
    data_buffer_.distance_from_launch_valid = false;
    data_buffer_.average_speed_valid = false;
  }

  shared_data_.blocks.calculated_data.set(data_buffer_);
}

void CalculatedData::addPressureData(double pressure_mbar) {
  /// @todo Move this pressure calculation function to BoosterSeat
  data_buffer_.pressure_altitude_m =
      (1 - std::pow(pressure_mbar / 1013.25, 0.190284)) * 145366.45 * 0.3048;

  data_buffer_.pressure_altitude_valid = true;
  pressure_data_timeout_.reset();
}

void CalculatedData::addPositionalData(const data::GpsFrame &gps_frame) {
  calculateDistanceData(gps_frame);
  calculateVelocityData(gps_frame);
}

void CalculatedData::calculateDistanceData(const data::GpsFrame &gps_frame) {
  try {
    bst::geo::Point current_point(gps_frame.latitude, gps_frame.longitude);

    // Calculate the distance from the launch point
    {
      double launch_lat = 0;
      double launch_lon = 0;
      double launch_alt = 0;

      if (shared_data_.flight_data.getLaunchPosition(launch_lat, launch_lon,
                                                     launch_alt)) {
        // distance from launch point
        data_buffer_.distance_from_launch_m =
            bst::geo::distance(bst::geo::Point(launch_lat, launch_lon),
                               current_point) *
            1000; // convert to meters

        // altitude difference from launch
        data_buffer_.distance_from_ground_m = gps_frame.altitude - launch_alt;

        data_buffer_.distance_from_launch_valid = true;
      }
    }

    // Calculate the distance traveled
    {
      /// @brief The distance that must be traveled for the distance traveled to
      /// be updated. Prevents small GPS drift from accumulating. Coastline
      /// paradox kind of thing.
      constexpr double MOVEMENT_THRESHOLD_KM = 0.2;

      if (!have_initial_gps_point_) {
        last_gps_point_ = current_point;
        have_initial_gps_point_ = true;
      }
      double new_distance = bst::geo::distance(last_gps_point_, current_point);
      data_buffer_.distance_traveled_m =
          (distance_traveled_ + new_distance) * 1000;

      if (new_distance >= MOVEMENT_THRESHOLD_KM) {
        distance_traveled_ += new_distance;
        last_gps_point_ = current_point;
      }
      data_buffer_.distance_traveled_valid = true;
    }

    positional_data_timeout_.reset();
  } catch (const bst::BoosterSeatException &e) {
    logger_.debug("bst::geo exception");
  }
}

void CalculatedData::calculateVelocityData(const data::GpsFrame &gps_frame) {
  // Maximum speeds
  if (gps_frame.horizontal_speed > data_buffer_.max_horizontal_speed_mps) {
    data_buffer_.max_horizontal_speed_mps = gps_frame.horizontal_speed;
    data_buffer_.max_speed_valid = true;
  }

  if (gps_frame.vertical_speed > data_buffer_.max_vertical_speed_mps) {
    data_buffer_.max_vertical_speed_mps = gps_frame.vertical_speed;
    data_buffer_.max_speed_valid = true;
  }

  // Average speed
  if (average_speed_timer_.isDone()) {
    data_buffer_.average_speed_valid = true;
    average_speed_timer_.reset();

    average_horizontal_speed_.addValue(gps_frame.horizontal_speed);
    average_vertical_speed_.addValue(gps_frame.vertical_speed);
  }

  data_buffer_.average_horiz_speed_mps_1min =
      average_horizontal_speed_.getAverage();
  data_buffer_.average_vert_speed_mps_1min =
      average_vertical_speed_.getAverage();
}

} // namespace data_middleware