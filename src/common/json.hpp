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

typedef nlohmann::ordered_json Json;

#endif /* JSON_HPP_ */