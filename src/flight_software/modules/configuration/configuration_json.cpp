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
#include <type_traits>

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
  json data({{"telemetry_packets", aprs_section.telemetry_packets},
             {"position_packets", aprs_section.position_packets},
             {"frequency", aprs_section.frequency.getFrequency()},
             {"ssid", aprs_section.ssid},
             {"destination_address", aprs_section.destination_address},
             {"destination_ssid", aprs_section.destination_ssid},
             {"symbol_table",
              cfg::kAprsSymbolTableToString.at(aprs_section.symbol_table)},
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
 * @details This one is a bit of a beast, but it does typechecking with nlohmann
 * json depending on the type. This little bit of logic is at compile time
 * thanks to `if constexpr` in C++17.
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
template <typename T>
void setValidValue(
    data::ErrorStream &es, const json &json_data, const std::string &section,
    const std::string &key, T &value_to_set,
    const std::string &error_code_prefix, int &errors_accumulator,
    std::function<bool(const T &, std::string &)> validator =
        [](const T &val, std::string &error) {
          (void)val;
          (void)error;
          return false;  // for booleans only as the validator is not called
        }) {
  // check for key existence
  if (!json_data.contains(key)) {
    cfg::reportError(es, "JT_" + error_code_prefix + "_NF",
                     section + "[" + key + "] not found");
    errors_accumulator++;
    return;
  }

  // Ensure that the value at this key is actually an int
  bool valid_type = false;

  if constexpr (std::is_same<T, int>::value) {
    valid_type = json_data[key].is_number_integer();
  } else if constexpr (std::is_same<T, std::string>::value) {
    valid_type = json_data[key].is_string();
  } else if constexpr (std::is_same<T, bool>::value) {
    valid_type = json_data[key].is_boolean();
  }

  if (!valid_type) {
    cfg::reportError(es, "JT_" + error_code_prefix + "_IT",
                     section + "[" + key + "] has an invalid type");
    errors_accumulator++;
    return;
  }

  // read the value
  T value = json_data[key].get<T>();

  // call the validator function that was passed in
  if constexpr (!std::is_same<T, bool>::value) {
    std::string error;
    if (!validator(value, error)) {
      cfg::reportError(es, "JT_" + error_code_prefix + "_VF",
                       section + "[" + key + "] failed validation: " + error);
      errors_accumulator++;
      return;
    }
  }

  // finally, set the value
  value_to_set = value;
  return;
}

template <typename T>
void setValidEnum(
    data::ErrorStream &es, const json &json_data, const std::string &section,
    const std::string &key, T &value_to_set,
    const std::unordered_map<std::string, T> string_to_value,
    const std::string &error_code_prefix, int &errors_accumulator,
    std::function<bool(const std::string &, std::string &)> validator) {
  // Check if the setting key is in the json data
  if (!json_data.contains(key)) {
    cfg::reportError(es, "JT_" + error_code_prefix + "_NF",
                     section + "[" + key + "] not found");
    errors_accumulator++;
    return;
  }

  // Ensure that the value at this key is actually a string before reading
  if (!json_data[key].is_string()) {
    cfg::reportError(es, "JT_" + error_code_prefix + "_IT",
                     section + "[" + key + "] has an invalid type");
    errors_accumulator++;
    return;
  }

  // read the value
  std::string value = json_data[key].get<std::string>();

  // call the validator function that was passed in
  std::string error;
  if (!validator(value, error)) {
    cfg::reportError(es, "JT_" + error_code_prefix + "_VF",
                     section + "[" + key + "] failed validation: " + error);
    errors_accumulator++;
    return;
  }

  // finally, set the value with the string to enum value map
  value_to_set = string_to_value.at(value);
}

void cfg::json::jsonToGeneral(const json &json_data, cfg::General &general,
                              data::ErrorStream &es, int &num_errors) {
  setValidValue<std::string>(es, json_data, "general", "project_name",
                             general.project_name, "GEN_PN", num_errors,
                             &cfg::general::validators::projectName);

  setValidEnum(es, json_data, "general", "main_board", general.main_board_type,
               cfg::kStringToMainBoard, "GEN_MB", num_errors,
               &cfg::general::validators::mainBoard);

  setValidEnum(es, json_data, "general", "starting_procedure",
               general.starting_procedure, cfg::kStringToProcedureType,
               "LD_GEN_SP", num_errors,
               &cfg::general::validators::startingProcedure);
}

void cfg::json::jsonToDebug(const json &json_data, cfg::Debug &debug,
                            data::ErrorStream &es, int &num_errors) {
  setValidValue<bool>(es, json_data, "debug", "console_enabled",
                      debug.console_enabled, "DBG_CE", num_errors);

  setValidValue<bool>(es, json_data, "debug", "print_errors",
                      debug.print_errors, "DBG_PE", num_errors);

  setValidValue<int>(es, json_data, "debug", "console_update_interval",
                     debug.console_update_interval, "DBG_CUI", num_errors,
                     &cfg::debug::validators::consoleUpdateInterval);
}

void cfg::json::jsonToServer(const json &json_data, cfg::Server &server,
                             data::ErrorStream &es, int &num_errors) {
  setValidValue<int>(es, json_data, "server", "tcp_socket_port",
                     server.tcp_socket_port, "SRV_TSP", num_errors,
                     &cfg::server::validators::tcpSocketPort);
}

void cfg::json::jsonToTelemetry(const json &json_data,
                                cfg::Telemetry &telemetry,
                                data::ErrorStream &es, int &num_errors) {
  setValidValue<bool>(es, json_data, "telemetry", "telemetry_enabled",
                      telemetry.telemetry_enabled, "TLM_TE", num_errors);

  setValidValue<std::string>(es, json_data, "telemetry", "call_sign",
                             telemetry.call_sign, "TLM_CS", num_errors,
                             cfg::telemetry::validators::callSign);
}

void cfg::json::jsonToAprs(const json &json_data, cfg::Aprs &aprs,
                           data::ErrorStream &es, int &num_errors) {
  setValidValue<bool>(es, json_data, "telemetry_aprs", "telemetry_packets",
                      aprs.telemetry_packets, "APRS_TPE", num_errors);

  setValidValue<bool>(es, json_data, "telemetry_aprs", "position_packets",
                      aprs.position_packets, "APRS_PPE", num_errors);

  std::string freq = aprs.frequency.getFrequency();
  setValidValue<std::string>(es, json_data, "telemetry_aprs", "frequency", freq,
                             "APRS_FQ", num_errors,
                             &cfg::telemetry::validators::frequency);
  aprs.frequency.setFrequency(freq);

  setValidValue<int>(es, json_data, "telemetry_aprs", "ssid", aprs.ssid,
                     "APRS_SSID", num_errors, &cfg::aprs::validators::ssid);

  setValidValue<std::string>(es, json_data, "telemetry_aprs",
                             "destination_address", aprs.destination_address,
                             "APRS_DA", num_errors,
                             &cfg::aprs::validators::destinationAddress);

  setValidValue<int>(es, json_data, "telemetry_aprs", "destination_ssid",
                     aprs.destination_ssid, "APRS_DST_SSID", num_errors,
                     &cfg::aprs::validators::ssid);

  setValidEnum(es, json_data, "telemetry_aprs", "symbol_table",
               aprs.symbol_table, cfg::kStringToAprsSymbolTable, "APRS_SYM_T",
               num_errors, &cfg::aprs::validators::symbolTable);

  std::string letter(1, aprs.symbol);
  setValidValue<std::string>(es, json_data, "telemetry_aprs", "symbol", letter,
                             "APRS_SYM", num_errors,
                             &cfg::aprs::validators::symbol);
  aprs.symbol = letter[0];

  setValidValue<std::string>(es, json_data, "telemetry_aprs", "comment",
                             aprs.comment, "APRS_CMT", num_errors,
                             &cfg::aprs::validators::comment);
}

void cfg::json::jsonToSstv(const json &json_data, cfg::Sstv &sstv,
                           data::ErrorStream &es, int &num_errors) {
  setValidValue<bool>(es, json_data, "sstv", "enabled", sstv.enabled, "SSTV_EN",
                      num_errors);

  std::string freq = sstv.frequency.getFrequency();
  setValidValue<std::string>(es, json_data, "telemetry_sstv", "frequency", freq,
                             "SSTV_FQ", num_errors,
                             &cfg::telemetry::validators::frequency);
  sstv.frequency.setFrequency(freq);

  setValidEnum(es, json_data, "telemetry_sstv", "mode", sstv.mode,
               cfg::kStringToSstvMode, "SSTV_MD", num_errors,
               &cfg::sstv::validators::mode);

  setValidValue<bool>(es, json_data, "telemetry_sstv", "overlay_data",
                      sstv.overlay_data, "SSTV_OD", num_errors);
}

void cfg::json::jsonToDataPackets(const json &json_data,
                                  cfg::DataPackets &data_packets,
                                  data::ErrorStream &es, int &num_errors) {

  setValidValue<bool>(es, json_data, "telemetry_data_packets", "enabled",
                      data_packets.enabled, "DATPKTS_EN", num_errors);

  std::string freq = data_packets.frequency.getFrequency();
  setValidValue<std::string>(es, json_data, "telemetry_data_packets", "frequency", freq,
                             "DATPKTS_FQ", num_errors,
                             &cfg::telemetry::validators::frequency);
  data_packets.frequency.setFrequency(freq);

  setValidEnum(es, json_data, "telemetry_data_packets", "mode", data_packets.mode, cfg::kStringToDataPacketsMode,
               "DATPKTS_MD", num_errors, &cfg::data_packets::validators::mode);

  setValidValue<bool>(es, json_data, "telemetry_data_packets", "morse_call_sign", data_packets.morse_call_sign,
                      "DATPKTS_MCS", num_errors);

  setValidValue<std::string>(es, json_data, "telemetry_data_packets", "comment",
                             data_packets.comment, "DATPKTS_CMT", num_errors,
                             &cfg::data_packets::validators::comment);
}
