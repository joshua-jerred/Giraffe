/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   flight_runner_data.cpp
 * @brief  Implementation of non-volatile data for the flight runner.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-01-03
 * @copyright  2024 (license to be defined)
 */

#include <fstream>

#include "json.hpp"

#include "flight_runner_data.hpp"

inline constexpr char kDataFilePath[] = "flight_runner_data.json";

namespace giraffe {

FlightRunnerData::FlightRunnerData() {
  loadData();
}

FlightRunnerData::~FlightRunnerData() {
  saveData();
}

void FlightRunnerData::fullReset() {
  constexpr uint32_t kNumStartupsDefault = 0;

  num_startups_ = kNumStartupsDefault;

  saveData();
}

void FlightRunnerData::incrementNumStartups() {
  num_startups_++;
  saveData();
}

void FlightRunnerData::loadData() {
  std::ifstream data_file;
  Json data_json;

  try {
    data_file.open(kDataFilePath);
    data_json = Json::parse(data_file);
    data_file.close();
    load_status_ = true;
  } catch (std::exception &e) {
    load_status_ = false;
    return;
  }

  // Load the data from the file. If the key is not found, the default value
  // will be used.
  num_startups_ = getJsonValue<uint32_t>(data_json, "num_startups", 0);
}

void FlightRunnerData::saveData() {
  Json data_json;
  data_json["num_startups"] = num_startups_;

  try {
    std::ofstream data_file;
    data_file.open(kDataFilePath);
    data_file << data_json.dump();
    data_file.close();
    save_status_ = true;
  } catch (std::exception &e) {
    save_status_ = false;
  }
}

}; // namespace giraffe