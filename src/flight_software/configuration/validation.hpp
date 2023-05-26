#ifndef VALIDATION_HPP_
#define VALIDATION_HPP_

#include <nlohmann/json.hpp>
#include <regex>
#include <string>
#include <unordered_map>

#include "streams.hpp"

using json = nlohmann::ordered_json;
namespace validation {

// template <typename T>
// void setValidEnum(data::LogStream &log, const json &json_data,
//                   const std::string &section, const std::string &key,
//                   T &value_to_set,
//                   const std::unordered_map<std::string, T> string_to_value);

// template <typename T>
// void setValidValue(data::LogStream &log, const json &json_data,
//                    const std::string &section, const std::string &key,
//                    T &value_to_set, float min = 0.0, float max = 0.0,
//                    std::string pattern = "");
// // error
// }  // namespace validation

using json = nlohmann::ordered_json;

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

template <typename T>
void setValidValue(data::LogStream &log, const json &json_data,
                               const std::string &section,
                               const std::string &key, T &value_to_set,
                               float min, float max, std::string pattern) {
  // error source
  constexpr node::Identification e_src = node::Identification::CONFIGURATION;

  // check for key existence
  if (!json_data.contains(key)) {
    log.error(e_src, data::logId::Config_settingNotFound, section + " " + key);
    return;
  }

  // Ensure that the value at this key is is correct json type
  bool valid_type = false;
  if constexpr (std::is_same<T, int>::value) {
    valid_type = json_data[key].is_number_integer();
  } else if constexpr (std::is_same<T, std::string>::value) {
    valid_type = json_data[key].is_string();
  } else if constexpr (std::is_same<T, bool>::value) {
    valid_type = json_data[key].is_boolean();
  } else if constexpr (std::is_same<T, float>::value) {
    valid_type = json_data[key].is_number_float();
  }

  if (!valid_type) {
    log.error(e_src, data::logId::Config_invalidJsonType, section + " " + key);
    return;
  }

  // read the value
  T value = json_data[key].get<T>();

  // validate
  bool valid_value = false;
  if constexpr (std::is_same<T, int>::value) {
    valid_value = validate(value, (int)min, (int)max);
  } else if constexpr (std::is_same<T, float>::value) {
    valid_value = validate(value, min, max);
  } else if constexpr (std::is_same<T, std::string>::value) {
    valid_value = validate(value, min, max, pattern);
  } else if constexpr (std::is_same<T, bool>::value) {
    // nothing to do
  }

  if (!valid_value) {
    log.error(e_src, data::logId::Config_invalidSettingValue,
              section + " " + key);
    return;
  }

  // finally, set the value
  value_to_set = value;
  return;
}

template <typename T>
void setValidEnum(
    data::LogStream &log, const json &json_data, const std::string &section,
    const std::string &key, T &value_to_set,
    const std::unordered_map<std::string, T> string_to_value) {
  constexpr node::Identification e_src = node::Identification::CONFIGURATION;

  // Check if the setting key is in the json data
  if (!json_data.contains(key)) {
    log.error(e_src, data::logId::Config_settingNotFound, section + " " + key);
    return;
  }

  // Ensure that the value at this key is actually a string before reading
  if (!json_data[key].is_string()) {
    log.error(e_src, data::logId::Config_enumIsNotString, section + " " + key);
    return;
  }

  // read the value
  std::string value = json_data[key].get<std::string>();

  // check if the key is in the map
  if (!string_to_value.contains(value)) {
    log.error(e_src, data::logId::Config_keyNotFoundInEnum,
              section + " " + key);
    return;
  }

  // finally, set the value with the string to enum value map
  value_to_set = string_to_value.at(value);
}

}  // namespace validation

#endif