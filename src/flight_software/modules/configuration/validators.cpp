/**
 * @file configuration.cpp
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief The implementation of the new (0.5) configuration class.
 * @date 2023-05-10
 * @copyright Copyright (c) 2023
 */

#include "configuration_internal.h"

#include <regex>
#include <string>

// enumerations
static const std::array<std::string, 3> main_board_enum =
    {"other", "pi_zero_w_2", "pi_4"};
const std::array<std::string, 6> procedure_name_enum =
    {"testing", "pre_launch", "ascent", "descent", "recovery", "failsafe"};

// Checking Utilities

inline bool matchesPattern(const std::string &pattern, const std::string &input) {
  return std::regex_match(input, std::regex(pattern));
}

inline bool inRange(const unsigned int min, const unsigned int max, const std::string &input) {
  return input.length() >= min && input.length() <= max;
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

bool cfg::interface::validators::consoleUpdateInterval(const int update_interval_ms, std::string &error) {
  if (!(update_interval_ms >= 100) || !(update_interval_ms <= 10000)) {
    error = "Console update interval is outside of the allowed range.";
    return false;
  } else if (update_interval_ms % 100 != 0) {
    error = "Console update interval is expected to be divisible by 100.";
    return false;
  }
  return true;
}

bool cfg::interface::validators::webSocketPort(const int port_number, std::string &error) {
  bool res = port_number >= 1024 && port_number <= 65535;
  if (!res) {
    error = "Port number " + std::to_string(port_number) + " is outside of the allowed range.";
  }
  return res;
}