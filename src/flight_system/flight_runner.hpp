#ifndef FLIGHT_RUNNER_HPP_
#define FLIGHT_RUNNER_HPP_

#include "configuration.hpp"
#include "console_module.h"
#include "data_module.hpp"
#include "extension_module.hpp"
#include "server_module.h"
#include "shared_data.hpp"
#include "system_module.hpp"

class FlightRunner {
public:
  FlightRunner() = default;

  FlightRunner(const FlightRunner &) = delete;            // No copy constructor
  FlightRunner &operator=(const FlightRunner &) = delete; // No copy assignment

  ~FlightRunner();

  auto start() -> int;
  auto shutdown() -> void;

private:
  auto flightLoop() -> int;

  bool shutdown_signal_ = false;
  data::SharedData shared_data_ = data::SharedData();

  cfg::Configuration *p_config_ = nullptr;

  modules::DataModule *p_data_module_ = nullptr;
  modules::ConsoleModule *p_console_module_ = nullptr;
  modules::ServerModule *p_server_module_ = nullptr;
  modules::SystemModule *p_system_module_ = nullptr;
  modules::ExtensionModule *p_extension_module_ = nullptr;

  //  void healthCheck();

  //  void switchLoops(FlightProcedure::Type procType);

  //  void deconstruct();

  // ConfigData config_data_ = ConfigData();
  //  FlightProcedure::Type current_flight_procedure_type_;
  //  FlightProcedure::Intervals current_intervals_ =
  //  FlightProcedure::Intervals();

  // DataStream data_stream_ = DataStream();

  //  modules::DataModule *p_data_module_ = nullptr;
  //  modules::ExtensionsModule *p_extension_module_ = nullptr;
  //  modules::ConsoleModule *p_console_module_ = nullptr;
  //  modules::ServerModule *p_server_module_ = nullptr;
  //  modules::TelemetryModule *p_telemetry_module_ = nullptr;
  // ComModule *mpComModule;

  //   FlightProcedure flt_proc_testing_ =
  //       FlightProcedure(false, FlightProcedure::Type::TESTING, {});
  //   FlightProcedure flt_proc_standard_ =
  //       FlightProcedure(false, FlightProcedure::Type::STANDARD, {});
  //   FlightProcedure flt_proc_descent_ =
  //       FlightProcedure(false, FlightProcedure::Type::DESCENT, {});
  //   FlightProcedure flt_proc_recovery_ =
  //       FlightProcedure(false, FlightProcedure::Type::RECOVERY, {});
  //   FlightProcedure flt_proc_failsafe_ =
  //       FlightProcedure(false, FlightProcedure::Type::FAILSAFE, {});
};

#endif