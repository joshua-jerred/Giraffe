/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   positional.hpp
 * @brief  Positional data for the flight system (gps/imu data)
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-06-14
 * @copyright  2023 (license to be defined)
 */

#ifndef POSITIONAL_HPP_
#define POSITIONAL_HPP_

#include <BoosterSeat/clock.hpp>
#include <cstdint>

namespace data {

enum class GpsFix : uint8_t { ERROR = 0, NO_FIX = 1, FIX_2D = 2, FIX_3D = 3 };

/**
 * @brief A frame of GPS data straight from a GPS extension.
 */
struct GpsFrame {
  /// @brief The UTC time of the GPS fix from the actual GPS clock.
  BoosterSeat::clck::TimePoint gps_utc_time = BoosterSeat::clck::TimePoint();

  /// @brief The GPS fix type.
  GpsFix fix = GpsFix::ERROR;

  /// @brief The number of satellites used to calculate the fix.
  uint8_t num_satellites = 0;

  /// @brief The latitude in decimal degrees.
  double latitude = 0.0;

  /// @brief The longitude in decimal degrees.
  double longitude = 0.0;

  /// @brief The horizontal accuracy in meters.
  double horz_accuracy = 0.0;

  /// @brief The altitude in meters.
  double altitude = 0.0;

  /// @brief The vertical accuracy in meters.
  double vert_accuracy = 0.0;

  /// @brief The vertical speed in meters/second.
  double vertical_speed = 0.0;

  /// @brief The horizontal speed in meters/second.
  double horizontal_speed = 0.0;

  /// @brief The horizontal speed accuracy in meters/second.
  double speed_accuracy = 0.0;

  /// @brief The heading of motion in degrees.
  double heading_of_motion = 0.0;

  /// @brief The heading of motion accuracy in degrees.
  double heading_accuracy = 0.0;
};

struct ImuFrame {
  double x_acceleration = 0.0;
  double y_acceleration = 0.0;
  double z_acceleration = 0.0;

  double x_rotation = 0.0;
  double y_rotation = 0.0;
  double z_rotation = 0.0;

  double x_magnetic_field = 0.0;
  double y_magnetic_field = 0.0;
  double z_magnetic_field = 0.0;
};

/**
 * @todo Implement this function.
 * @warning This function is not yet implemented!
 *
 * @brief Checks if a GPS frame is valid. Currently not implemented.
 * @details Simply checks if the data fields are within logical bounds. Does
 * not have any knowledge of the previous location, so it cannot check
 * for a sudden jump in position.
 *
 * @param frame - The frame to check.
 * @return true - If the frame is valid.
 * @return false - If the frame is invalid.
 */
bool isGpsFrameValid(const data::GpsFrame &frame);

} // namespace data

#endif /* POSITIONAL_HPP_ */