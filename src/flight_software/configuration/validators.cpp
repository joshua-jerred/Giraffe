/**
 * @file configuration.cpp
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief The implementation of the new (0.5) configuration class.
 * @date 2023-05-10
 * @copyright Copyright (c) 2023
 */

#include <regex>
#include <string>

#include "configuration_internal.h"

// enumerations
static const std::array<std::string, 3> main_board_enum = {
    "other", "pi_zero_w_2", "pi_4"};

static const std::array<std::string, 6> procedure_name_enum = {
    "testing", "pre_launch", "ascent", "descent", "recovery", "failsafe"};

static const std::array<std::string, 4> log_strategy_enum = {
    "interval", "all", "selection_interval", "selection_all"};

static const std::array<std::string, 2> log_detail_enum = {"full", "partial"};

static const std::array<std::string, 1> archive_method_enum = {"plain_text"};

static const std::array<std::string, 6> influx_retention_policy_enum = {
    "1 Hour", "1 Day", "1 Week", "1 Month", "1 Year", "INF"};

static const std::array<std::string, 3> debug_levels_enum = {
  "error", "warn", "info"
};

static const std::array<std::string, 2> aprs_symbol_table = {"primary",
                                                             "alternate"};

static const std::array<std::string, 1> sstv_modes_enum = {"robot36"};

static const std::array<std::string, 8> data_packet_modes_enum = {
    "bpsk125", "bpsk250", "bpsk500", "bpsk1000",
    "qpsk125", "qpsk250", "qpsk500", "afsk_ax25"};

// Checking Utilities
template <std::size_t SIZE>
inline bool macroInOptions(const std::array<std::string, SIZE> &options,
                           const std::string value_to_compare) {
  if (std::find(std::begin(options), std::end(options), value_to_compare) !=
      std::end(options)) {
    return true;
  }
  return false;
}

// General
/**
 * @todo This guy still accepts illegal characters
 *
 */
bool cfg::general::validators::projectName(const std::string &project_name,
                                           std::string &error) {
  const std::string pattern = "^(?!\\s)[a-zA-Z0-9_ -]{0,19}[^\\s]$";
  constexpr unsigned int min = 1;
  constexpr unsigned int max = 20;

  if (!inRange(min, max, project_name)) {
    error = "project_name is out of acceptable length range.";
    return false;
  }

  if (!matchesPattern(pattern, project_name)) {
    error = "project_name does not match regex pattern.";
    return false;
  }

  return true;
}

bool cfg::general::validators::mainBoard(const std::string &main_board,
                                         std::string &error) {
  bool res = macroInOptions(main_board_enum, main_board);
  if (!res) {
    error = "Main Board selection is invalid.";
    return false;
  }
  return true;
}

bool cfg::general::validators::startingProcedure(
    const std::string &starting_procedure, std::string &error) {
  bool res = macroInOptions(procedure_name_enum, starting_procedure);
  if (!res) {
    error = "Starting flight procedure selection is invalid.";
    return false;
  }
  return true;
}

// Data Module General

bool cfg::dm_general::validators::framePurgeTime(const int input,
                                                 std::string &error) {
  if (!(input >= 100) || !(input <= 600000)) {
    error = "Frame purge time is outside of the allowed range.";
    return false;
  }
  return true;
}

// Data Module Data Log

bool cfg::dm_data_log::validators::logStrategy(const std::string &input,
                                               std::string &error) {
  bool res = macroInOptions(log_strategy_enum, input);
  if (!res) {
    error = "log strategy selection is invalid.";
    return false;
  }
  return true;
}

bool cfg::dm_data_log::validators::logDetail(const std::string &input,
                                             std::string &error) {
  bool res = macroInOptions(log_detail_enum, input);
  if (!res) {
    error = "log detail selection is invalid.";
    return false;
  }
  return true;
}

bool cfg::dm_data_log::validators::logInterval(const int input,
                                               std::string &error) {
  if (!(input >= 100) || !(input <= 600000)) {
    error = "Log interval time is outside of the allowed range.";
    return false;
  }
  return true;
}

