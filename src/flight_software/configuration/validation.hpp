#ifndef VALIDATION_HPP_
#define VALIDATION_HPP_

#include "streams.hpp"
#include <nlohmann/json.hpp>
#include <unordered_map>
#include <string>

using json = nlohmann::ordered_json;

template <typename T>
void setValidEnum(
    data::LogStream &log, const json &json_data, const std::string &section,
    const std::string &key, T &value_to_set,
    const std::unordered_map<std::string, T> string_to_value);

template <typename T>
void setValidValue(data::LogStream &log, const json &json_data,
                   const std::string &section, const std::string &key,
                   T &value_to_set, float min, float max, std::string pattern);
  // error 


#endif