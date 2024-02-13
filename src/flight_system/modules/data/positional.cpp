/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   positional.cpp
 * @brief  Positional data for the flight system (gps/imu data) handling.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-06-23
 * @copyright  2023 (license to be defined)
 */

#include "positional.hpp"

bool data::isGpsFrameValid(const data::GpsFrame &frame) {
  if (!frame.is_valid) {
    return false;
  }

  // Validate the time
  constexpr int kMaxTimeDiffHours = 24;
  bst::clck::TimePoint now = bst::clck::now();
  if (std::chrono::duration_cast<std::chrono::hours>(now - frame.gps_utc_time)
          .count() > kMaxTimeDiffHours) {
    return false;
  }

  // Validate the fix
  constexpr int kRequiredSatellitesFor2DFix = 3;
  constexpr int kRequiredSatellitesFor3DFix = 4;
  if (frame.fix == data::GpsFix::ERROR || frame.fix == data::GpsFix::NO_FIX) {
    return false;
  } else if (frame.fix == data::GpsFix::FIX_2D) {
    if (frame.num_satellites < kRequiredSatellitesFor2DFix) {
      return false;
    }
  } else if (frame.fix == data::GpsFix::FIX_3D) {
    if (frame.num_satellites < kRequiredSatellitesFor3DFix) {
      return false;
    }
  }

  // Validate the latitude
  constexpr double kMaxLatitude = 90.0;
  constexpr double kMinLatitude = -90.0;
  if (frame.latitude < kMinLatitude || frame.latitude > kMaxLatitude) {
    return false;
  }

  // Validate the longitude
  constexpr double kMaxLongitude = 180.0;
  constexpr double kMinLongitude = -180.0;
  if (frame.longitude < kMinLongitude || frame.longitude > kMaxLongitude) {
    return false;
  }

  // Validate the horizontal accuracy
  constexpr double kMinHorizontalAccuracy = 0.0;    // Must be positive
  constexpr double kMaxHorizontalAccuracy = 1000.0; // within 1000 meters
  if (frame.horz_accuracy < kMinHorizontalAccuracy ||
      frame.horz_accuracy > kMaxHorizontalAccuracy) {
    return false;
  }

  // Validate the altitude
  constexpr double kMinAltitude = -450.0;
  constexpr double kMaxAltitude = 40000.0;
  if (frame.altitude < kMinAltitude || frame.altitude > kMaxAltitude) {
    return false;
  }

  // Validate the heading of motion
  constexpr double kMinHeading = 0.0;
  constexpr double kMaxHeading = 360.0;
  if (frame.heading_of_motion < kMinHeading ||
      frame.heading_of_motion > kMaxHeading) {
    return false;
  }

  return true;
}

bool data::isImuFrameValid(const data::ImuFrame &frame) {
  constexpr int kMaxTimeDiffSeconds = 10;         // seconds
  constexpr double kMinAcceleration = -24.0;      // m/s^2
  constexpr double kMaxAcceleration = 24.0;       // m/s^2
  constexpr double kMinAngularVelocity = -2000.0; // deg/s
  constexpr double kMaxAngularVelocity = 2000.0;  // deg/s

  if (!frame.is_valid) {
    return false;
  }
  // Validate the time
  bst::clck::TimePoint now = bst::clck::now();
  if (std::chrono::duration_cast<std::chrono::seconds>(now - frame.time)
          .count() > kMaxTimeDiffSeconds) {
    return false;
  }
  if (frame.x_acceleration < kMinAcceleration ||
      frame.x_acceleration > kMaxAcceleration ||
      frame.y_acceleration < kMinAcceleration ||
      frame.y_acceleration > kMaxAcceleration ||
      frame.z_acceleration < kMinAcceleration ||
      frame.z_acceleration > kMaxAcceleration) {
    return false;
  }

  if (frame.x_angular_velocity < kMinAngularVelocity ||
      frame.x_angular_velocity > kMaxAngularVelocity ||
      frame.y_angular_velocity < kMinAngularVelocity ||
      frame.y_angular_velocity > kMaxAngularVelocity ||
      frame.z_angular_velocity < kMinAngularVelocity ||
      frame.z_angular_velocity > kMaxAngularVelocity) {
    return false;
  }

  return true;
}