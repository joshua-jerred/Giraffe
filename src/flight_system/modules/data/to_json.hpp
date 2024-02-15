/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   to_json.hpp
 * @brief  Utility functions for converting data to JSON.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-10-26
 * @copyright  2023 (license to be defined)
 */

#ifndef TO_JSON_HPP_
#define TO_JSON_HPP_

#include "positional.hpp"
#include "to_string.hpp"
#include <json.hpp>

namespace util {
Json to_json(data::GpsFrame gps_frame) {
  Json json;
  json["utc"] = gps_frame.gps_utc_time.time_since_epoch().count();
  json["is_valid"] = gps_frame.is_valid;
  json["fix"] = util::to_string(gps_frame.fix);
  json["num_sats"] = gps_frame.num_satellites;
  json["lat"] = gps_frame.latitude;
  json["lon"] = gps_frame.longitude;
  json["horz_accuracy"] = gps_frame.horz_accuracy;
  json["alt"] = gps_frame.altitude;
  json["vert_accuracy"] = gps_frame.vert_accuracy;
  json["vert_velocity"] = gps_frame.vertical_speed;
  json["horz_velocity"] = gps_frame.horizontal_speed;
  json["speed_accuracy"] = gps_frame.speed_accuracy;
  json["heading"] = gps_frame.heading_of_motion;
  json["heading_accuracy"] = gps_frame.heading_accuracy;

  return json;
}
} // namespace util

#endif /* TO_JSON_HPP_ */