/**
 * @file loader.cpp
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief Loads the configuration from a JSON file.
 * @date 2023-05-14
 * @copyright Copyright (c) 2023
 */

#include <filesystem>
#include <fstream>

#include "configuration.h"
#include "configuration_internal.h"
#include "streams.h"

using json = nlohmann::ordered_json;

void cfg::file::saveConfiguration(data::ErrorStream &es,
                                  cfg::Configuration &config,
                                  const std::string &file_path,
                                  bool overwrite) {
  if (!overwrite && std::filesystem::exists(file_path)) {
    throw cfg::ConfigurationException(
        "SV_NO_OW", "will not overwrite the configuration file");
  }
  std::ofstream out(file_path);
  if (out.fail()) {
    throw cfg::ConfigurationException("SV_FOF",
                                      "failed to open the file while saving");
  }
  constexpr int indent = 2;
  std::string data = cfg::json::allToJson(config).dump(indent);
  out << data;
}

inline bool valid_section(const json &config_json,
                          const std::string &section_key) {
  return config_json.contains(section_key) &&
         config_json[section_key].is_object();
}

void cfg::file::loadConfiguration(data::ErrorStream &es,
                                  cfg::Configuration &config,
                                  const std::string &file_path) {
  using json = nlohmann::ordered_json;

  if (!std::filesystem::exists(file_path)) {
    throw cfg::ConfigurationException(
        "LD_FDE", "while loading, file does not exist: " + file_path);
  }
  std::ifstream in(file_path);
  if (in.fail()) {
    throw cfg::ConfigurationException(
        "LD_FOF", "could not open the file while loading: " + file_path);
  }

  json parsed;
  try {
    parsed = json::parse(in);
  } catch (json::parse_error &e) {
    throw cfg::ConfigurationException("LD_PERR",
                                      "while parsing the file: " + file_path);
  }

  int num_errors = 0;

  if (valid_section(parsed, "general")) {
    cfg::General general = config.getGeneral();
    cfg::json::jsonToGeneral(parsed["general"], general, es, num_errors);
    config.setGeneral(general);
  } else {
    es.addError(data::Source::CONFIGURATION_MODULE, "LD_SNF_GEN");
  }

  // if (valid_section(parsed, "debug")) {
  //   std::string error;
  //   cfg::Debug debug = config.getDebug();

  //   if (!cfg::json::jsonToDebug(parsed["debug"], debug, error)) {
  //     es.addError(data::Source::CONFIGURATION_MODULE, "LD_PE_DBG", error);
  //   }
  //   config.setDebug(debug);
  // } else {
  //   es.addError(data::Source::CONFIGURATION_MODULE, "LD_SNF_DBG");
  // }

  // if (valid_section(parsed, "server")) {
  //   std::string error;
  //   cfg::Server server = config.getServer();

  //   if (!cfg::json::jsonToServer(parsed["server"], server, error)) {
  //     es.addError(data::Source::CONFIGURATION_MODULE, "LD_PE_SRV", error);
  //   }
  //   config.setServer(server);
  // } else {
  //   es.addError(data::Source::CONFIGURATION_MODULE, "LD_SNF_SRV");
  // }

  // if (valid_section(parsed, "telemetry")) {
  //   std::string error;
  //   cfg::Telemetry telem = config.getTelemetry();

  //   if (!cfg::json::jsonToTelemetry(parsed["telemetry"], telem, error)) {
  //     es.addError(data::Source::CONFIGURATION_MODULE, "LD_PE_TLM", error);
  //   }
  //   config.setTelemetry(telem);
  // } else {
  //   es.addError(data::Source::CONFIGURATION_MODULE, "LD_SNF_TLM");
  // }

  // if (valid_section(parsed, "telemetry_aprs")) {
  //   std::string error;
  //   cfg::Aprs aprs = config.getAprs();

  //   if (!cfg::json::jsonToAprs(parsed["telemetry_aprs"], aprs, error)) {
  //     es.addError(data::Source::CONFIGURATION_MODULE, "LD_PE_APRS", error);
  //   }
  //   config.setAprs(aprs);
  // } else {
  //   es.addError(data::Source::CONFIGURATION_MODULE, "LD_SNF_APRS");
  // }

  // if (valid_section(parsed, "telemetry_sstv")) {
  //   std::string error;
  //   cfg::Sstv sstv = config.getSstv();

  //   if (!cfg::json::jsonToSstv(parsed["telemetry_sstv"], sstv, error)) {
  //     es.addError(data::Source::CONFIGURATION_MODULE, "LD_PE_SSTV", error);
  //   }
  //   config.setSstv(sstv);
  // } else {
  //   es.addError(data::Source::CONFIGURATION_MODULE, "LD_SNF_SSTV");
  // }

  // if (valid_section(parsed, "telemetry_data_packets")) {
  //   std::string error;
  //   cfg::DataPackets data_packets = config.getDataPackets();

  //   if (!cfg::json::jsonToDataPackets(parsed["telemetry_data_packets"], data_packets, error)) {
  //     es.addError(data::Source::CONFIGURATION_MODULE, "LD_PE_DATPKTS", error);
  //   }
  //   config.setDataPackets(data_packets);
  // } else {
  //   es.addError(data::Source::CONFIGURATION_MODULE, "LD_SNF_DATPKTS");
  // }
}