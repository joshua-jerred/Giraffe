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

#include <BoosterSeat/sleep.hpp>
#include <BoosterSeat/stopwatch.hpp>
#include <BoosterSeat/timer.hpp>

#include "sim_environmental.hpp"
#include "sim_parameters.hpp"
#include "sim_physics.hpp"

namespace gfs_sim {

class GfsSimulator {
public:
  GfsSimulator() = default;
  ~GfsSimulator() = default;

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
  void stateMachine();
  void statePreLaunch();
  void stateAscent();
  void statePop();
  void stateDescent();
  void stateLanding();
  void stateLanded();

  void printData();

  SimState state_{SimState::PRE_LAUNCH};
  bst::Timer state_machine_timer_{K_UPDATE_INTERVAL_MS};
  bst::Timer print_timer_{K_PRINT_INTERVAL_MS};
  bst::Stopwatch flight_stopwatch_{};
  double elapsed_seconds_{0.0};

  SimEnvironmental environment_{};
  BalloonPhysics physics_{state_, environment_};
};

} // namespace gfs_sim

#endif /* GFS_SIMULATOR_HPP_ */