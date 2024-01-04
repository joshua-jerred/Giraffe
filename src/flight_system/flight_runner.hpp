#ifndef FLIGHT_RUNNER_HPP_
#define FLIGHT_RUNNER_HPP_

#include "configuration.hpp"
#include "flight_modes.hpp"
#include "flight_runner_data.hpp"

#include "console_module.hpp"
#include "data_module.hpp"
#include "extension_module.hpp"
#include "server_module.hpp"
#include "shared_data.hpp"
#include "system_module.hpp"
#include "telemetry_module.hpp"

using namespace giraffe;

/**
 * @brief The FlightRunner class is the main class for the flight system. It
 * ties everything together.
 */
class FlightRunner {
public:
  FlightRunner() = default;

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
   * @brief Non-volatile data for the flight runner.
   */
  FlightRunnerData flight_runner_data_ = FlightRunnerData();

  bool shutdown_signal_ = false;

  data::SharedData shared_data_ = data::SharedData();

  cfg::Configuration *p_config_ = nullptr;

  modules::DataModule *p_data_module_ = nullptr;
  modules::ConsoleModule *p_console_module_ = nullptr;
  modules::ServerModule *p_server_module_ = nullptr;
  modules::SystemModule *p_system_module_ = nullptr;
  modules::ExtensionModule *p_extension_module_ = nullptr;
  modules::TelemetryModule *p_telemetry_module_ = nullptr;

  /**
   * @brief The current flight mode. This will be used to determine what to do.
   * @details This is loaded at startup from flight data.
   */
  FlightMode current_flight_mode_ = FlightMode::UNKNOWN;
};

#endif