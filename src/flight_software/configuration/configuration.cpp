/**
 * @file configuration.cpp
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief
 * @date 2023-05-13
 * @copyright Copyright (c) 2023
 */

#include "configuration.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

#include "configuration_internal.h"

using json = nlohmann::ordered_json;

cfg::Configuration::Configuration(data::Streams &streams) : streams_(streams) {
  if (!std::filesystem::exists(kConfigurationPath)) {
    std::cout << "Generation Configuration File" << std::endl;
    std::lock_guard<std::mutex> lock(config_lock_);
    saveConfig();
  } else {
    loadConfig();  // Load the config from the default path
  }
}

cfg::Configuration::~Configuration() {
  std::lock_guard<std::mutex> lock(config_lock_);
}

// ----- General

bool cfg::Configuration::setGeneral(const cfg::General &general) {
  std::lock_guard<std::mutex> lock(config_lock_);

  bool error_free = true;

  std::string error;
  if (!cfg::general::validators::projectName(general.project_name, error)) {
    error_free = false;
    reportError("ST_GEN_PN", error);
  } else {
    general_.project_name = general.project_name;
  }

  general_.main_board_type = general.main_board_type;
  general_.starting_procedure = general.starting_procedure;

  saveConfig();
  return error_free;
}

cfg::General cfg::Configuration::getGeneral() {
  std::lock_guard<std::mutex> lock(config_lock_);
  return general_;
}

// ----- Debug

bool cfg::Configuration::setDebug(const cfg::Debug &debug) {
  std::lock_guard<std::mutex> lock(config_lock_);
  debug_ = debug;
  saveConfig();
  return true;
}

cfg::Debug cfg::Configuration::getDebug() {
  std::lock_guard<std::mutex> lock(config_lock_);
  cfg::Debug ret = debug_;
  return ret;
}

bool cfg::Configuration::setServer(const cfg::Server &server) {
  std::lock_guard<std::mutex> lock(config_lock_);

  std::string error;
  if (!cfg::server::validators::tcpSocketPort(server.tcp_socket_port, error)) {
    reportError("ST_SRV_TSP", error);
  }

  server_ = server;
  saveConfig();
  return true;
}

cfg::Server cfg::Configuration::getServer() {
  std::lock_guard<std::mutex> lock(config_lock_);
  return server_;
}

bool cfg::Configuration::setTelemetry(const cfg::Telemetry &telemetry) {
  std::lock_guard<std::mutex> lock(config_lock_);
  telemetry_ = telemetry;

  saveConfig();
  return true;
}

cfg::Telemetry cfg::Configuration::getTelemetry() {
  std::lock_guard<std::mutex> lock(config_lock_);
  return telemetry_;
}

bool cfg::Configuration::setAprs(const cfg::Aprs &aprs) {
  std::lock_guard<std::mutex> lock(config_lock_);
  aprs_ = aprs;
  saveConfig();
  return true;
}

cfg::Aprs cfg::Configuration::getAprs() {
  std::lock_guard<std::mutex> lock(config_lock_);
  return aprs_;
}

bool cfg::Configuration::setSstv(const cfg::Sstv &sstv) {
  std::lock_guard<std::mutex> lock(config_lock_);
  sstv_ = sstv;
  saveConfig();
  return true;
}

cfg::Sstv cfg::Configuration::getSstv() {
  std::lock_guard<std::mutex> lock(config_lock_);
  return sstv_;
}

bool cfg::Configuration::setDataPackets(const cfg::DataPackets &data_packets) {
  std::lock_guard<std::mutex> lock(config_lock_);
  data_packets_ = data_packets;
  saveConfig();
  return true;
}

cfg::DataPackets cfg::Configuration::getDataPackets() {
  std::lock_guard<std::mutex> lock(config_lock_);
  return data_packets_;
}

/**
 * @brief Save the configuration. Lock must be held before calling this
 * function.
 *
 * @param file_path - optional, if empty it will save to the default location.
 */
void cfg::Configuration::saveConfig() {
  const std::string kTestStr = "gfs_config.json";

  using json = nlohmann::ordered_json;

  std::ofstream out(kConfigurationPath);

  if (out.fail()) {
    reportError("SV_FOF", "failed to open the file while saving");
    return;
  }

  json all = {
      {"general", cfg::json::generalToJson(general_)},
      {"debug", cfg::json::debugToJson(debug_)},
      {"server", cfg::json::serverToJson(server_)},
      {"telemetry", cfg::json::telemetryToJson(telemetry_)},
      {"telemetry_aprs", cfg::json::aprsToJson(aprs_)},
      {"telemetry_sstv", cfg::json::sstvToJson(sstv_)},
      {"telemetry_data_packets", cfg::json::dataPacketsToJson(data_packets_)},
  };

  constexpr int json_indent = 2;
  std::string data = all.dump(json_indent);
  out << data;
}

