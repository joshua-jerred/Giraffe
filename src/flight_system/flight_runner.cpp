/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   flight_runner.cpp
 * @brief  The implementation of the flight runner, where everything comes
 * together.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-11-08
 * @copyright  2023 (license to be defined)
 */

#include "flight_runner.hpp"

/**
 * @brief The release version of Giraffe, set by CMake.
 */
const std::string K_GIRAFFE_VERSION_NUMBER = GIRAFFE_VERSION_NUMBER;

/**
 * @brief The release stage of Giraffe, set by CMake.
 */
const std::string K_GIRAFFE_VERSION_STAGE = GIRAFFE_VERSION_STAGE;

/**
 * @brief The path to the configuration file.
 */
static const std::string K_CONFIG_FILE_PATH = "./config.json";

/**
 * @brief These are helper functions to print out startup and shutdown messages
 *
 * @defgroup FlightRunnerConsoleHelpers Flight Runner Console Helpers
 * @{
 */
inline void _start(const std::string &module_name) {
  std::cout << module_name << " module starting ... ";
}

inline void _done() {
  std::cout << "done." << std::endl;
}

inline void _disabled(const std::string &module_name) {
  std::cout << module_name << " module disabled." << std::endl;
}

inline void _stop(const std::string &module_name) {
  std::cout << module_name << " module stopping ... ";
}
/** @} */ // end of FlightRunnerConsoleHelpers

FlightRunner::~FlightRunner() {
  // Shutdown the console module first as it will hijack the terminal.
  if (p_console_module_ != nullptr) {
    _stop("console");
    p_console_module_->stop();
    delete p_console_module_;
    _done();
  } else {
    _disabled("console");
  }

  if (p_telemetry_module_ != nullptr) {
    _stop("telemetry");
    p_telemetry_module_->stop();
    delete p_telemetry_module_;
    _done();
  } else {
    _disabled("telemetry");
  }

  _stop("extension");
  std::cout << std::endl;
  p_extension_module_->stop();
  delete p_extension_module_;
  _done();

  _stop("system");
  p_system_module_->stop();
  delete p_system_module_;
  _done();

  _stop("server");
  p_server_module_->stop();
  delete p_server_module_;
  _done();

  _stop("data");
  p_data_module_->stop();
  delete p_data_module_;
  delete p_config_;
  _done();

  std::cout << "Flight Runner stopped." << std::endl;
}

auto FlightRunner::start() -> int {
  std::cout << "Giraffe Flight Software v" << K_GIRAFFE_VERSION_NUMBER
            << std::endl;

  /*
    First, initialize the data streams to facilitate cross-thread communication.
    Then, load the configuration, if it exists, otherwise, create one.
    After that, startup the data module to start processing the data streams.
  */
  p_config_ = new cfg::Configuration(shared_data_.streams);
  p_config_->load(K_CONFIG_FILE_PATH);
  p_data_module_ = new modules::DataModule(shared_data_, *p_config_);
  p_data_module_->start();

  /*
  Start the console module if it's enabled.
  */
  if (p_config_->console_module.getEnabled()) {
    p_console_module_ = new modules::ConsoleModule(shared_data_, *p_config_);
    p_console_module_->start();
  } else {
    shared_data_.streams.data.reportStatus(node::Identification::CONSOLE_MODULE,
                                           node::Status::DISABLED);
  }

  /*
    Start the server, system, and telemetry modules.
  */
  p_server_module_ = new modules::ServerModule(shared_data_, *p_config_);
  p_server_module_->start();
  p_system_module_ = new modules::SystemModule(shared_data_, *p_config_);
  p_system_module_->start();

  if (p_config_->telemetry.getTelemetryEnabled()) {
    p_telemetry_module_ =
        new modules::TelemetryModule(shared_data_, *p_config_);
    p_telemetry_module_->start();
  } else {
    shared_data_.streams.data.reportStatus(
        node::Identification::TELEMETRY_MODULE, node::Status::DISABLED);
  }

  // ~~~ Initialize BCM/GPIO Interface ~~~ //
  // if (config_data_.bcm_interface_used) {
  //   std::cout << "Initializing BCM Interface... ";
  //   try {
  //     interface::Gpio::Initialize();
  //     std::cout << "Success" << std::endl;
  //   } catch (interface::Gpio::GpioException& e) {
  //     std::cout << "Failed: " << e.what() << std::endl;
  //   }
  // }

  /*
    Start the extension module (along with its extensions)
  */
  p_extension_module_ = new modules::ExtensionModule(shared_data_, *p_config_);
  p_extension_module_->start();

  // ~~~ Setup Done, Start the Flight Loop ~~~ //
  // if (config_data_.general.starting_proc ==
  //     FlightProcedure::Type::TESTING) {  // If user specified in config to
  //     use
  //                                        // the testing proc, it's selected
  //                                        // here.
  //   switchLoops(FlightProcedure::Type::TESTING);
  //   std::cout << "Starting in Testing Loop" << std::endl;
  // } else {
  //   healthCheck();  // Perform a health check to determine the flight proc
  //   type
  // }
  return flightLoop(); // This will only return on shutdown
}

