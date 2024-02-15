/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   command_processor.cpp
 * @brief  Used for parsing and processing commands.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-11-03
 * @copyright  2023 (license to be defined)
 */

#include "command.hpp"

#include <array>
#include <iostream>
#include <regex>
#include <unordered_map>

static const std::unordered_map<std::string, node::Identification>
    K_DESTINATION_MAP = {{"flr", node::Identification::FLIGHT_RUNNER},
                         {"cfg", node::Identification::CONFIGURATION},
                         {"dat", node::Identification::DATA_MODULE},
                         {"csl", node::Identification::CONSOLE_MODULE},
                         {"srv", node::Identification::SERVER_MODULE},
                         {"sys", node::Identification::SYSTEM_MODULE},
                         {"etm", node::Identification::EXTENSION_MODULE},
                         {"ext", node::Identification::EXTENSION},
                         {"tlm", node::Identification::TELEMETRY_MODULE}};

static const std::array<std::string, 6> K_MODULE_IDENTIFIERS = {
    "dat", "csl", "srv", "sys", "etm", "tlm"};

/**
 * @brief Returns the value of a hex string as a uint32_t.
 *
 * @param hex_string The hex string to convert.
 * @param hex_string_value (out) The value of the hex string.
 * @return true If the hex string was converted successfully.
 * @return false If the hex string was not converted successfully.
 */
bool hexStringToUInt32(const std::string &hex_string,
                       uint32_t &hex_string_value) {
  try {
    hex_string_value = std::stoul(hex_string, nullptr, 16);
  } catch (const std::exception &e) {
    (void)e;
    return false;
  }
  return true;
}

bool parseFlightRunnerCommand(const std::string &command_id_str,
                              const std::string &arg, cmd::Command &command) {
  static const std::unordered_map<std::string, cmd::CommandId>
      K_COMMAND_ID_MAP = {{"sdn", cmd::CommandId::FLIGHT_RUNNER_shutdownSystem},
                          {"mst", cmd::CommandId::FLIGHT_RUNNER_startModule},
                          {"msp", cmd::CommandId::FLIGHT_RUNNER_stopModule},
                          {"mrt", cmd::CommandId::FLIGHT_RUNNER_restartModule}};
  if (!K_COMMAND_ID_MAP.contains(command_id_str)) {
    return false;
  }
  command.command_id = K_COMMAND_ID_MAP.at(command_id_str);

  switch (command.command_id) {
  case cmd::CommandId::FLIGHT_RUNNER_shutdownSystem:
    /**
     * @todo need to implement some kind of rolling authentication system for
     * some of these commands.
     */
    if (arg.length() != 0) {
      return false;
    }
    return true;
  case cmd::CommandId::FLIGHT_RUNNER_startModule:
  case cmd::CommandId::FLIGHT_RUNNER_stopModule:
  case cmd::CommandId::FLIGHT_RUNNER_restartModule:
    if (arg.length() != 3) {
      return false;
    }
    for (const auto &module_id : K_MODULE_IDENTIFIERS) {
      if (arg == module_id) {
        command.int_arg = 0;
        command.str_arg = arg;
        return true;
      }
    }
    return false;
  default:
    return false;
  }
  return false;
}

bool parseConfigurationCommand(const std::string &command_id_str,
                               const std::string &arg, cmd::Command &command) {
  (void)command_id_str;
  (void)arg;
  (void)command;
  return false;
}

bool parseDataModuleCommand(const std::string &command_id_str,
                            const std::string &arg, cmd::Command &command) {
  static const std::unordered_map<std::string, cmd::CommandId>
      K_COMMAND_ID_MAP = {
          {"cae", cmd::CommandId::DATA_MODULE_clearAllErrors},
          {"cse", cmd::CommandId::DATA_MODULE_clearSpecificError}};
  if (!K_COMMAND_ID_MAP.contains(command_id_str)) {
    return false;
  }
  command.command_id = K_COMMAND_ID_MAP.at(command_id_str);

  switch (command.command_id) {
  case cmd::CommandId::DATA_MODULE_clearAllErrors:
    if (arg.length() != 0) {
      return false;
    }
    return true;

  case cmd::CommandId::DATA_MODULE_clearSpecificError:
    if (arg.length() != 4) {
      return false;
    }
    return hexStringToUInt32(arg, command.int_arg);

  default:
    return false;
  }
  return false;
}

