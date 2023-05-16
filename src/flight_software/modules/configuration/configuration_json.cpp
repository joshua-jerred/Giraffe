/**
 * @file config_json.cpp
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief Implementation of JSON handlers.
 * @details All "json to X" functions are "best effort" functions.
 * @date 2023-05-14
 * @copyright Copyright (c) 2023
 */

#include <functional>
#include <string>

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

/**
 * @brief This function validates a setting string or enum before setting it.
 *
 * @param es - The error stream
 * @param json_data - The section of data that contains the key
 * @param section - The name of this section, only used for error reporting
 * @param key - The key related to the setting
 * @param string_to_set - A reference to the actual string variable to update
 * @param validator - A function specific to each setting. Validates the value.
 * @param error_code_prefix - Used for errors only
 * @param errors_accumulator - A integer to increment if there is an error
 */
void setValidString(
    data::ErrorStream &es, const json &json_data, const std::string &section,
    const std::string &key, std::string &string_to_set,
    std::function<bool(const std::string &, std::string &)> validator,
    const std::string &error_code_prefix, int &errors_accumulator) {
  // Check if the setting key is in the json data
  if (!json_data.contains(key)) {
    cfg::reportError(es, error_code_prefix + "_NF",
                     section + "[" + key + "] not found");
    errors_accumulator++;
    return;
  }

  // Ensure that the value at this key is actually a string before reading
  if (!json_data[key].is_string()) {
    cfg::reportError(es, error_code_prefix + "_IT",
                     section + "[" + key + "] has an invalid type");
    errors_accumulator++;
    return;
  }

  // read the value
  std::string value = json_data[key].get<std::string>();

  // call the validator function that was passed in
  std::string error;
  if (!validator(value, error)) {
    cfg::reportError(es, error_code_prefix + "_VF",
                     section + "[" + key + "] failed validation: " + error);
    errors_accumulator++;
    return;
  }

  // finally, set the value
  string_to_set = value;
}

template <typename T>
void setValidEnum(
    data::ErrorStream &es, const json &json_data, const std::string &section,
    const std::string &key, T &value_to_set,
    const std::unordered_map<std::string, T> string_to_value,
    std::function<bool(const std::string &, std::string &)> validator,
    const std::string &error_code_prefix, int &errors_accumulator) {
  // Check if the setting key is in the json data
  if (!json_data.contains(key)) {
    cfg::reportError(es, error_code_prefix + "_NF",
                     section + "[" + key + "] not found");
    errors_accumulator++;
    return;
  }

  // Ensure that the value at this key is actually a string before reading
  if (!json_data[key].is_string()) {
    cfg::reportError(es, error_code_prefix + "_IT",
                     section + "[" + key + "] has an invalid type");
    errors_accumulator++;
    return;
  }

  // read the value
  std::string value = json_data[key].get<std::string>();

  // call the validator function that was passed in
  std::string error;
  if (!validator(value, error)) {
    cfg::reportError(es, error_code_prefix + "_VF",
                     section + "[" + key + "] failed validation: " + error);
    errors_accumulator++;
    return;
  }

  // finally, set the value with the string to enum value map
  value_to_set = string_to_value.at(value);
}

inline bool setValidInt(const json &json_data, const std::string &key,
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

inline bool setValidBool(const json &json_data, const std::string &key,
                         std::string &error, bool &bool_to_set) {
  if (!json_data.contains(key)) {
    error += key + " not found in json data | ";
    return false;
  } else if (!json_data[key].is_boolean()) {
    error += key + " is not a valid bool | ";
    return false;
  } else {
    bool_to_set = json_data[key].get<bool>();
  }

  return true;
}

void cfg::json::jsonToGeneral(const json &json_data, cfg::General &general,
                              data::ErrorStream &es, int &num_errors) {
  setValidString(es, json_data, "general", "project_name", general.project_name,
                 &cfg::general::validators::projectName, "GEN_PN", num_errors);

  setValidEnum(es, json_data, "general", "main_board", general.main_board_type,
               cfg::kStringToMainBoard, &cfg::general::validators::mainBoard,
               "GEN_PN", num_errors);

  setValidEnum(es, json_data, "general", "starting_procedure",
               general.starting_procedure, cfg::kStringToProcedureType,
               &cfg::general::validators::startingProcedure, "GEN_PN",
               num_errors);
}

void cfg::json::jsonToDebug(const json &json_data, cfg::Debug &debug,
                 data::ErrorStream &es, int &num_errors) {
  //setValidBool(json_data, "console_enabled", error, debug.console_enabled);
  //setValidBool(json_data, "print_errors", error, debug.print_errors);

  // int val = 0;
  // if (validInt(json_data)) return error.empty();
}

void jsonToServer(const json &json_data, cfg::Server &server,
                  data::ErrorStream &es, int &num_errors) {
}

void jsonToTelemetry(const json &json_data, cfg::Telemetry &telemetry,
                     data::ErrorStream &es, int &num_errors) {
}

void jsonToAprs(const json &json_data, cfg::Aprs &aprs, data::ErrorStream &es,
                int &num_errors) {
}

void jsonToSstv(const json &json_data, cfg::Sstv &sstv, data::ErrorStream &es,
                int &num_errors) {
}

void jsonToDataPackets(const json &json_data, cfg::DataPackets &data_packets,
                       data::ErrorStream &es, int &num_errors) {
}
