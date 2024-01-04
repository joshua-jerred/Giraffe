/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   flight_modes.hpp
 * @brief  Contains an enumeration of flight modes.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-01-03
 * @copyright  2024 (license to be defined)
 */

#ifndef FLIGHT_MODES_HPP_
#define FLIGHT_MODES_HPP_

namespace giraffe {

/**
 * @brief An enumeration of flight modes. Used by the flight runner to
 * determine what to do.
 */
enum class FlightMode {
  /**
   * @brief The flight mode at startup. If data can not be loaded, the system
   * will enter the pre-launch mode.
   */
  UNKNOWN,

  /**
   * @brief The system is in pre-launch mode. This is the initial state.
   */
  PRE_LAUNCH,

  /**
   * @brief The system is in launch mode. This is the state right before a
   * launch.
   */
  LAUNCH,

  /**
   * @brief The system is in ascent mode. This is the state during the entire
   * ascent phase.
   */
  ASCENT,

  /**
   * @brief The system is in descent mode. This is the state during the entire
   * descent phase.
   */
  DESCENT,

  /**
   * @brief The system is in recovery mode. The flight is over and the system is
   * in recovery mode.
   */
  RECOVERY
};

} // namespace giraffe

#endif /* FLIGHT_MODES_HPP_ */