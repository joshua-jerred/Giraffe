#include "shared_data.hpp"

Json data::toJson(data::Frame<std::string, data::DataPacket> &frame) {
  Json json_data;
  for (auto &item : frame.getAll()) {
    json_data[item.first] = item.second.value;
  }
  return json_data;
}

Json data::toJson(data::Frame<std::string, data::AdcValue> &frame) {
  Json json_data;
  for (auto &item : frame.getAll()) {
    json_data[item.first] = Json{{"raw_count", item.second.first},
                                 {"interpolated_value", item.second.second}};
  }
  return json_data;
}

void data::to_json(json &j, const data::GpsFrame &frame) {
  std::string time_str = bst::time::dateAndTimeString(
      bst::time::TimeZone::UTC, '-', ' ', ':', frame.gps_utc_time);
  j = json{{"gps_utc_time", time_str},
           {"is_valid", frame.is_valid},
           {"fix", data::K_GPS_FIX_TO_STRING_MAP.at(frame.fix)},
           {"num_satellites", frame.num_satellites},
           {"latitude", frame.latitude},
           {"longitude", frame.longitude},
           {"horz_accuracy", frame.horz_accuracy},
           {"altitude", frame.altitude},
           {"vert_accuracy", frame.vert_accuracy},
           {"vertical_speed", frame.vertical_speed},
           {"horizontal_speed", frame.horizontal_speed},
           {"speed_accuracy", frame.speed_accuracy},
           {"heading_of_motion", frame.heading_of_motion},
           {"heading_of_motion_accuracy", frame.heading_accuracy}};
}

void data::blocks::to_json(json &j, const data::blocks::LocationData &data) {
  j = json{{"have_gps_source", data.have_gps_source},
           {"current_gps_fix",
            data::K_GPS_FIX_TO_STRING_MAP.at(data.current_gps_fix)},
           {"last_valid_gps_fix",
            data::K_GPS_FIX_TO_STRING_MAP.at(data.last_valid_gps_fix)},
           {"last_valid_gps_frame", data.last_valid_gps_frame},
           {"last_gps_frame", data.last_gps_frame}};
}
