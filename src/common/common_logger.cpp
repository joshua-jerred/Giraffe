/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   common_logger.cpp
 * @brief  A common logger for Giraffe
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-01-23
 * @copyright  2024 (license to be defined)
 */

#include "common_logger.hpp"

namespace giraffe {

std::string to_string(const LoggerLevel level) {
  switch (level) {
  case LoggerLevel::DEBUG:
    return "DEBUG";
  case LoggerLevel::INFO:
    return "INFO";
  case LoggerLevel::WARN:
    return "WARN";
  case LoggerLevel::ERROR:
    return "ERROR";
  }
  return "UNKNOWN";
}

}; // namespace giraffe