/**
 * @brief This function with check if a JSON key is both in a JSON dictionary
 * and that it's a JSON object.
 * @param config_json The parsed JSON data to check.
 * @param section_key The key to the json object.
 * @return true - The key exists, and it relates to a JSON object.
 * @return false - Either the key does not exist, or it's not an object.
 */
inline bool valid_section(const json &config_json,
                          const std::string &section_key) {
  return config_json.contains(section_key) &&
         config_json[section_key].is_object();
}

/**
 * @brief Best effort loading.
 * @param file_path The optional file path to load from. If empty, it will
 * use the default path.
 */
void cfg::Configuration::loadConfig() {
  // Check if the file exists, if not report the error and return early.
  if (!std::filesystem::exists(kConfigurationPath)) {
    reportError("LD_FDE", "while loading, file does not exist: " + kConfigurationPath);
    return;
  }

  std::ifstream in(kConfigurationPath);  // Attempt to open the file
  // If the file failed to open, report the error and return early.
  if (in.fail()) {
    reportError("LD_FOF",
                "could not open the file while loading: " + kConfigurationPath);
    return;
  }

  // Next, attempt to parse the file as json data. If it fails to parse
  // report the error and return early.
  json parsed;
  try {
    parsed = json::parse(in);
  } catch (json::parse_error &e) {
    reportError("LD_PE", "while parsing the file: " + kConfigurationPath);
    return;
  }

  int num_errors = 0;  // Number or errors accumulator

  /*
  Now check that each section is a valid json object and if so, parse it
  with the json functions. These functions will add errors to the error stream.
  */

  if (valid_section(parsed, "general")) {
    cfg::General tmp_general = getGeneral();
    cfg::json::jsonToGeneral(parsed["general"], tmp_general,
                             streams_.error_stream, num_errors);
    setGeneral(tmp_general);
  } else {
    reportError("LD_SNF_GEN", "general section not found in config file");
  }

  if (valid_section(parsed, "debug")) {
    cfg::Debug debug = getDebug();
    cfg::json::jsonToDebug(parsed["debug"], debug, streams_.error_stream,
                           num_errors);
    setDebug(debug);
  } else {
    reportError("LD_SNF_DBG", "debug section not found in config file");
  }

  if (valid_section(parsed, "server")) {
    cfg::Server server = getServer();
    cfg::json::jsonToServer(parsed["server"], server, streams_.error_stream,
                            num_errors);
    setServer(server);
  } else {
    reportError("LD_SNF_SRV", "server section not found in config file");
  }

  if (valid_section(parsed, "telemetry")) {
    cfg::Telemetry telem = getTelemetry();
    cfg::json::jsonToTelemetry(parsed["telemetry"], telem,
                               streams_.error_stream, num_errors);
    setTelemetry(telem);
  } else {
    reportError("LD_SNF_TLM", "telemetry section not found in config file");
  }

  if (valid_section(parsed, "telemetry_aprs")) {
    cfg::Aprs aprs = getAprs();
    cfg::json::jsonToAprs(parsed["telemetry_aprs"], aprs, streams_.error_stream,
                          num_errors);
    setAprs(aprs);
  } else {
    reportError("LD_SNF_APRS", "aprs section not found in config file");
  }

  if (valid_section(parsed, "telemetry_sstv")) {
    cfg::Sstv sstv = getSstv();
    cfg::json::jsonToSstv(parsed["telemetry_sstv"], sstv, streams_.error_stream,
                          num_errors);
    setSstv(sstv);
  } else {
    reportError("LD_SNF_SSTV", "sstv section not found in config file");
  }

  if (valid_section(parsed, "telemetry_data_packets")) {
    cfg::DataPackets data_packets = getDataPackets();
    cfg::json::jsonToDataPackets(parsed["telemetry_data_packets"], data_packets,
                                 streams_.error_stream, num_errors);
    setDataPackets(data_packets);
  } else {
    reportError("LD_SNF_DATPKTS",
                "data packets section not found in config file");
  }
}

void cfg::Configuration::reportError(std::string code, std::string info) {
  streams_.error_stream.addError(node::Identification::CONFIGURATION, code,
                                 info);
}