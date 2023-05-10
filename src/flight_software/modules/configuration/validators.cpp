/**
 * @file configuration.cpp
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief The implementation of the new (0.5) configuration class.
 * @date 2023-05-10
 * @copyright Copyright (c) 2023
 */

#include <regex>
#include <string>

#include "validators.h"

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

bool cfg::validator::general::projectName(const std::string &project_name, std::string &error) {
  const std::string pattern = "^(?!\\s)[a-zA-Z0-9_ -]{0,19}[^\\s]$";
  constexpr unsigned int min = 1;
  constexpr unsigned int max = 20;

  if (!inRange(min, max, project_name)) {
    error = "project_name is out of range";
    return false;
  }

  if (!matchesPattern(pattern, project_name)) {
    error = "project_name does not match pattern";
    return false;
  }

  return true;
}

bool cfg::validator::general::mainBoard(const std::string &main_board) {
  const std::array<std::string, 3> options = {"other", "pi_zero_w_2", "pi_4"};
  return macroInOptions(options, main_board);
}