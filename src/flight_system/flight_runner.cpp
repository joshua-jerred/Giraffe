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

#include <BoosterSeat/sleep.hpp>

#include "giraffe_file_paths.hpp"

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

#if RUN_IN_SIMULATOR == 1
  std::cout << "!! -- Running in simulator mode -- !!" << std::endl;
#endif

  flight_runner_data_.incrementNumStartups();

  /*
    First, initialize the data streams to facilitate cross-thread communication.
    Then, load the configuration, if it exists, otherwise, create one.
    After that, startup the data module to start processing the data streams.
  */
  p_config_ = new cfg::Configuration(shared_data_.streams);
  p_config_->load(giraffe::file_paths::getGfsConfigFilePath());
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

  return flightLoop(); // This will only return on shutdown
}

/**
 * @brief Called only from main via a keyboard interrupt, this function sends a
 * shutdown signal to the flight runner.
 */
auto FlightRunner::shutdown() -> void {
  flight_runner_data_.setShutdownReason(
      FlightRunnerData::ShutdownReason::CTRL_C);
  shutdown_signal_ = true;
  return;
}

auto FlightRunner::flightLoop() -> int {
  constexpr uint32_t K_FLIGHT_LOOP_INTERVAL_MS = 250;
  std::cout << "Starting Flight Procedure" << std::endl;
  shared_data_.status_led.setGreen(StatusLedState::State::ON);

  // The endless loop where everything happens
  while (!shutdown_signal_) {
    // Run the flight logic
    flightLogic();

    // Check for new commands
    if (shared_data_.streams.command.getNumPackets() > 0) {
      data::CommandPacket command_packet{};
      bool new_command = shared_data_.streams.command.getPacket(command_packet);
      if (new_command) {
        routeCommand(command_packet.command);
      }
    }

    bst::sleep(K_FLIGHT_LOOP_INTERVAL_MS);
  }
  std::cout << std::endl
            << "Shutdown signal received." << std::endl
            << std::endl;
  p_config_->save(giraffe::file_paths::getGfsConfigFilePath());
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
  case node::Identification::TELEMETRY_MODULE:
    if (p_telemetry_module_ != nullptr) {
      command_added = p_telemetry_module_->addCommand(command);
    }
    break;
  case node::Identification::EXTENSION_MODULE:
    if (p_extension_module_ != nullptr) {
      command_added = p_extension_module_->addCommand(command);
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
  case cmd::CommandId::FLIGHT_RUNNER_startModule:
    toggleModule(command.str_arg, true);
    break;
  case cmd::CommandId::FLIGHT_RUNNER_stopModule:
    toggleModule(command.str_arg, false);
    break;
  case cmd::CommandId::FLIGHT_RUNNER_restartModule:
    toggleModule(command.str_arg, false);
    toggleModule(command.str_arg, true);
    break;
  case cmd::CommandId::FLIGHT_RUNNER_enterLaunchPhase:
    setFlightPhase(FlightPhase::LAUNCH);
    break;
  case cmd::CommandId::FLIGHT_RUNNER_enterPreLaunchPhase:
    setFlightPhase(FlightPhase::PRE_LAUNCH);
    break;
  default:
    shared_data_.streams.log.error(node::Identification::FLIGHT_RUNNER,
                                   DiagnosticId::FLIGHT_RUNNER_invalidCommand);
  }
}

void FlightRunner::toggleModule(const std::string &module_id, bool on_or_off) {
  if (module_id == "csl") {
    if (on_or_off) {
      if (p_console_module_ == nullptr) {
        p_console_module_ =
            new modules::ConsoleModule(shared_data_, *p_config_);
        p_console_module_->start();
      }
    } else {
      if (p_console_module_ != nullptr) {
        p_console_module_->stop();
        delete p_console_module_;
        p_console_module_ = nullptr;
      }
    }
  }
}

bool FlightRunner::setLaunchPosition() {
  constexpr int K_LAUNCH_POSITION_TIMEOUT_S = 20;
  const auto last_valid =
      shared_data_.blocks.location_data.get().last_valid_gps_frame;

  if (!last_valid.is_valid) {
    shared_data_.streams.log.error(
        node::Identification::FLIGHT_RUNNER,
        DiagnosticId::FLIGHT_RUNNER_launchPositionInvalid, "inv");
    flight_runner_data_.clearLaunchPosition();
    return false;
  }

  const int age_seconds = last_valid.getAgeSeconds();
  if (age_seconds) {
    shared_data_.streams.log.error(
        node::Identification::FLIGHT_RUNNER,
        DiagnosticId::FLIGHT_RUNNER_launchPositionInvalid,
        "to:" + std::to_string(age_seconds));
    flight_runner_data_.clearLaunchPosition();
    return false;
  }

  const double lat = last_valid.latitude;
  const double lon = last_valid.longitude;
  const double alt = last_valid.altitude;

  flight_runner_data_.setLaunchPosition(true, lat, lon, alt);
  shared_data_.flight_data.setLaunchPosition(lat, lon, alt);

  return true;
}