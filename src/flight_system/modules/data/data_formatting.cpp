/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   data_formatting.cpp
 * @brief  Implementation of the DataFormatter class.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-06-08
 * @copyright  2023 (license to be defined)
 */

#include <type_traits>

#include <BoosterSeat/string_formatting.hpp>
#include <BoosterSeat/time.hpp>

#include "data_formatting.hpp"
#include "to_string.hpp"

namespace bst = BoosterSeat::time;
using namespace data_middleware;

std::string DataFormatter::fullFrame() {
  Json frame;
  setupFrameStructure(frame, "data");
  addComponent(DataFrameComponent::MODULES_STATUSES, frame);
  addComponent(DataFrameComponent::STREAM_STATS, frame);
  addComponent(DataFrameComponent::SERVER_STATS, frame);
  addComponent(DataFrameComponent::SYSTEM_INFO, frame);
  addComponent(DataFrameComponent::ENVIRONMENTAL_DATA, frame);
  addComponent(DataFrameComponent::TELEMETRY_DATA, frame);
  addComponent(DataFrameComponent::CALCULATED_DATA, frame);
  return frame.dump();
}

std::string
DataFormatter::partialFrame(std::vector<DataFrameComponent> components) {
  Json frame;
  setupFrameStructure(frame, "data");

  for (auto component : components) {
    addComponent(component, frame);
  }

  return frame.dump();
}

void DataFormatter::setupFrameStructure(Json &frame,
                                        const std::string &body_field) {
  frame["timestamp"] = generateTimestamp();
  frame["uptime"] = shared_data_.misc.getUptimeString();
  frame[body_field] = Json::object();
}

void DataFormatter::addComponent(DataFrameComponent component, Json &frame) {
  switch (component) {
  case DataFrameComponent::MODULES_STATUSES:
    frame["data"]["modules_statuses"] =
        shared_data_.blocks.modules_statuses.get().toJson();
    break;
  case DataFrameComponent::STREAM_STATS:
    frame["data"]["stream_stats"] =
        shared_data_.blocks.stream_stats.get().toJson();
    break;
  case DataFrameComponent::SERVER_STATS:
    frame["data"]["server_stats"] =
        shared_data_.blocks.server_module_stats.get().toJson();
    break;
  case DataFrameComponent::SYSTEM_INFO:
    frame["data"]["system_info"] =
        shared_data_.blocks.system_info.get().toJson();
    break;
  case DataFrameComponent::ENVIRONMENTAL_DATA:
    frame["environmental"]["temperature"] =
        data::toJson(shared_data_.frames.env_temp);
    frame["environmental"]["pressure"] =
        data::toJson(shared_data_.frames.env_pres);
    frame["environmental"]["humidity"] =
        data::toJson(shared_data_.frames.env_hum);
    break;
  case DataFrameComponent::TELEMETRY_DATA:
    frame["data"]["telemetry"] =
        shared_data_.blocks.telemetry_module_stats.get().toJson();
    break;
  case DataFrameComponent::CALCULATED_DATA:
    frame["data"]["calculated"] =
        shared_data_.blocks.calculated_data.get().toJson();
  }
}

std::string
DataFormatter::dataPacketToJsonString(const data::DataPacket &packet) const {
  std::string source;
  std::string id;
  std::string timestamp;
  std::string value;

  if (packet.source == node::Identification::EXTENSION) {
    // Extensions all have the same source, so use the secondary.
    source = "ext." + packet.secondary_identifier;
  } else {
    source = node::K_IDENTIFICATION_TO_STRING_MAP.at(packet.source);
  }

  id = BoosterSeat::string::intToHex(static_cast<uint16_t>(packet.identifier),
                                     4, false, true);

  value = packet.value;
  auto time = packet.created_time;
  timestamp = generateTimestamp(time);
  timestamp = BoosterSeat::time::timeString(BoosterSeat::time::TimeZone::LOCAL,
                                            ':', packet.created_time);

  // Format into json
  Json data_packet = {
      {"src", source}, {"id", id}, {"ts", timestamp}, {"val", value}};
  return data_packet.dump();
}

Json DataFormatter::fullFrameLogPacketToJson(
    const data::ErrorFrameItem &item) const {

  std::string id = BoosterSeat::string::intToHex(
      static_cast<uint16_t>(item.packet.id), 4, false);
  std::string first = BoosterSeat::time::timeString(
      BoosterSeat::time::TimeZone::LOCAL, ':', item.first_reported);
  std::string last = BoosterSeat::time::timeString(
      BoosterSeat::time::TimeZone::LOCAL, ':', item.last_reported);
  int count = item.occurrences;

  // clang-format off
  Json output = {
    {"id", id},
    {"first", first},
    {"last", last},
    {"count", count}
  };
  // clang-format on

  return output;
}

std::string DataFormatter::fullErrorFrame() {
  Json full_frame = {
      {"timestamp", generateTimestamp()},
      {"uptime", shared_data_.misc.getUptimeString()},
      {"errors", Json::array()},
  };

  auto error_frame = shared_data_.frames.error_frame.getFullFrame();
  for (auto error_pair : error_frame) {
    full_frame["errors"].push_back(fullFrameLogPacketToJson(error_pair.second));
  }

  return full_frame.dump();
}

std::string
DataFormatter::logPacketToJsonString(const data::LogPacket &packet) const {
  std::string timestamp;
  std::string level;
  std::string source;
  std::string id;
  std::string info;

  if (packet.source == node::Identification::EXTENSION) {
    // Extensions all have the same source, so use the secondary.
    source = "ext." + packet.secondary_identifier;
  } else {
    source = node::K_IDENTIFICATION_TO_STRING_MAP.at(packet.source);
  }

  id = BoosterSeat::string::intToHex(static_cast<uint16_t>(packet.id), 4, false,
                                     true);

  level = util::to_string(packet.level);

  info = packet.info;
  auto time = packet.created_time;
  timestamp = generateTimestamp(time);
  timestamp = BoosterSeat::time::timeString(BoosterSeat::time::TimeZone::LOCAL,
                                            ':', packet.created_time);

  // Format into json
  // clang-format off
  Json data_packet = {
    {"lvl", level},
    {"src", source},
    {"id", id},
    {"ts", timestamp},
    {"info", info}
  };
  // clang-format on
  return data_packet.dump();
}

std::string DataFormatter::generateTimestamp(
    const BoosterSeat::clck::TimePoint time_point) const {
  cfg::gEnum::TimestampDetail detail =
      config_.data_module_data.getTimestampDetail();
  cfg::gEnum::TimestampTimezone timezone =
      config_.data_module_data.getTimestampTimezone();

  BoosterSeat::time::TimeZone tz;
  if (timezone == cfg::gEnum::TimestampTimezone::UTC) {
    tz = BoosterSeat::time::TimeZone::UTC;
  } else {
    tz = BoosterSeat::time::TimeZone::LOCAL;
  }

  if (detail == cfg::gEnum::TimestampDetail::DATE_HHMMSS) {
    return bst::dateAndTimeString(tz, '-', '_', ':', time_point);
  } else {
    return bst::timeString(tz, ':', time_point);
  }
}