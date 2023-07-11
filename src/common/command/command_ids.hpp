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

#include <cstdint>

namespace cmd {
// NOLINTBEGIN(readability-identifier-naming)
enum class CommandId : uint16_t {
  GENERAL_unknown=0x0000, // Unknown
  NODE_reloadConfiguration=0x0001, // Reload Configuration
  DATA_MODULE_clearAllErrors=0x1001  // Clear All Errors
};
// NOLINTEND(readability-identifier-naming)
} // namespace cmd

#endif // COMMAND_IDS_HPP_


/**
 * @endverbatim
 * 
 * &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
 * AUTOMATICALLY GENERATED, DO NOT EDIT MANUALLY
 * &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
 */