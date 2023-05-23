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

cfg::Configuration::Configuration(data::SharedData &shared_data)
    : shared_data_(shared_data) {
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

// ----- Data Module
bool cfg::Configuration::setDataModuleGeneral(
    const cfg::DataModuleGeneral &section) {
  std::lock_guard<std::mutex> lock(config_lock_);

  std::string error;
  if (!cfg::dm_general::validators::framePurgeTime(section.frame_purge_time,
                                                   error)) {
    reportError("ST_DMG_FPT", error);
  } else {
    dm_general_.frame_purge_time = section.frame_purge_time;
  }

  saveConfig();
  return true;
}

cfg::DataModuleGeneral cfg::Configuration::getDataModuleGeneral() {
  std::lock_guard<std::mutex> lock(config_lock_);
  return dm_general_;
}

bool cfg::Configuration::setDataModuleDataLog(
    const cfg::DataModuleDataLog &section) {
  std::lock_guard<std::mutex> lock(config_lock_);

  std::string error;
  if (!cfg::dm_data_log::validators::logInterval(section.log_interval_ms,
                                                 error)) {
    reportError("ST_DMDL_LGI", error);
  } else {
    dm_data_log_.log_interval_ms = section.log_interval_ms;
  }

  if (!cfg::dm_data_log::validators::maxFileSize(
          section.max_data_log_file_size_mb, error)) {
    reportError("ST_DMDL_MFS", error);
  } else {
    dm_data_log_.max_data_log_file_size_mb = section.max_data_log_file_size_mb;
  }

  if (!cfg::dm_data_log::validators::maxArchiveSize(
          section.max_data_archive_size_mb, error)) {
    reportError("ST_DMDL_MAS", error);
  } else {
    dm_data_log_.max_data_archive_size_mb = section.max_data_archive_size_mb;
  }

  dm_data_log_.log_data_to_file = section.log_data_to_file;
  dm_data_log_.log_strategy = section.log_strategy;
  dm_data_log_.log_detail = section.log_detail;
  dm_data_log_.archival_method = section.archival_method;

  saveConfig();
  return true;
}

cfg::DataModuleDataLog cfg::Configuration::getDataModuleDataLog() {
  std::lock_guard<std::mutex> lock(config_lock_);
  return dm_data_log_;
}

bool cfg::Configuration::setDataModuleInfluxDb(
    const cfg::DataModuleInfluxDb &section) {
  std::lock_guard<std::mutex> lock(config_lock_);

  std::string error;
  if (!cfg::dm_influx_db::validators::url(section.url, error)) {
    reportError("ST_DMIFLX_URL", error);
  } else {
    dm_influxdb_.url = section.url;
  }

  if (!cfg::dm_influx_db::validators::token(section.token, error)) {
    reportError("ST_DMIFLX_TKN", error);
  } else {
    dm_influxdb_.token = section.token;
  }

  if (!cfg::dm_influx_db::validators::org(section.organization, error)) {
    reportError("ST_DMIFLX_ORG", error);
  } else {
    dm_influxdb_.organization = section.organization;
  }

  if (!cfg::dm_influx_db::validators::bucket(section.data_bucket, error)) {
    reportError("ST_DMIFLX_DBKT", error);
  } else {
    dm_influxdb_.data_bucket = section.data_bucket;
  }

  if (!cfg::dm_influx_db::validators::bucket(section.error_bucket, error)) {
    reportError("ST_DMIFLX_EBKT", error);
  } else {
    dm_influxdb_.error_bucket = section.error_bucket;
  }

  dm_influxdb_.influx_enabled = section.influx_enabled;
  dm_influxdb_.log_errors = section.log_errors;
  dm_influxdb_.retention_policy = section.retention_policy;

  saveConfig();
  return true;
}

cfg::DataModuleInfluxDb cfg::Configuration::getDataModuleInfluxDb() {
  std::lock_guard<std::mutex> lock(config_lock_);
  return dm_influxdb_;
}

bool cfg::Configuration::setDataModuleErrorLog(
    const cfg::DataModuleErrorLog &section) {
  std::lock_guard<std::mutex> lock(config_lock_);

  std::string error;
  if (!cfg::dm_data_log::validators::maxFileSize(
          section.max_error_log_file_size_mb, error)) {
    reportError("ST_DMEL_MFS", error);
  } else {
    dm_error_log_.max_error_log_file_size_mb =
        section.max_error_log_file_size_mb;
  }

  if (!cfg::dm_data_log::validators::maxArchiveSize(
          section.max_error_archive_size_mb, error)) {
    reportError("ST_DMEL_MAS", error);
  } else {
    dm_error_log_.max_error_archive_size_mb = section.max_error_archive_size_mb;
  }

  dm_error_log_.log_errors_to_file = section.log_errors_to_file;
  dm_error_log_.archival_method = section.archival_method;

  saveConfig();
  return true;
}

cfg::DataModuleErrorLog cfg::Configuration::getDataModuleErrorLog() {
  std::lock_guard<std::mutex> lock(config_lock_);
  return dm_error_log_;
}

bool cfg::Configuration::setDataModuleDebug(
    const cfg::DataModuleDebug &section) {
  std::lock_guard<std::mutex> lock(config_lock_);

  dm_debug_ = section;

  saveConfig();
  return true;
}

cfg::DataModuleDebug cfg::Configuration::getDataModuleDebug() {
  std::lock_guard<std::mutex> lock(config_lock_);
  return dm_debug_;
}

// ----- Console Module

bool cfg::Configuration::setConsoleModule(const cfg::ConsoleModule &section) {
  std::lock_guard<std::mutex> lock(config_lock_);

  std::string error;
  if (!cfg::console_module::validators::updateInterval(section.update_interval,
                                                       error)) {
    reportError("ST_CNSL_UI", error);
  } else {
    console_module_.update_interval = section.update_interval;
  }

  console_module_.enabled = section.enabled;

  saveConfig();
  return true;
}

cfg::ConsoleModule cfg::Configuration::getConsoleModule() {
  std::lock_guard<std::mutex> lock(config_lock_);
  return console_module_;
}

// ----- Server Module

bool cfg::Configuration::setServerModule(const cfg::ServerModule &server) {
  std::lock_guard<std::mutex> lock(config_lock_);

  std::string error;
  if (!cfg::server_module::validators::tcpSocketPort(server.tcp_socket_port,
                                                     error)) {
    reportError("ST_SRV_TSP", error);
  } else {
    server_module_.tcp_socket_port = server.tcp_socket_port;
  }

  saveConfig();
  return true;
}

cfg::ServerModule cfg::Configuration::getServerModule() {
  std::lock_guard<std::mutex> lock(config_lock_);
  return server_module_;
}

// ----- System Module

bool cfg::Configuration::setSystemModule(const cfg::SystemModule &section) {
  std::lock_guard<std::mutex> lock(config_lock_);

  std::string error;
  if (!cfg::system_module::validators::systemInfoPollRate(
          section.system_info_poll_rate_ms, error)) {
    reportError("ST_SYS_SIPR", error);
  } else {
    system_module_.system_info_poll_rate_ms = section.system_info_poll_rate_ms;
  }

  saveConfig();
  return true;
}

cfg::SystemModule cfg::Configuration::getSystemModule() {
  std::lock_guard<std::mutex> lock(config_lock_);
  return system_module_;
}

// ----- Telemetry

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
  if (load_mode_) {
    return;
  }

  using json = nlohmann::ordered_json;

  std::ofstream out(kConfigurationPath);

  if (out.fail()) {
    reportError("SV_FOF", "failed to open the file while saving");
    return;
  }

  json all = {
      {"general", cfg::json::generalToJson(general_)},
      {"data_module_general", cfg::json::dataModuleGeneralToJson(dm_general_)},
      {"data_module_data_log",
       cfg::json::dataModuleDataLogToJson(dm_data_log_)},
      {"data_module_influxdb",
       cfg::json::dataModuleInfluxDbToJson(dm_influxdb_)},
      {"data_module_error_log",
       cfg::json::dataModuleErrorLogToJson(dm_error_log_)},
      {"data_module_debug", cfg::json::dataModuleDebugToJson(dm_debug_)},
      {"console_module", cfg::json::consoleModuleToJson(console_module_)},
      {"server_module", cfg::json::serverModuleToJson(server_module_)},
      {"system_module", cfg::json::systemModuleToJson(system_module_)},
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
    reportError("LD_FDE",
                "while loading, file does not exist: " + kConfigurationPath);
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
  load_mode_ = true;   // Prevent saving

  /*
  Now check that each section is a valid json object and if so, parse it
  with the json functions. These functions will add errors to the error stream.
  */

  data::ErrorStream &es = shared_data_.streams.error;

  if (valid_section(parsed, "general")) {
    cfg::General tmp_general = getGeneral();
    cfg::json::jsonToGeneral(parsed["general"], tmp_general, es, num_errors);
    setGeneral(tmp_general);
  } else {
    reportError("LD_SNF_GEN", "general section not found in config file");
  }

  if (valid_section(parsed, "data_module_general")) {
    cfg::DataModuleGeneral dm_gen = getDataModuleGeneral();
    cfg::json::jsonToDataModuleGeneral(parsed["data_module_general"], dm_gen,
                                       es, num_errors);
    setDataModuleGeneral(dm_gen);
  } else {
    reportError("LD_SNF_DM_GEN",
                "data module general section not found in config file");
  }

  if (valid_section(parsed, "data_module_data_log")) {
    cfg::DataModuleDataLog sec = getDataModuleDataLog();
    cfg::json::jsonToDataModuleDataLog(parsed["data_module_data_log"], sec, es,
                                       num_errors);
    setDataModuleDataLog(sec);
  } else {
    reportError("LD_SNF_DM_DL",
                "data module data log section not found in config file");
  }

  if (valid_section(parsed, "data_module_influxdb")) {
    cfg::DataModuleInfluxDb sec = getDataModuleInfluxDb();
    cfg::json::jsonToDataModuleInfluxDb(parsed["data_module_influxdb"], sec, es,
                                        num_errors);
    setDataModuleInfluxDb(sec);
  } else {
    reportError("LD_SNF_DM_IFXDB",
                "data module influxdb section not found in config file");
  }

  if (valid_section(parsed, "data_module_error_log")) {
    cfg::DataModuleErrorLog sec = getDataModuleErrorLog();
    cfg::json::jsonToDataModuleErrorLog(parsed["data_module_error_log"], sec,
                                        es, num_errors);
    setDataModuleErrorLog(sec);
  } else {
    reportError("LD_SNF_DM_EL",
                "data module error log section not found in config file");
  }

  if (valid_section(parsed, "data_module_debug")) {
    cfg::DataModuleDebug sec = getDataModuleDebug();
    cfg::json::jsonToDataModuleDebug(parsed["data_module_debug"], sec, es,
                                     num_errors);
    setDataModuleDebug(sec);
  } else {
    reportError("LD_SNF_DM_DBG",
                "data module error log section not found in config file");
  }

  if (valid_section(parsed, "console_module")) {
    cfg::ConsoleModule sec = getConsoleModule();
    cfg::json::jsonToConsoleModule(parsed["console_module"], sec, es,
                                   num_errors);
    setConsoleModule(sec);
  } else {
    reportError("LD_SNF_CM", "console_module section not found in config file");
  }

  if (valid_section(parsed, "server_module")) {
    cfg::ServerModule server = getServerModule();
    cfg::json::jsonToServerModule(parsed["server_module"], server, es,
                                  num_errors);
    setServerModule(server);
  } else {
    reportError("LD_SNF_SRV", "server_module section not found in config file");
  }

  if (valid_section(parsed, "system_module")) {
    cfg::SystemModule sec = getSystemModule();
    cfg::json::jsonToSystemModule(parsed["system_module"], sec, es, num_errors);
    setSystemModule(sec);
  } else {
    reportError("LD_SNF_SM", "system_module section not found in config file");
  }

  if (valid_section(parsed, "telemetry")) {
    cfg::Telemetry telem = getTelemetry();
    cfg::json::jsonToTelemetry(parsed["telemetry"], telem, es, num_errors);
    setTelemetry(telem);
  } else {
    reportError("LD_SNF_TLM", "telemetry section not found in config file");
  }

  if (valid_section(parsed, "telemetry_aprs")) {
    cfg::Aprs aprs = getAprs();
    cfg::json::jsonToAprs(parsed["telemetry_aprs"], aprs, es, num_errors);
    setAprs(aprs);
  } else {
    reportError("LD_SNF_APRS", "aprs section not found in config file");
  }

  if (valid_section(parsed, "telemetry_sstv")) {
    cfg::Sstv sstv = getSstv();
    cfg::json::jsonToSstv(parsed["telemetry_sstv"], sstv, es, num_errors);
    setSstv(sstv);
  } else {
    reportError("LD_SNF_SSTV", "sstv section not found in config file");
  }

  if (valid_section(parsed, "telemetry_data_packets")) {
    cfg::DataPackets data_packets = getDataPackets();
    cfg::json::jsonToDataPackets(parsed["telemetry_data_packets"], data_packets,
                                 es, num_errors);
    setDataPackets(data_packets);
  } else {
    reportError("LD_SNF_DATPKTS",
                "data packets section not found in config file");
  }

  load_mode_ = false;  // Allow saving again
}

void cfg::Configuration::reportError(std::string code, std::string info) {
  shared_data_.streams.error.addError(node::Identification::CONFIGURATION, code,
                                 info);
}