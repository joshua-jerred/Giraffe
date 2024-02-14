/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   gfs_simulated_environment.hpp
 * @brief  A simulated environment for testing the flight system.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-02-13
 * @copyright  2024 (license to be defined)
 */

#ifndef GFS_SIMULATOR_HPP_
#define GFS_SIMULATOR_HPP_

#include <fmt/core.h>
#include <iostream>

#include <BoosterSeat/random.hpp>
#include <BoosterSeat/science.hpp>
#include <BoosterSeat/sleep.hpp>
#include <BoosterSeat/stopwatch.hpp>
#include <BoosterSeat/timer.hpp>

#include "sim_environmental.hpp"
#include "sim_parameters.hpp"
#include "sim_physics.hpp"

namespace gfs_sim {

class GfsSimulator {
public:
  GfsSimulator() {
  }
  ~GfsSimulator() {
  }

  void run() {
    flight_stopwatch_.start();
    while (true) {
      bst::sleep(K_SLEEP_INTERVAL_MS);
      elapsed_seconds_ = flight_stopwatch_.elapsed(bst::Resolution::SECONDS);
      if (state_machine_timer_.isDone()) {
        physics_.update(K_DELTA_TIME_S);
        stateMachine();
        state_machine_timer_.reset();
      }
      if (print_timer_.isDone()) {
        printData();
        print_timer_.reset();
      }
    }
  }

private:
  void statePreLaunch() {
    if (elapsed_seconds_ > K_PRELAUNCH_LAUNCH_DELAY_S) {
      physics_.launch();
    }
  }

  void stateAscent() {
  }

  void statePop() {
  }

  void stateDescent() {
  }

  void stateLanding() {
  }

  void stateLanded() {
  }

  void printData() {
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
        environment_.getTemperatureCelsius(),
        environment_.getPressureMillibars(),
        environment_.getRelativeHumidityPercent(),
        environment_.getWindSpeedMPS(), environment_.getWindDirectionDegrees());
  }

  SimState state_{SimState::PRE_LAUNCH};
  bst::Timer state_machine_timer_{K_UPDATE_INTERVAL_MS};
  bst::Timer print_timer_{K_PRINT_INTERVAL_MS};
  bst::Stopwatch flight_stopwatch_{};
  double elapsed_seconds_{0.0};

  SimEnvironmental environment_{};
  BalloonPhysics physics_{state_, environment_};

  void stateMachine() {
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
};

} // namespace gfs_sim

#endif /* GFS_SIMULATOR_HPP_ */