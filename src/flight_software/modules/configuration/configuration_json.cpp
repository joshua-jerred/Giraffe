/**
 * @file config_json.cpp
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief Implementation of JSON handlers.
 * @details All "json to X" functions are "best effort" functions.
 * @date 2023-05-14
 * @copyright Copyright (c) 2023
 */

#include "configuration.h"
#include "configuration_internal.h"

using json = nlohmann::ordered_json;

// To Json
json cfg::json::allToJson(cfg::Configuration &cfg) {
  json all = {
      {"general", cfg::json::generalToJson(cfg.getGeneral())},
      {"debug", cfg::json::debugToJson(cfg.getDebug())},
      {"server", cfg::json::serverToJson(cfg.getServer())},
      {"telemetry", cfg::json::telemetryToJson(cfg.getTelemetry())},
      {"telemetry_aprs", cfg::json::aprsToJson(cfg.getAprs())},
      {"telemetry_sstv", cfg::json::sstvToJson(cfg.getSstv())},
      {"telemetry_data_packets",
       cfg::json::dataPacketsToJson(cfg.getDataPackets())},
  };
  return all;
}

json cfg::json::generalToJson(const cfg::General &general_section) {
  json data({{"project_name", general_section.project_name},
             {"main_board",
              cfg::kMainBoardToString.at(general_section.main_board_type)},
             {"starting_procedure", cfg::kProcedureTypeToString.at(
                                        general_section.starting_procedure)}});
  return data;
}

json cfg::json::debugToJson(const cfg::Debug &debug_section) {
  json data(
      {{"print_errors", debug_section.print_errors},
       {"console_enabled", debug_section.console_enabled},
       {"console_update_interval", debug_section.console_update_interval}});
  return data;
}

json cfg::json::serverToJson(const cfg::Server &server_section) {
  json data({{"tcp_socket_port", server_section.tcp_socket_port}});
  return data;
}

json cfg::json::telemetryToJson(const cfg::Telemetry &telemetry_section) {
  json data({{"telemetry_enabled", telemetry_section.telemetry_enabled},
             {"call_sign", telemetry_section.call_sign}});
  return data;
}

json cfg::json::aprsToJson(const cfg::Aprs &aprs_section) {
  json data(
      {{"telemetry_packets", aprs_section.telemetry_packets},
       {"position_packets", aprs_section.position_packets},
       {"frequency", aprs_section.frequency.getFrequency()},
       {"ssid", aprs_section.ssid},
       {"destination_address", aprs_section.destination_address},
       {"destination_ssid", aprs_section.destination_ssid},
       {"symbol_table", cfg::kAprsSymbolTable.at(aprs_section.symbol_table)},
       {"symbol", std::string(1, aprs_section.symbol)},
       {"comment", aprs_section.comment}});
  return data;
}

json cfg::json::sstvToJson(const cfg::Sstv &sstv_section) {
  json data({
      {"enabled", sstv_section.enabled},
      {"frequency", sstv_section.frequency.getFrequency()},
      {"mode", cfg::kSstvModeToString.at(sstv_section.mode)},
      {"overlay_data", sstv_section.overlay_data},
  });
  return data;
}

json cfg::json::dataPacketsToJson(
    const cfg::DataPackets &data_packets_section) {
  json data({
      {"enabled", data_packets_section.enabled},
      {"frequency", data_packets_section.frequency.getFrequency()},
      {"mode", cfg::kDataPacketsModeToString.at(data_packets_section.mode)},
      {"morse_call_sign", data_packets_section.morse_call_sign},
      {"comment", data_packets_section.comment},
  });
  return data;
}

// From JSON

inline bool validString(const json &json_data, const std::string &key,
                        std::string &error) {
  if (!json_data.contains(key)) {
    error += key + " not found in json data | ";
    return false;
  } else if (!json_data[key].is_string()) {
    error += key + " is not a valid string/enum value | ";
    return false;
  }
  return true;
}

inline bool validInt(const json &json_data, const std::string &key,
                     std::string &error) {
  if (!json_data.contains(key)) {
    error += key + " not found in json data | ";
    return false;
  } else if (!json_data[key].is_number_integer()) {
    error += key + " is not a valid int | ";
    return false;
  }
  return true;
}

inline bool validBool(const json &json_data, const std::string &key,
                      std::string &error) {
  if (!json_data.contains(key)) {
    error += key + " not found in json data | ";
    return false;
  } else if (!json_data[key].is_boolean()) {
    error += key + " is not a valid bool | ";
    return false;
  }

  return true;
}

bool cfg::json::jsonToGeneral(const json &json_data, cfg::General &general,
                              std::string &error) {
  error = "";

  if (validString(json_data, "project_name", error)) {
    general.project_name = json_data["project_name"].get<std::string>();
  }

  if (validString(json_data, "main_board", error)) {
    std::string val = json_data["main_board"].get<std::string>();

    if (cfg::general::validators::mainBoard(val, error)) {
      general.main_board_type = cfg::kStringToMainBoard.at(val);
    }
  }

  if (validString(json_data, "starting_procedure", error)) {
    std::string val = json_data["starting_procedure"].get<std::string>();

    if (cfg::general::validators::startingProcedure(val, error)) {
      general.starting_procedure = cfg::kStringToProcedureType.at(val);
    }
  }

  return error.empty();
}

bool cfg::json::jsonToDebug(const json &json_data, cfg::Debug &debug,
                            std::string &error) {
  (void)json_data;
  (void)debug;
  (void)error;

  return error.empty();
}

bool cfg::json::jsonToServer(const json &json_data, cfg::Server &server,
                             std::string &error) {
  (void)json_data;
  (void)server;
  (void)error;

  return error.empty();
}

bool cfg::json::jsonToTelemetry(const json &json_data,
                                cfg::Telemetry &telemetry, std::string &error) {
  (void)json_data;
  (void)telemetry;
  (void)error;

  return error.empty();
}

bool cfg::json::jsonToAprs(const json &json_data, cfg::Aprs &aprs,
                           std::string &error) {
  (void)json_data;
  (void)aprs;
  (void)error;

  return error.empty();
}

bool cfg::json::jsonToSstv(const json &json_data, cfg::Sstv &sstv,
                           std::string &error) {
  (void)json_data;
  (void)sstv;
  (void)error;

  return error.empty();
}

bool cfg::json::jsonToDataPackets(const json &json_data,
                                  cfg::DataPackets &data_packets,
                                  std::string &error) {
  (void)json_data;
  (void)data_packets;
  (void)error;

  return error.empty();
}