/**
 * @brief Called from main via a keyboard interrupt, this function sends a
 * shutdown signal to the flight runner.
 */
auto FlightRunner::shutdown() -> void {
  shutdown_signal_ = true;
  return;
}

auto FlightRunner::flightLoop() -> int {

  std::cout << "Starting Flight Procedure" << std::endl;
  // Timer tsl_data_log;  // Refer to timer.h
  // Timer tsl_server;
  // Timer tsl_data_packet;  // tsl = time since last
  // Timer tsl_photo;
  // Timer tsl_APRS;
  // Timer tsl_SSTV_image;
  // Timer tsl_health_check;

  // GFSCommand command;
  static int count = 0;
  while (!shutdown_signal_) { // The endless loop where everything happens
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    if (shared_data_.streams.command.getNumPackets() > 0) {
      data::CommandPacket command_packet{};
      bool new_command = shared_data_.streams.command.getPacket(command_packet);
      if (new_command) {
        routeCommand(command_packet.command);
      }
    }

    // if (data_stream_.getNextCommand(command)) {  // Check for commands
    //   switch (command.category) {
    //     case GFSCommand::CommandCategory::FLR:
    //       /** @todo implement flight runner commands*/
    //       break;
    //     case GFSCommand::CommandCategory::TLM:  // Telemetry Module
    //       if (p_telemetry_module_ != nullptr) {
    //         p_telemetry_module_->addCommand(command);
    //       } else {
    //         // call to error
    //       }
    //       break;
    //     case GFSCommand::CommandCategory::MDL:
    //       /** @todo implement module commands */
    //       break;
    //     case GFSCommand::CommandCategory::EXT:
    //       if (p_extension_module_ != nullptr) {
    //         p_extension_module_->addCommand(command);
    //       } else {
    //         // call to error
    //       }
    //       break;
    //     case GFSCommand::CommandCategory::DAT:
    //       if (p_data_module_ != nullptr) {
    //         p_data_module_->addCommand(command);
    //       } else {
    //         // call to error
    //       }
    //       break;
    //     default:
    //       std::cout << "unknown command category" << std::endl;
    //       break;
    //   }
    // }

    // if (tsl_data_log.elapsed() > current_intervals_.data_log) {
    //   p_data_module_->log();
    //   tsl_data_log.reset();
    // }

    // if (config_data_.telemetry.telemetry_enabled) {
    //   if (config_data_.telemetry.data_packets_enabled &&
    //       tsl_data_packet.elapsed() > current_intervals_.data_packet) {
    //     p_telemetry_module_->sendDataPacket();
    //     tsl_data_packet.reset();
    //   }
    //   if (config_data_.telemetry.aprs_enabled &&
    //       tsl_APRS.elapsed() > current_intervals_.aprs) {
    //     p_telemetry_module_->sendAPRS();
    //     tsl_APRS.reset();
    //   }
    //   if (config_data_.telemetry.sstv_enabled &&
    //       tsl_SSTV_image.elapsed() > current_intervals_.sstv) {
    //     p_telemetry_module_->sendSSTVImage();
    //     tsl_SSTV_image.reset();
    //   }
    // }

    // if (config_data_.debug.web_server_enabled &&
    //     tsl_server.elapsed() > MODULE_SERVER_CHECK_COMMANDS_INTERVAL) {
    //   if (p_server_module_->checkShutdown()) {
    //     shutdown();
    //   }
    // }
    // if (tslPhoto.elapsed() > current_intervals_.picture) {
    //     p_data_module_->capturePhoto();
    // }
    // if (tslHealthCheck.elapsed() > current_intervals_.healthCheck) {
    //     healthCheck();
    // }
  }
  std::cout << std::endl
            << "Shutdown signal received." << std::endl
            << std::endl;
  p_config_->save(K_CONFIG_FILE_PATH);
  return 0;
}

