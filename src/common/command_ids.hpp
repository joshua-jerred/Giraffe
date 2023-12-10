/**
 *
 * 
 * &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
 * AUTOMATICALLY GENERATED, DO NOT EDIT MANUALLY
 * &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
 *
 *
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 * 
 * 
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-06-30
 * @copyright  2023 (license to be defined)
 * =*=======================*=
 *
 * @verbatim
 */


#ifndef COMMAND_IDS_HPP_
#define COMMAND_IDS_HPP_

#include <unordered_map>
#include <string>
#include <cstdint>

namespace cmd {
// NOLINTBEGIN(readability-identifier-naming)
enum class CommandId : uint16_t {
  GENERAL_unknown=0x0000, // Unknown
  NODE_reloadConfiguration=0x0001, // Reload Configuration
  FLIGHT_RUNNER_shutdownSystem=0x1000, // Shutdown System
  FLIGHT_RUNNER_startModule=0x1001, // Start Module
  FLIGHT_RUNNER_stopModule=0x1002, // Stop Module
  FLIGHT_RUNNER_restartModule=0x1003, // Restart Module
  DATA_MODULE_clearAllErrors=0x2000, // Clear All Errors
  DATA_MODULE_clearSpecificError=0x2001, // Clear a Specific Error
  TELEMETRY_MODULE_sendNumActiveErrors=0x3000, // Send Number of Active Errors
  TELEMETRY_MODULE_sendRssi=0x3001, // Send RSSI
  TELEMETRY_MODULE_sendSnr=0x3002  // Send SNR
};
// NOLINTEND(readability-identifier-naming)

extern const std::unordered_map<std::string, CommandId> K_STRING_TO_COMMAND_MAP;
} // namespace cmd

#endif // COMMAND_IDS_HPP_


/**
 * @endverbatim
 * 
 * &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
 * AUTOMATICALLY GENERATED, DO NOT EDIT MANUALLY
 * &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
 */