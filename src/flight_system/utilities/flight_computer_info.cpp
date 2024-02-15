/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   flight_computer_info.cpp
 * @brief  Simple utility to get flight computer information.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-02-13
 * @copyright  2024 (license to be defined)
 */

#include "flight_computer_info.hpp"

#include <BoosterSeat/process.hpp>
#include <BoosterSeat/string_utils.hpp>

namespace giraffe {

FCInfo getFlightComputerInfo() {
  FCInfo info;

  // -- detect architecture
  try {
    bst::Process process("uname", {"-m"});
    process.waitToComplete();
    const std::string &output = process.getStdout();
    if (process.getExitCode() != 0 || output.empty()) {
      throw std::exception();
    }

    if (bst::containsSubstring(output, "arm")) {
      info.architecture = FCInfo::Architecture::ARM;
    } else if (bst::containsSubstring(output, "x86")) {
      info.architecture = FCInfo::Architecture::X86;
    } else if (bst::containsSubstring(output, "x86_64")) {
      info.architecture = FCInfo::Architecture::X86_64;
    } else {
      info.architecture = FCInfo::Architecture::UNKNOWN;
    }

  } catch (const std::exception &e) {
    info.architecture = FCInfo::Architecture::UNKNOWN;
  }

  // -- detect type
  // ARM, assume it's a Raspberry Pi (this can be expanded in the future)
  /// @todo Add support for PI 3, 4, etc.
  if (info.architecture == FCInfo::Architecture::ARM) {
    try {
      bst::Process process("cat", {"/proc/device-tree/model"});
      process.waitToComplete();
      const std::string &output = process.getStdout();
      if (process.getExitCode() != 0 || output.empty()) {
        throw std::exception();
      }

      if (bst::containsSubstring(output, "Pi Zero 2")) {
        info.type = FCInfo::Type::PI_ZERO_2;
      } else if (bst::containsSubstring(output, "Pi Zero")) {
        info.type = FCInfo::Type::PI_ZERO;
      } else {
        info.type = FCInfo::Type::OTHER;
      }

    } catch (const std::exception &e) {
      info.type = FCInfo::Type::UNKNOWN;
    }
  } else if (info.architecture == FCInfo::Architecture::X86 ||
             info.architecture == FCInfo::Architecture::X86_64) {
    info.type = FCInfo::Type::OTHER;
  } else {
    info.type = FCInfo::Type::UNKNOWN;
  }

  return info;
}

} // namespace giraffe