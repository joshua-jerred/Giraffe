#ifndef FLIGHT_RUNNER_H_
#define FLIGHT_RUNNER_H_

#include "configuration_module.h"
#include "data_module.h"
#include "streams.h"

class FlightRunner {
 public:
  FlightRunner();

  FlightRunner(const FlightRunner &) = delete;  // No copy constructor
  FlightRunner &operator=(const FlightRunner &) = delete;  // No copy assignment

  ~FlightRunner();

  int start();
  void shutdown();

 private:
  int flightLoop();

  bool shutdown_signal_ = false;
  data::Streams streams_ = data::Streams();

  modules::ConfigurationModule config_module_ =
      modules::ConfigurationModule(streams_);
  modules::DataModule data_module_ = modules::DataModule(streams_);

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