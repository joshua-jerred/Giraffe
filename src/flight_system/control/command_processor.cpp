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

#include <regex>

bool cmd::parseCommandString(const std::string &command_string,
                             Command &command) {
  std::regex command_regex("^cmd/[a-z]{3}/[a-z]{3}/[a-z0-9-]{0,20}$");
  if (!std::regex_match(command_string, command_regex)) {
    return false;
  }

  std::string destination_string = command_string.substr(4, 3);
  std::string command_id_string = command_string.substr(8, 3);

  return true;
}
