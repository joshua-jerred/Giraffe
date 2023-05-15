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
static const std::array<std::string, 3> main_board_enum =
    {"other", "pi_zero_w_2", "pi_4"};

static const std::array<std::string, 6> procedure_name_enum =
    {"testing", "pre_launch", "ascent", "descent", "recovery", "failsafe"};

static const std::array<std::string, 1> sstv_modes_enum =
    {"robot36"};

static const std::array<std::string, 8> data_packet_modes_enum =
    {"bpsk125",
     "bpsk250",
     "bpsk500",
     "bpsk1000",
     "qpsk125",
     "qpsk250",
     "qpsk500",
     "afsk_ax25"};

// Checking Utilities

inline bool matchesPattern(const std::string &pattern, const std::string &input) {
  return std::regex_match(input, std::regex(pattern));
}

inline bool inRange(const unsigned int min, const unsigned int max, const std::string &input) {
  return input.length() >= min && input.length() <= max;
}

inline bool inRange(const int min, const int max, const int input) {
  return input >= min && input <= max;
}

template <std::size_t SIZE>
inline bool macroInOptions(
    const std::array<std::string, SIZE> &options,
    const std::string value_to_compare) {
  if (std::find(std::begin(options), std::end(options), value_to_compare) != std::end(options)) {
    return true;
  }
  return false;
}

// General

bool cfg::general::validators::projectName(const std::string &project_name, std::string &error) {
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

bool cfg::general::validators::mainBoard(const std::string &main_board, std::string &error) {
  bool res = macroInOptions(main_board_enum, main_board);
  if (!res) {
    error = "Main Board selection is invalid.";
    return false;
  }
  return true;
}

bool cfg::general::validators::startingProcedure(const std::string &starting_procedure, std::string &error) {
  bool res = macroInOptions(procedure_name_enum, starting_procedure);
  if (!res) {
    error = "Starting flight procedure selection is invalid.";
    return false;
  }
  return true;
}

// Interface

bool cfg::debug::validators::consoleUpdateInterval(const int update_interval_ms, std::string &error) {
  if (!(update_interval_ms >= 100) || !(update_interval_ms <= 10000)) {
    error = "Console update interval is outside of the allowed range.";
    return false;
  } else if (update_interval_ms % 100 != 0) {
    error = "Console update interval is expected to be divisible by 100.";
    return false;
  }
  return true;
}

// Server

bool cfg::server::validators::tcpSocketPort(const int port_number, std::string &error) {
  bool res = port_number >= 1024 && port_number <= 65535;
  if (!res) {
    error = "Port number " + std::to_string(port_number) + " is outside of the allowed range.";
  }
  return res;
}

// Telemetry
bool cfg::telemetry::validators::callSign(
    const std::string &call_sign, std::string &error) {
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

bool cfg::telemetry::validators::frequency(
    const std::string &frequency, std::string &error) {
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
    error = "SSID " + std::to_string(ssid) + " is outside of the allowed range.";
  }
  return res;
};

bool cfg::aprs::validators::destinationAddress(const std::string &address, std::string &error) {
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

bool cfg::aprs::validators::symbol(const std::string &symbol, std::string &error) {
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

bool cfg::aprs::validators::comment(const std::string &comment, std::string &error) {
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
bool cfg::data_packets::validators::mode(const std::string &mode, std::string &error) {
  bool res = macroInOptions(data_packet_modes_enum, mode);
  if (!res) {
    error = "Data Packets Mode selection is invalid.";
    return false;
  }
  return true;
}

bool cfg::data_packets::validators::comment(const std::string &comment, std::string &error) {
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