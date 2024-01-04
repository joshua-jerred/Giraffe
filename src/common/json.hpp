/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   json.hpp
 * @brief  Simple wrapper around nlohmann/json.hpp to provide a common
 *         interface.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-06-09
 * @copyright  2023 (license to be defined)
 */

#ifndef JSON_HPP_
#define JSON_HPP_

#include <nlohmann/json.hpp>
#include <string>

typedef nlohmann::ordered_json Json;

template <typename T>
inline T getJsonValue(const Json &json, std::string key, T default_value) {
  try {
    return json.at(key).get<T>();
  } catch (std::exception &e) {
    return default_value;
  }
}

#endif /* JSON_HPP_ */