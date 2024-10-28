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
#include "sim_log.hpp"
#include "sim_parameters.hpp"
#include "sim_physics.hpp"

namespace gfs_sim {

class GfsSimulator {
public:
  GfsSimulator() = default;
  ~GfsSimulator() {
    stop();
  }

  void start();
  void stop();
  bool isRunning() const;

  void launch() {
    physics_.launch();
    log("launching");
  }

  double getTemperatureC() const {
    return sim_data_.temperature_c;
  }
  double getPressureMbar() const {
    return sim_data_.pressure_mbar;
  }
  double getHumidityPercent() const {
    return sim_data_.humidity_percent;
  }
  double getAltitudeM() const {
    return sim_data_.altitude_m;
  }
  double getVertVelocityMps() const {
    return sim_data_.vert_velocity_mps;
  }
  double getVertAccelMps2() const {
    return sim_data_.vert_accel_mps2;
  }
  double getHorzVelocityMps() const {
    return sim_data_.horz_velocity_mps;
  }
  double getHeadingDeg() const {
    return sim_data_.heading_deg;
  }
  double getLatitudeDeg() const {
    return sim_data_.latitude_deg;
  }
  double getLongitudeDeg() const {
    return sim_data_.longitude_deg;
  }
  double getBatteryVoltage() const {
    return sim_data_.battery_voltage;
  }

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

  SimData sim_data_{};

  SimEnvironmental environment_{sim_data_};
  BalloonPhysics physics_{state_, environment_, sim_data_};
  SimElectrical electrical_{sim_data_};

  std::atomic<bool> stop_flag_{false};
  std::thread sim_thread_{};
};

} // namespace gfs_sim

#if RUN_IN_SIMULATOR == 1
extern gfs_sim::GfsSimulator g_GFS_SIMULATOR;
#endif

#endif /* GFS_SIMULATOR_HPP_ */