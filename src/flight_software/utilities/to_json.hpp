#ifndef TO_JSON_HPP_
#define TO_JSON_HPP_

#include "blocks.hpp"
#include <nlohmann/json.hpp>
#include <BoosterSeat/numbers.hpp>

using json = nlohmann::ordered_json;
inline auto rnd = BoosterSeat::numbers::doubleToPrecisionTwo;

static json to_json(const data::blocks::SystemInfo &system_info) {
  json j;
  j["uptime_hours"] = rnd(system_info.uptime_hours);
  j["cpu_load_avg_1"] = rnd(system_info.cpu_load_avg_1);
  j["cpu_load_avg_5"] = rnd(system_info.cpu_load_avg_5);
  j["cpu_load_avg_15"] = rnd(system_info.cpu_load_avg_15);
  j["cpu_temp_c"] = rnd(system_info.cpu_temp_c);
  j["mem_total_gb"] = rnd(system_info.mem_total_gb);
  j["mem_free_gb"] = rnd(system_info.mem_free_gb);
  j["mem_used_percent"] = rnd(system_info.mem_used_percent);
  j["swap_total_gb"] = rnd(system_info.swap_total_gb);
  j["swap_free_gb"] = rnd(system_info.swap_free_gb);
  j["swap_used_percent"] = rnd(system_info.swap_used_percent);
  j["disk_total_gb"] = rnd(system_info.disk_total_gb);
  j["disk_free_gb"] = rnd(system_info.disk_free_gb);
  j["disk_used_percent"] = rnd(system_info.disk_used_percent);
  return j;
}

#endif /* TO_JSON_HPP_ */