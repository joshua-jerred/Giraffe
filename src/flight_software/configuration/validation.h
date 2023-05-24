#ifndef VALIDATION_HPP_
#define VALIDATION_HPP_

#include <regex>
#include <string>

/**
 * @brief Validates that a string is both in range, and that it
 * matches a regex pattern (if provided)
 * @return true
 * @return false
 */
inline bool validate(const std::string &input, unsigned int min,
                     unsigned int max, const std::string &pattern = "") {
  bool in_range = input.length() >= min && input.length() <= max;
  if (pattern == "" || !in_range) {
    return in_range;
  }
  return std::regex_match(input, std::regex(pattern));
}

/**
 * @brief Validates that an integer matches the provided limitations
 * 
 */
inline bool validate(const int input, const int min, const int max) {
  return (input >= min) && (input <= max);
}

/**
 * @brief Validates that a float matches the provided limitations
 * 
 */
inline bool validate(const float input, const float min, const float max) {
  return (input >= min) && (input <= max);
}

template <std::size_t SIZE>
inline bool validate(const std::string &input, const std::array<std::string, SIZE> &options) {
  if (std::find(std::begin(options), std::end(options), input) !=
      std::end(options)) {
    return true;
  }
  return false;
}

#endif