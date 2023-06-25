#include "shared_data.hpp"

json data::to_json(data::Frame<std::string, data::DataPacket> &frame) {
  json j;
  for (auto &item : frame.getAll()) {
    j[item.first] = item.second.value;
  }
  return j;
}