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

#include "data_formatting.hpp"
#include <BoosterSeat/time.hpp>

namespace bst = BoosterSeat::time;
using namespace data_middleware;

std::string DataFormatter::fullFrame() {
  json frame;
  setupFrameStructure(frame);
  addComponent(DataFrameComponent::MODULES_STATUSES, frame);
  addComponent(DataFrameComponent::STREAM_STATS, frame);
  addComponent(DataFrameComponent::SERVER_STATS, frame);
  addComponent(DataFrameComponent::SYSTEM_INFO, frame);
  addComponent(DataFrameComponent::ENVIRONMENTAL_DATA, frame);
  return frame.dump();
}

std::string
DataFormatter::partialFrame(std::vector<DataFrameComponent> components) {
  json frame;
  setupFrameStructure(frame);

  for (auto component : components) {
    addComponent(component, frame);
  }

  return frame.dump();
}

void DataFormatter::setupFrameStructure(json &frame) {
  frame["timestamp"] = bst::dateAndTimeString();
  frame["uptime"] = shared_data_.misc.getUptimeString();
  frame["data"] = json::object();
}

void DataFormatter::addComponent(DataFrameComponent component, json &frame) {
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
        data::to_json(shared_data_.frames.env_temp);
    frame["environmental"]["pressure"] =
        data::to_json(shared_data_.frames.env_pres);
    frame["environmental"]["humidity"] =
        data::to_json(shared_data_.frames.env_hum);
    break;
  }
}