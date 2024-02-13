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

#include <iostream>

#include <BoosterSeat/random.hpp>
#include <BoosterSeat/science.hpp>
#include <BoosterSeat/sleep.hpp>
#include <BoosterSeat/stopwatch.hpp>
#include <BoosterSeat/timer.hpp>

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
    switch (state_) {
    case SimState::PRE_LAUNCH:
      std::cout << "PRE_LAUNCH";
      break;
    case SimState::ASCENT:
      std::cout << "ASCENT";
      break;
    case SimState::POP:
      std::cout << "POP";
      break;
    case SimState::DESCENT:
      std::cout << "DESCENT";
      break;
    case SimState::LANDING:
      std::cout << "LANDING";
      break;
    case SimState::LANDED:
      std::cout << "LANDED";
      break;
    }
    std::cout << ", Time: " << elapsed_seconds_;
    std::cout << ", Alt: " << physics_.getAltitude() << "m";
    std::cout << ", VSpeed: " << physics_.getVerticalSpeed() << "m/s";
    std::cout << ", VAccel: " << physics_.getVerticalAcceleration() << "m/s^2";
    std::cout << std::endl;
  }

  SimState state_{SimState::PRE_LAUNCH};
  bst::Timer state_machine_timer_{K_UPDATE_INTERVAL_MS};
  bst::Timer print_timer_{K_PRINT_INTERVAL_MS};
  bst::Stopwatch flight_stopwatch_{};
  double elapsed_seconds_{0.0};

  BalloonPhysics physics_{state_};

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