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

#include <atomic>
#include <iostream>
#include <thread>

#include <BoosterSeat/sleep.hpp>
#include <BoosterSeat/stopwatch.hpp>
#include <BoosterSeat/timer.hpp>

#include "sim_electrical.hpp"
#include "sim_environmental.hpp"
#include "sim_parameters.hpp"
#include "sim_physics.hpp"

namespace gfs_sim {

class GfsSimulator {
public:
  GfsSimulator() = default;
  ~GfsSimulator() = default;

  void start();
  void stop();
  bool isRunning() const;

private:
  void run();
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
  SimElectrical electrical_{};

  std::atomic<bool> stop_flag_{false};
  std::thread sim_thread_{};
};

} // namespace gfs_sim

#endif /* GFS_SIMULATOR_HPP_ */