#include "shared_data.hpp"

Json data::to_json(data::Frame<std::string, data::DataPacket> &frame) {
  Json json_data;
  for (auto &item : frame.getAll()) {
    json_data[item.first] = item.second.value;
  }
  return json_data;
}