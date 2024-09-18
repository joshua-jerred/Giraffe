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
  flight_phase_manager_.update();
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

void FlightRunner::fl_phaseChanged() {
  FlightPhase new_phase = shared_data_.flight_data.getFlightPhase();

  /// @todo remove this debug print
  std::cout << "Flight phase changed to: " << util::to_string(new_phase)
            << std::endl;

  // Save the new phase to non-volatile memory
  flight_runner_data_.setFlightPhase(new_phase);

  switch (new_phase) {
  case FlightPhase::UNKNOWN:
    shared_data_.status_led.setGreen(StatusLedState::State::OFF);
    break;
  case FlightPhase::PRE_LAUNCH:
    shared_data_.status_led.setGreen(StatusLedState::State::ON);
    break;
  case FlightPhase::LAUNCH: {
    shared_data_.status_led.setGreen(StatusLedState::State::BLINK);
#if RUN_IN_SIMULATOR == 1
    p_simulator_->launch();
#endif
  } break;
  case FlightPhase::ASCENT:
    shared_data_.status_led.setGreen(StatusLedState::State::OFF);
    break;
  case FlightPhase::DESCENT: {
    shared_data_.status_led.setGreen(StatusLedState::State::OFF);
    fl_reportDescent();
  } break;
  case FlightPhase::RECOVERY:
    shared_data_.status_led.setGreen(StatusLedState::State::BLINK);
    break;
  default:
    giraffe_assert(false);
    break;
  }
}

//// ------------------------------------------------------------------ ////