bool cfg::dm_data_log::validators::maxFileSize(const int input,
                                               std::string &error) {
  if (!(input >= 1) || !(input <= 500)) {
    error = "Max log file size is outside of the allowed range.";
    return false;
  }
  return true;
}

bool cfg::dm_data_log::validators::maxArchiveSize(const int input,
                                                  std::string &error) {
  if (!(input >= 50) || !(input <= 100000)) {
    error = "Max archive size is outside of the allowed range.";
    return false;
  }
  return true;
}

bool cfg::dm_data_log::validators::archiveMethod(const std::string &input,
                                                 std::string &error) {
  bool res = macroInOptions(archive_method_enum, input);
  if (!res) {
    error = "archive method selection is invalid.";
    return false;
  }
  return true;
}

// Data Module Influx DB
bool cfg::dm_influx_db::validators::url(const std::string &input,
                                        std::string &error) {
  if (!inRange(0, 50, input)) {
    error = "InfluxDB URL is outside of the allowed length range.";
    return false;
  }
  return true;
}

bool cfg::dm_influx_db::validators::token(const std::string &input,
                                          std::string &error) {
  if (!inRange(0, 50, input)) {
    error = "InfluxDB Token is outside of the allowed length range.";
    return false;
  }
  return true;
}

bool cfg::dm_influx_db::validators::org(const std::string &input,
                                        std::string &error) {
  if (!inRange(0, 50, input)) {
    error = "InfluxDB Organization u outside of the allowed length range.";
    return false;
  }
  return true;
}

bool cfg::dm_influx_db::validators::bucket(const std::string &input,
                                           std::string &error) {
  if (!inRange(0, 50, input)) {
    error = "InfluxDB Bucket u outside of the allowed length range.";
    return false;
  }
  return true;
}

bool cfg::dm_influx_db::validators::retentionPolicy(const std::string &input,
                                                    std::string &error) {
  bool res = macroInOptions(influx_retention_policy_enum, input);
  if (!res) {
    error = "influxdb retention policy selection is invalid.";
    return false;
  }
  return true;
}

// Data Module Debug

bool cfg::dm_debug::validators::debugLevel(const std::string &input,
                                           std::string &error) {
  bool res = macroInOptions(debug_levels_enum, input);
  if (!res) {
    error = "debug level selection is invalid.";
    return false;
  }
  return true;
}

// Console Module

bool cfg::console_module::validators::updateInterval(
    const int update_interval_ms, std::string &error) {
  if (!(update_interval_ms >= 100) || !(update_interval_ms <= 10000)) {
    error = "Console update interval is outside of the allowed range.";
    return false;
  } else if (update_interval_ms % 100 != 0) {
    error = "Console update interval is expected to be divisible by 100.";
    return false;
  }
  return true;
}

// Server Module

bool cfg::server_module::validators::tcpSocketPort(const int port_number,
                                                   std::string &error) {
  bool res = port_number >= 1024 && port_number <= 65535;
  if (!res) {
    error = "Port number " + std::to_string(port_number) +
            " is outside of the allowed range.";
  }
  return res;
}

// System Module
bool cfg::system_module::validators::systemInfoPollRate(const int poll_rate_ms,
                                                        std::string &error) {
  if (!(poll_rate_ms >= 100) || !(poll_rate_ms <= 600000)) {
    error = "System info poll rate is outside of the allowed range.";
    return false;
  } else if (poll_rate_ms % 100 != 0) {
    error = "System info poll rate is expected to be divisible by 100.";
    return false;
  }
  return true;
}

// Telemetry
bool cfg::telemetry::validators::callSign(const std::string &call_sign,
                                          std::string &error) {
  const std::string pattern = "[a-zA-Z0-9]{1,3}[0-9][a-zA-Z0-9]{0,3}[a-zA-Z]";
  constexpr unsigned int min = 3;
  constexpr unsigned int max = 6;

  if (!inRange(min, max, call_sign)) {
    error = "call_sign is out of acceptable length range.";
    return false;
  }

  if (!matchesPattern(pattern, call_sign)) {
    error = "call_sign does not match regex pattern.";
    return false;
  }

  return true;
}

