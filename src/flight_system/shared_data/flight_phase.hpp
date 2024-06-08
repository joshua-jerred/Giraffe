/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   flight_phase.hpp
 * @brief  Contains the flight phase enum and related functions.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-02-15
 * @copyright  2024 (license to be defined)
 */

#ifndef FLIGHT_PHASE_HPP_
#define FLIGHT_PHASE_HPP_

#include <string>

/**
 * @brief Used to track the current phase of the flight.
 */
enum class FlightPhase : int {
  UNKNOWN = 0,
  PRE_LAUNCH = 1,
  LAUNCH = 2,
  ASCENT = 3,
  DESCENT = 4,
  RECOVERY = 5
};

namespace util {

/**
 * @brief Converts a flight phase to a string.
 *
 * @param phase The flight phase to convert.
 * @return std::string The string representation of the flight phase.
 */
std::string to_string(FlightPhase phase);

} // namespace util

#endif /* FLIGHT_PHASE_HPP_ */