bool parseServerModuleCommand(const std::string &command_id_str,
                              const std::string &arg, cmd::Command &command) {
  (void)command_id_str;
  (void)arg;
  (void)command;
  return false;
}

bool parseConsoleModuleCommand(const std::string &command_id_str,
                               const std::string &arg, cmd::Command &command) {
  (void)command_id_str;
  (void)arg;
  (void)command;
  return false;
}

bool parseSystemModuleCommand(const std::string &command_id_str,
                              const std::string &arg, cmd::Command &command) {
  (void)command_id_str;
  (void)arg;
  (void)command;
  return false;
}

bool parseExtensionModuleCommand(const std::string &command_id_str,
                                 const std::string &arg,
                                 cmd::Command &command) {
  (void)command_id_str;
  (void)arg;
  (void)command;
  return false;
}

bool parseExtensionCommand(const std::string &command_id_str,
                           const std::string &arg, cmd::Command &command) {
  (void)command_id_str;
  (void)arg;
  (void)command;
  return false;
}

bool parseTelemetryModuleCommand(const std::string &command_id_str,
                                 const std::string &arg,
                                 cmd::Command &command) {
  static const std::unordered_map<std::string, cmd::CommandId>
      K_COMMAND_ID_MAP = {
          {"nae", cmd::CommandId::TELEMETRY_MODULE_sendNumActiveErrors},
          {"rsi", cmd::CommandId::TELEMETRY_MODULE_sendRssi},
          {"snr", cmd::CommandId::TELEMETRY_MODULE_sendSnr}};
  if (!K_COMMAND_ID_MAP.contains(command_id_str)) {
    return false;
  }
  command.command_id = K_COMMAND_ID_MAP.at(command_id_str);

  switch (command.command_id) {
  case cmd::CommandId::TELEMETRY_MODULE_sendNumActiveErrors:
  case cmd::CommandId::TELEMETRY_MODULE_sendRssi:
  case cmd::CommandId::TELEMETRY_MODULE_sendSnr:
    if (arg.length() != 0) {
      return false;
    }
    return true;
  default:
    return false;
  };

  return false;
}

bool cmd::parseCommandString(const std::string &command_string,
                             Command &command) {
  const std::regex command_regex("^cmd/[a-z]{3}/[a-z]{3}/[a-z0-9-]{0,20}$");
  if (!std::regex_match(command_string, command_regex)) {
    return false;
  }

  std::string destination_string = command_string.substr(4, 3);
  if (!K_DESTINATION_MAP.contains(destination_string)) {
    return false;
  }
  command.destination = K_DESTINATION_MAP.at(destination_string);

  std::string command_id_string = command_string.substr(8, 3);
  std::string arg = "";
  if (command_string.length() >= 12) {
    arg = command_string.substr(12);
    std::cout << "ARG: " << arg << "FOR:  " << command_string << std::endl;
  }

  switch (command.destination) {
  case node::Identification::FLIGHT_RUNNER:
    return parseFlightRunnerCommand(command_id_string, arg, command);
  case node::Identification::CONFIGURATION:
    return parseConfigurationCommand(command_id_string, arg, command);
  case node::Identification::DATA_MODULE:
    return parseDataModuleCommand(command_id_string, arg, command);
  case node::Identification::CONSOLE_MODULE:
    return parseConsoleModuleCommand(command_id_string, arg, command);
  case node::Identification::SERVER_MODULE:
    return parseServerModuleCommand(command_id_string, arg, command);
  case node::Identification::SYSTEM_MODULE:
    return parseSystemModuleCommand(command_id_string, arg, command);
  case node::Identification::EXTENSION_MODULE:
    return parseExtensionModuleCommand(command_id_string, arg, command);
  case node::Identification::EXTENSION:
    return parseExtensionCommand(command_id_string, arg, command);
  case node::Identification::TELEMETRY_MODULE:
    return parseTelemetryModuleCommand(command_id_string, arg, command);
  default:
    return false;
  }

  return false;
}
