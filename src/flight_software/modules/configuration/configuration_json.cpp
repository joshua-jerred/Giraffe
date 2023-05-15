/**
 * @file config_json.cpp
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief Implementation of JSON handlers.
 * @date 2023-05-14
 * @copyright Copyright (c) 2023
 */

#include "configuration.h"
#include "configuration_internal.h"

using json = nlohmann::ordered_json;

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
             {"main_board", general_section.main_board_type},
             {"starting_procedure", general_section.starting_procedure}});
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
             {"symbol_table", aprs_section.symbol_table},
             {"symbol", aprs_section.symbol},
             {"comment", aprs_section.comment}});
  return data;
}

json cfg::json::sstvToJson(const cfg::Sstv &sstv_section) {
  json data({
      {"enabled", sstv_section.enabled},
      {"frequency", sstv_section.frequency.getFrequency()},
      {"mode", sstv_section.mode},
      {"overlay_data", sstv_section.overlay_data},
  });
  return data;
}

json cfg::json::dataPacketsToJson(
    const cfg::DataPackets &data_packets_section) {
  json data({
      {"enabled", data_packets_section.enabled},
      {"frequency", data_packets_section.frequency.getFrequency()},
      {"mode", data_packets_section.mode},
      {"morse_call_sign", data_packets_section.morse_call_sign},
      {"comment", data_packets_section.comment},
  });
  return data;
}