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


#include "command_ids.hpp"

namespace cmd {

const std::unordered_map<std::string, CommandId> K_STRING_TO_COMMAND_MAP {
  {"GENERAL_unknown",CommandId::GENERAL_unknown},
  {"INTERNAL_reportDescent",CommandId::INTERNAL_reportDescent},
  {"INTERNAL_reportPhaseChange",CommandId::INTERNAL_reportPhaseChange},
  {"FLIGHT_RUNNER_shutdownSystem",CommandId::FLIGHT_RUNNER_shutdownSystem},
  {"FLIGHT_RUNNER_startModule",CommandId::FLIGHT_RUNNER_startModule},
  {"FLIGHT_RUNNER_stopModule",CommandId::FLIGHT_RUNNER_stopModule},
  {"FLIGHT_RUNNER_restartModule",CommandId::FLIGHT_RUNNER_restartModule},
  {"FLIGHT_RUNNER_enterLaunchPhase",CommandId::FLIGHT_RUNNER_enterLaunchPhase},
  {"FLIGHT_RUNNER_enterPreLaunchPhase",CommandId::FLIGHT_RUNNER_enterPreLaunchPhase},
  {"FLIGHT_RUNNER_startBitTest",CommandId::FLIGHT_RUNNER_startBitTest},
  {"FLIGHT_RUNNER_stopBitTest",CommandId::FLIGHT_RUNNER_stopBitTest},
  {"FLIGHT_RUNNER_resetBitTest",CommandId::FLIGHT_RUNNER_resetBitTest},
  {"DATA_MODULE_clearAllErrors",CommandId::DATA_MODULE_clearAllErrors},
  {"DATA_MODULE_clearSpecificError",CommandId::DATA_MODULE_clearSpecificError},
  {"TELEMETRY_MODULE_sendNumActiveErrors",CommandId::TELEMETRY_MODULE_sendNumActiveErrors},
  {"TELEMETRY_MODULE_sendRssi",CommandId::TELEMETRY_MODULE_sendRssi},
  {"TELEMETRY_MODULE_sendSnr",CommandId::TELEMETRY_MODULE_sendSnr},
  {"TELEMETRY_MODULE_sendAprsLocation",CommandId::TELEMETRY_MODULE_sendAprsLocation},
  {"TELEMETRY_MODULE_sendSstvImage",CommandId::TELEMETRY_MODULE_sendSstvImage},
  {"TELEMETRY_MODULE_sendOnAirDefinitionOfTelem",CommandId::TELEMETRY_MODULE_sendOnAirDefinitionOfTelem},
  {"EXTENSION_MODULE_addPreConfiguredExtension",CommandId::EXTENSION_MODULE_addPreConfiguredExtension},
  {"EXTENSION_MODULE_disableExtension",CommandId::EXTENSION_MODULE_disableExtension},
  {"EXTENSION_MODULE_enableExtension",CommandId::EXTENSION_MODULE_enableExtension},
  {"EXTENSION_MODULE_restartExtension",CommandId::EXTENSION_MODULE_restartExtension}
};

} // namespace cmd


/**
 * @endverbatim
 *
 * &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
 * AUTOMATICALLY GENERATED, DO NOT EDIT MANUALLY
 * &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
 */