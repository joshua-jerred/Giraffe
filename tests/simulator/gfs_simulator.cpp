/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   gfs_simulator.cpp
 * @brief  The simulator for the flight system.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-02-13
 * @copyright  2024 (license to be defined)
 */

#include <fmt/core.h>

#include "gfs_simulator.hpp"

namespace gfs_sim {

void GfsSimulator::start() {
  stop_flag_ = false;
  sim_thread_ = std::thread(&GfsSimulator::run, this);
}

void GfsSimulator::stop() {
  stop_flag_ = true;

  if (sim_thread_.joinable()) {
    sim_thread_.join();
  }
}

bool GfsSimulator::isRunning() const {
  return !stop_flag_ || sim_thread_.joinable();
}

void GfsSimulator::run() {
  flight_stopwatch_.start();
  while (!stop_flag_) {
    bst::sleep(K_SLEEP_INTERVAL_MS);
    elapsed_seconds_ = flight_stopwatch_.elapsedSecondsF();
    if (state_machine_timer_.isDone()) {
      physics_.update(K_DELTA_TIME_S);
      electrical_.update(K_DELTA_TIME_S);
      stateMachine();
      state_machine_timer_.reset();
    }
    if (print_timer_.isDone() && K_PRINT_SIM_DATA) {
      printData();
      print_timer_.reset();
    }
  }
}

void GfsSimulator::statePreLaunch() {
  // if (elapsed_seconds_ > K_PRELAUNCH_LAUNCH_DELAY_S) {
  //   physics_.launch();
  // }
}

void GfsSimulator::stateAscent() {
}

void GfsSimulator::statePop() {
}

void GfsSimulator::stateDescent() {
}

void GfsSimulator::stateLanding() {
}

void GfsSimulator::stateLanded() {
}

void GfsSimulator::stateMachine() {
  switch (state_) {
  case SimState::PRE_LAUNCH:
    statePreLaunch();
    break;
  case SimState::ASCENT:
    stateAscent();
    break;
  case SimState::POP:
    statePop();
    break;
  case SimState::DESCENT:
    stateDescent();
    break;
  case SimState::LANDING:
    stateLanding();
    break;
  case SimState::LANDED:
    stateLanded();
    break;
  }
}

void GfsSimulator::printData() {
  std::string state_str;
  switch (state_) {
  case SimState::PRE_LAUNCH:
    state_str = "LNCH";
    break;
  case SimState::ASCENT:
    state_str = "ASNT";
    break;
  case SimState::POP:
    state_str = "PEAK";
    break;
  case SimState::DESCENT:
    state_str = "DCNT";
    break;
  case SimState::LANDING:
    state_str = "LDNG";
    break;
  case SimState::LANDED:
    state_str = "LDED";
    break;
  }

  fmt::print(
      "{} {:<5.0f} alt:{:<6.0f} vs:{:<5.1f} hs:{:<5.1f} hd:{:<3.0f} "
      " dst:{:<6.2f} "
      "[{:5.1f}c {:6.1f}mb {:4.1f}% {:4.1f}m/s @{:3.0f}]\n",
      state_str, elapsed_seconds_, physics_.getAltitude(),
      physics_.getVerticalSpeed(), physics_.getHorizontalSpeed(),
      physics_.getDirectionOfTravel(), (physics_.getTotalDistance() / 1000.0),
      environment_.getTemperatureCelsius(), environment_.getPressureMillibars(),
      environment_.getRelativeHumidityPercent(), environment_.getWindSpeedMPS(),
      environment_.getWindDirectionDegrees());
}

} // namespace gfs_sim