bool cfg::telemetry::validators::frequency(const std::string &frequency,
                                           std::string &error) {
  const std::string pattern = "[0-9]{3}.[0-9]{4}";
  constexpr unsigned int min = 8;
  constexpr unsigned int max = 8;

  if (!inRange(min, max, frequency)) {
    error = "frequency string is out of acceptable length range.";
    return false;
  }

  if (!matchesPattern(pattern, frequency)) {
    error = "frequency string does not match regex pattern.";
    return false;
  }

  return true;
}

// Telemetry APRS
bool cfg::aprs::validators::ssid(const int ssid, std::string &error) {
  bool res = ssid >= 0 && ssid <= 15;
  if (!res) {
    error =
        "SSID " + std::to_string(ssid) + " is outside of the allowed range.";
  }
  return res;
};

bool cfg::aprs::validators::destinationAddress(const std::string &address,
                                               std::string &error) {
  const std::string pattern = ""; /** @todo pattern */
  constexpr unsigned int min = 3;
  constexpr unsigned int max = 6; /** @todo verify this value */

  if (!inRange(min, max, address)) {
    error = "frequency string is out of acceptable length range.";
    return false;
  }

  // if (!matchesPattern(pattern, address)) {
  //   error = "frequency string does not match regex pattern.";
  //   return false;
  // }

  return true;
}

bool cfg::aprs::validators::symbol(const std::string &symbol,
                                   std::string &error) {
  const std::string pattern = ""; /** @todo pattern */
  constexpr unsigned int min = 1;
  constexpr unsigned int max = 1;

  if (!inRange(min, max, symbol)) {
    error = "The APRS symbol must be a single character.";
    return false;
  }

  (void)pattern;
  // if (!matchesPattern(pattern, symbol)) {
  //   error = "frequency string does not match regex pattern.";
  //   return false;
  // }

  return true;
}

bool cfg::aprs::validators::symbolTable(const std::string &symbol_table,
                                        std::string &error) {
  bool res = macroInOptions(aprs_symbol_table, symbol_table);
  if (!res) {
    error = "APRS Symbol Table macro option is invalid.";
    return false;
  }

  return true;
}

bool cfg::aprs::validators::comment(const std::string &comment,
                                    std::string &error) {
  const std::string pattern = ""; /** @todo pattern */
  constexpr unsigned int min = 0;
  constexpr unsigned int max = 43; /** @todo verify this value */

  if (!inRange(min, max, comment)) {
    error = "APRS Comment must be from 0 to 43 characters long.";
    return false;
  }

  (void)pattern;
  // if (!matchesPattern(pattern, symbol)) {
  //   error = "frequency string does not match regex pattern.";
  //   return false;
  // }

  return true;
}

// Telemetry SSTV
bool cfg::sstv::validators::mode(const std::string &mode, std::string &error) {
  bool res = macroInOptions(sstv_modes_enum, mode);
  if (!res) {
    error = "SSTV Mode selection is invalid.";
    return false;
  }
  return true;
}

// Telemetry Data Packets
bool cfg::data_packets::validators::mode(const std::string &mode,
                                         std::string &error) {
  bool res = macroInOptions(data_packet_modes_enum, mode);
  if (!res) {
    error = "Data Packets Mode selection is invalid.";
    return false;
  }
  return true;
}

bool cfg::data_packets::validators::comment(const std::string &comment,
                                            std::string &error) {
  const std::string pattern = ""; /** @todo pattern */
  constexpr unsigned int min = 0;
  constexpr unsigned int max = 200; /** @todo verify this value */

  if (!inRange(min, max, comment)) {
    error = "Data Packets Comment must be from 0 to 200 characters long.";
    return false;
  }

  (void)pattern;
  // if (!matchesPattern(pattern, symbol)) {
  //   error = "frequency string does not match regex pattern.";
  //   return false;
  // }
  return true;
}