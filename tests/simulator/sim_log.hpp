/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   sim_log.hpp
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-09-17
 * @copyright  2024 (license to be defined)
 */

#pragma once

#include <iostream>

namespace gfs_sim {

/// @brief Log a message within the GFS simulator.
/// @param msg - The message to log.
inline void log(const std::string &msg) {
  // This is just a stub at the moment, log to a file in the future if needed.
  std::cout << "sim: " << msg << std::endl;
}

} // namespace gfs_sim