void FlightRunner::routeCommand(cmd::Command &command) {
  bool command_added = false;
  switch (command.destination) {
  case node::Identification::FLIGHT_RUNNER:
    processCommand(command);
    command_added = true;
    break;
  case node::Identification::DATA_MODULE:
    command_added = p_data_module_->addCommand(command);
    break;
  case node::Identification::CONSOLE_MODULE:
    if (p_console_module_ != nullptr) {
      command_added = p_console_module_->addCommand(command);
    }
    break;
  case node::Identification::SERVER_MODULE:
    if (p_server_module_ != nullptr) {
      command_added = p_server_module_->addCommand(command);
    }
    break;
  default:
    shared_data_.streams.log.error(
        node::Identification::FLIGHT_RUNNER,
        DiagnosticId::FLIGHT_RUNNER_commandDestinationNotFound,
        node::K_IDENTIFICATION_TO_STRING_MAP.at(command.destination));
    return;
  }

  if (command_added == false) {
    shared_data_.streams.log.error(
        node::Identification::FLIGHT_RUNNER,
        DiagnosticId::FLIGHT_RUNNER_failedToRouteCommand);
  }
}

void FlightRunner::processCommand(const cmd::Command &command) {
  switch (command.command_id) {
  case cmd::CommandId::FLIGHT_RUNNER_shutdownSystem:
    shutdown_signal_ = true; // This will cause the flight loop to exit
    break;
  default:
    shared_data_.streams.log.error(node::Identification::FLIGHT_RUNNER,
                                   DiagnosticId::FLIGHT_RUNNER_invalidCommand);
  }
}

/**
 * @brief This function will switch the flight proc to a different proc type
 * if the values of extensions are within certain ranges.
 * @details This function is currently not implemented but it is responsible
 * for switching the flight procs if it's needed. This can be because flight
 * critical components are not working or to switch from standard to recovery.
 * @param None
 * @return void
 */
// void FlightRunner::healthCheck() {
// }

// void FlightRunner::switchLoops(FlightProcedure::Type procType) {
//   ConfigData::Procs procs = config_data_.flight_procs;
//   FlightProcedure current_procedure;
//   switch (procType) {
//     case FlightProcedure::Type::TESTING:

//       current_intervals_ = procs.testing.intervals;  // Set the intervals

//       // Send the procedure to the data stream
//       current_procedure.type = FlightProcedure::Type::TESTING;
//       current_procedure.intervals = procs.testing.intervals;
//       data_stream_.updateFlightProcedure(current_procedure);

//       break;
//     default:  // Default back to failsafe flight proc

//       current_intervals_ = procs.failsafe.intervals;  // Set the intervals

//       // Send the procedure to the data stream
//       current_procedure.type = FlightProcedure::Type::FAILSAFE;
//       current_procedure.intervals = procs.failsafe.intervals;
//       data_stream_.updateFlightProcedure(current_procedure);
//       break;
//   }
// }