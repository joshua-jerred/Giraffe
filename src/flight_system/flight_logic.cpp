/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   flight_logic.cpp
 * @brief  The implementation of the flight logic within the flight runner.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-02-15
 * @copyright  2024 (license to be defined)
 */

#include "flight_runner.hpp"
#include "to_string.hpp"

void FlightRunner::flightLogic() {
  detectFlightPhase();
}

void FlightRunner::detectFlightPhase() {
  bool gps_valid = false;
  bool ascending = false;
  bool descending = false;

  switch (shared_data_.flight_data.getFlightPhase()) {

    // -- CURRENTLY UNKNOWN --
  case FlightPhase::UNKNOWN:
    // If the flight phase is unknown, (start-up only) we need check if we were
    // previously in a flight phase.
    switch (flight_runner_data_.getFlightPhase()) {
    case FlightPhase::UNKNOWN:
      // Previously unknown/never set.
      setFlightPhase(FlightPhase::LAUNCH);
      break;
    case FlightPhase::PRE_LAUNCH:
      setFlightPhase(FlightPhase::PRE_LAUNCH);
      break;
    case FlightPhase::LAUNCH:
      setFlightPhase(FlightPhase::LAUNCH);
      break;
    case FlightPhase::ASCENT:
      setFlightPhase(FlightPhase::ASCENT);
      break;
    case FlightPhase::DESCENT:
      setFlightPhase(FlightPhase::DESCENT);
      break;
    case FlightPhase::RECOVERY:
      setFlightPhase(FlightPhase::RECOVERY);
      break;
    default:
      giraffe_assert(false);
      break;
    }
    shared_data_.streams.log.info(
        node::Identification::FLIGHT_RUNNER,
        "startup flight phase set to: " +
            util::to_string(shared_data_.flight_data.getFlightPhase()));
    break;

    // -- CURRENTLY IN PRE-LAUNCH PHASE --
  case FlightPhase::PRE_LAUNCH:
    break;

    // -- CURRENTLY IN LAUNCH PHASE --
  case FlightPhase::LAUNCH:
    break;

    // -- CURRENTLY IN ASCENT PHASE --
  case FlightPhase::ASCENT:
    break;

    // -- CURRENTLY IN DESCENT PHASE --
  case FlightPhase::DESCENT:
    break;

    // -- CURRENTLY IN RECOVERY PHASE --
  case FlightPhase::RECOVERY:
    break;
  default:
    giraffe_assert(false);
    break;
  }
}

void FlightRunner::setFlightPhase(FlightPhase phase) {
  shared_data_.flight_data.flight_phase = phase;
  flight_runner_data_.setFlightPhase(phase);

  shared_data_.streams.data.addData(
      node::Identification::FLIGHT_RUNNER,
      data::DataId::FLIGHT_RUNNER_flightPhaseChange, util::to_string(phase));
}