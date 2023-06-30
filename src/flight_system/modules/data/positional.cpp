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
  // Validate the time
  constexpr int kMaxTimeDiffHours = 24;
  BoosterSeat::clck::TimePoint now = BoosterSeat::clck::now();
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