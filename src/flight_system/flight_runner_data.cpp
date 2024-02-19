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

namespace giraffe {

inline constexpr char kDataFilePath[] = "flight_runner_data.json";
inline constexpr uint32_t kNumStartupsDefault = 0;

NLOHMANN_JSON_SERIALIZE_ENUM(
    FlightRunnerData::ShutdownReason,
    {
        {FlightRunnerData::ShutdownReason::CRASH_OR_UNKNOWN,
         "crash_or_unknown"},
        {FlightRunnerData::ShutdownReason::CTRL_C, "ctrl_c"},
        {FlightRunnerData::ShutdownReason::TELEMETRY_SDN_COMMAND,
         "telemetry_sdn_command"},
    })

NLOHMANN_JSON_SERIALIZE_ENUM(FlightPhase,
                             {
                                 {FlightPhase::UNKNOWN, "unknown"},
                                 {FlightPhase::LAUNCH, "prelaunch"},
                                 {FlightPhase::ASCENT, "ascent"},
                                 {FlightPhase::DESCENT, "descent"},
                                 {FlightPhase::RECOVERY, "landing"},
                             })

FlightRunnerData::FlightRunnerData() {
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
  num_startups_ =
      getJsonValue<uint32_t>(data_json, "num_startups", kNumStartupsDefault);

  startup_time_.setToNow(); // Set the startup time.

  // Attempt to load the old startup and shutdown timestamps. If they do not
  // already exist, set to the current time.
  std::string startup_time_str = getJsonValue<std::string>(
      data_json, "startup_time", startup_time_.toString());
  if (previous_startup_time_.fromString(startup_time_str)) {
    previous_startup_time_valid_ = true;
  }
  std::string shutdown_time_str = getJsonValue<std::string>(
      data_json, "shutdown_time", previous_shutdown_time_.toString());
  if (previous_shutdown_time_.fromString(startup_time_str)) {
    previous_shutdown_time_valid_ = true;
  }

  // Load the shutdown reason.
  try {
    shutdown_reason_ = data_json.at("shutdown_reason").get<ShutdownReason>();
  } catch (std::exception &e) {
    shutdown_reason_ = ShutdownReason::CRASH_OR_UNKNOWN;
  }

  // Load the flight phase.
  try {
    flight_phase_ = data_json.at("flight_phase").get<FlightPhase>();
  } catch (std::exception &e) {
    flight_phase_ = FlightPhase::UNKNOWN;
  }

  // Save the data to the file.
  saveData();
}

FlightRunnerData::~FlightRunnerData() {
  saveData(true); // signal that this is a shutdown save. (pass true)
}

void FlightRunnerData::fullReset() {
  num_startups_ = kNumStartupsDefault;

  saveData();
}

void FlightRunnerData::incrementNumStartups() {
  num_startups_++;
  saveData();
}

uint32_t FlightRunnerData::getNumStartups() const {
  return num_startups_;
}

void FlightRunnerData::setShutdownReason(ShutdownReason shutdown_reason) {
  shutdown_reason_ = shutdown_reason;
  saveData();
}

void FlightRunnerData::setFlightPhase(FlightPhase flight_phase) {
  flight_phase_ = flight_phase;
  saveData();
}

FlightPhase FlightRunnerData::getFlightPhase() const {
  return flight_phase_;
}

bool FlightRunnerData::getSecondsSinceStartup(int64_t &num_seconds) {
  num_seconds = startup_time_.secondsFromNow();
  return true;
}

bool FlightRunnerData::getSecondsSincePreviousStartup(int64_t &num_seconds) {
  if (!previous_startup_time_valid_) {
    return false;
  }
  num_seconds = previous_startup_time_.secondsFromNow();
  return true;
}

bool FlightRunnerData::getSecondsSincePreviousShutdown(int64_t &num_seconds) {
  if (!previous_shutdown_time_valid_) {
    return false;
  }
  num_seconds = previous_shutdown_time_.secondsFromNow();
  return true;
}

void FlightRunnerData::saveData(bool shutdown_save) {
  Json data_json;
  data_json["startup_time"] = startup_time_.toString();
  if (shutdown_save) {
    previous_shutdown_time_.setToNow();
  }
  data_json["shutdown_time"] = previous_shutdown_time_.toString();
  data_json["shutdown_reason"] = Json(shutdown_reason_);
  data_json["num_startups"] = num_startups_;
  data_json["flight_phase"] = Json(flight_phase_);

  try {
    std::ofstream data_file;
    data_file.open(kDataFilePath);
    data_file << data_json.dump(1); // indent with 1 space
    data_file.close();
    save_status_ = true;
  } catch (std::exception &e) {
    save_status_ = false;
  }
}

}; // namespace giraffe