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

//// ---------------------- FLIGHT LOGIC PLUGINS ---------------------- ////

void FlightRunner::fl_preLaunchLogic() {
  // This is the pre-launch logic
}

void FlightRunner::fl_reportDescent() {
  cmd::Command command{};
  command.destination = node::Identification::TELEMETRY_MODULE;
  command.command_id = cmd::CommandId::INTERNAL_reportDescent;

  shared_data_.streams.command.addCommand(node::Identification::FLIGHT_RUNNER,
                                          command);
}

//// ------------------------------------------------------------------ ////

void FlightRunner::flightLogic() {
  flight_phase_detection_.update();
}

void FlightRunner::setFlightPhase(FlightPhase new_phase) {
  const auto old_phase = shared_data_.flight_data.getFlightPhase();
  if (old_phase == new_phase) {
    return;
  }

  // The transition from pre-launch to launch can only be triggered by the
  // user. If the transition is made, the launch position must be set.
  if (old_phase == FlightPhase::PRE_LAUNCH &&
      new_phase == FlightPhase::LAUNCH && !setLaunchPosition()) {
    // If the launch position is not set, we cannot transition to launch.
    // Error message is already logged.
    return;
  }

  switch (new_phase) {
  case FlightPhase::UNKNOWN:
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

  // Save to file
  flight_runner_data_.setFlightPhase(new_phase);

  shared_data_.streams.data.addData(
      node::Identification::FLIGHT_RUNNER,
      data::DataId::FLIGHT_RUNNER_flightPhaseChange,
      util::to_string(new_phase));

#if RUN_IN_SIMULATOR == 1
  if (new_phase == FlightPhase::LAUNCH) {
    p_simulator_->launch();
  }
#endif
}