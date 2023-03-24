/**
 * @file flight-runner.h
 * @author Joshua Jerred (https://joshuajer.red/)
 * @brief This file defines the FlightRunner class.
 * @version 0.1
 * @date 2022-09-20
 *
 * @todo Implement Flight Loop
 *
 * @copyright Copyright (c) 2022
 */
#ifndef _FLIGHT_RUNNER_H_
#define _FLIGHT_RUNNER_H_

#include "modules.h"

/**
 * @brief The FlightRunner class is responsible for pulling everything together.
 * It manages the timing of each action that is defined in the other files.
 * It is triggered with the start() method and will only return upon program
 * termination.
 *
 */
class FlightRunner {
 public:
  /**
   * @brief Construct a new Flight Runner object.
   */
  FlightRunner();

  FlightRunner(const FlightRunner &) = delete;  // No copy constructor
  FlightRunner &operator=(const FlightRunner &) = delete;  // No copy assignment

  /**
   * @brief Deconstructs the flight runner object, called at program
   * termination; ideally called after shutdown().
   * @see shutdown()
   */
  ~FlightRunner();

  /**
   * @brief This method starts and runs the flight computer.
   * @details Start will only return upon shutdown.
   *
   * @return int shutdown signal to main [0 = good shutdown, any other number
   * = bad shutdown]
   */
  int start();

  /**
   * @brief This is used by the signal handler to shutdown the flight computer.
   *
   * @return int shutdown signal to main [0 = good shutdown, any other
   * number = bad shutdown]
   */
  void shutdown();

 private:
  /**
   * @brief The flight loop is where the flight procedure is executed. Contains
   * an endless loop that will not stop until shutdown.
   *
   * @return int shutdown signal [0 = good shutdown, any other
   * number = bad shutdown]
   */
  int flightLoop();

  void healthCheck();

  void switchLoops(FlightProcedure::Type procType);

  void deconstruct();

  ConfigData config_data_ = ConfigData();
  FlightProcedure::Type current_flight_procedure_type_;
  FlightProcedure::Intervals current_intervals_ = FlightProcedure::Intervals();

  DataStream data_stream_ = DataStream();

  modules::DataModule *p_data_module_ = nullptr;
  modules::ExtensionsModule *p_extension_module_ = nullptr;
  modules::ConsoleModule *p_console_module_ = nullptr;
  modules::ServerModule *p_server_module_ = nullptr;
  modules::TelemetryModule *p_telemetry_module_ = nullptr;
  // ComModule *mpComModule;
  int shutdown_signal_;

  FlightProcedure flt_proc_testing_ =
      FlightProcedure(false, FlightProcedure::Type::TESTING, {});
  FlightProcedure flt_proc_standard_ =
      FlightProcedure(false, FlightProcedure::Type::STANDARD, {});
  FlightProcedure flt_proc_descent_ =
      FlightProcedure(false, FlightProcedure::Type::DESCENT, {});
  FlightProcedure flt_proc_recovery_ =
      FlightProcedure(false, FlightProcedure::Type::RECOVERY, {});
  FlightProcedure flt_proc_failsafe_ =
      FlightProcedure(false, FlightProcedure::Type::FAILSAFE, {});
};

#endif