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
  if (flight_phase_manager_.update()) {
    fl_phaseChanged(flight_phase_manager_.getCurrentFlightPhase());
  }
}

//// ---------------------- FLIGHT LOGIC PLUGINS ---------------------- ////

void FlightRunner::fl_preLaunchLogic() {
}

void FlightRunner::fl_reportDescent() {
  cmd::Command command{};
  command.destination = node::Identification::TELEMETRY_MODULE;
  command.command_id = cmd::CommandId::INTERNAL_reportDescent;
  shared_data_.streams.command.addCommand(node::Identification::FLIGHT_RUNNER,
                                          command);
}

void FlightRunner::fl_phaseChanged(FlightPhase new_phase) {
  if (new_phase == FlightPhase::DESCENT) {
    fl_reportDescent();
  }

  switch (new_phase) {
  case FlightPhase::UNKNOWN:
    shared_data_.status_led.setGreen(StatusLedState::State::OFF);
    break;
  case FlightPhase::PRE_LAUNCH:
    shared_data_.status_led.setGreen(StatusLedState::State::ON);
    break;
  case FlightPhase::LAUNCH:
    shared_data_.status_led.setGreen(StatusLedState::State::BLINK);
    break;
  case FlightPhase::ASCENT:
    shared_data_.status_led.setGreen(StatusLedState::State::OFF);
    break;
  case FlightPhase::DESCENT:
    shared_data_.status_led.setGreen(StatusLedState::State::OFF);
    fl_reportDescent();
    break;
  case FlightPhase::RECOVERY:
    shared_data_.status_led.setGreen(StatusLedState::State::BLINK);
    break;
  default:
    giraffe_assert(false);
    break;
  }
}

//// ------------------------------------------------------------------ ////