/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   flight_runner.hpp
 * @brief  The main class for the flight system.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-02-15
 * @copyright  2024 (license to be defined)
 */

#ifndef FLIGHT_RUNNER_HPP_
#define FLIGHT_RUNNER_HPP_

#include "configuration.hpp"
#include "flight_phase_manager.hpp"
#include "flight_runner_data.hpp"

#include "console_module.hpp"
#include "data_module.hpp"
#include "extension_module.hpp"
#include "server_module.hpp"
#include "shared_data.hpp"
#include "system_module.hpp"
#include "telemetry_module.hpp"

#if RUN_IN_SIMULATOR == 1
#include "gfs_simulator.hpp"
#endif

using namespace giraffe;

/**
 * @brief The FlightRunner class is the main class for the flight system. It
 * ties everything together.
 */
class FlightRunner {
public:
#if RUN_IN_SIMULATOR == 1
  FlightRunner(gfs_sim::GfsSimulator *p_simulator) {
    p_simulator_ = p_simulator;
  }
#else
  FlightRunner() = default;
#endif

  FlightRunner(const FlightRunner &) = delete;            // No copy constructor
  FlightRunner &operator=(const FlightRunner &) = delete; // No copy assignment

  ~FlightRunner();

  /**
   * @brief Starts the flight runner.
   * @return int The exit code.
   */
  auto start() -> int;

  /**
   * @brief Shuts down the flight runner.
   */
  auto shutdown() -> void;

private:
  auto flightLoop() -> int;

  /**
   * @brief Process commands that are sent to the flight runner.
   * @param command The command to process.
   */
  void processCommand(const cmd::Command &command);

  /**
   * @brief Sends a command to the proper module.
   * @param command The command to route.
   */
  void routeCommand(cmd::Command &command);

  void toggleModule(const std::string &module_id, bool on_or_off);

  /**
   * @brief The main flight logic function. Called in the flight loop.
   */
  void flightLogic();

  /**
   * @brief Detects the current flight phase and sets it in the shared data.
   * @ref flight_phase.hpp
   */
  void detectFlightPhase();

  /**
   * @brief Called when transitioning from pre-launch to launch phase. Sets
   * the launch position in shared data and saves it to file.
   */
  bool setLaunchPosition();

  //// -- Flight Logic Methods -- ////
  void fl_phaseChanged(FlightPhase new_phase);
  void fl_preLaunchLogic();
  void fl_launchLogic();
  void fl_reportDescent();
  //// -------------------------- ////

  /**
   * @brief Non-volatile data for the flight runner.
   */
  FlightRunnerData flight_runner_data_ = FlightRunnerData();

  bool shutdown_signal_ = false;

  data::SharedData shared_data_ = data::SharedData();

  /// @todo Can this be put on the stack?
  cfg::Configuration *p_config_ = nullptr;

  modules::DataModule *p_data_module_ = nullptr;
  modules::ConsoleModule *p_console_module_ = nullptr;
  modules::ServerModule *p_server_module_ = nullptr;
  modules::SystemModule *p_system_module_ = nullptr;
  modules::ExtensionModule *p_extension_module_ = nullptr;
  modules::TelemetryModule *p_telemetry_module_ = nullptr;

#if RUN_IN_SIMULATOR == 1
  gfs_sim::GfsSimulator *p_simulator_ = nullptr;
#endif

  FlightPhaseManager flight_phase_manager_ = {
      shared_data_, flight_runner_data_.getFlightPhase(), 100, 10};
};

#endif