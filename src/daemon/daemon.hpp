/// =*========GIRAFFE========*=
/// A Unified Flight Command and Control System
/// https://github.com/joshua-jerred/Giraffe
/// https://giraffe.joshuajer.red/
/// =*=======================*=
///
/// @file   daemon.hpp
///
/// =*=======================*=
/// @author     Joshua Jerred (https://joshuajer.red)
/// @date       2024-11-23
/// @copyright  2024 (license to be defined)

#pragma once

#include <netinet/in.h>
#include <sys/socket.h>

#include <BoosterSeat/sleep.hpp>
#include <BoosterSeat/timer.hpp>

#include "agent_network.hpp"
#include "common_logger.hpp"
#include "giraffe_file_paths.hpp"
#include "socket.hpp"

#include "agent_data.hpp"
#include "agent_settings.hpp"
#include "external_socket_handler.hpp"
#include "ground_station_agent.hpp"

namespace command_line_interface {

class Daemon {
private:
  constexpr static int K_LOOP_DELAY_MS = 50;

public:
  Daemon() = default;
  ~Daemon() = default;

  /// @brief Check if a daemon is already running
  /// @return \c true if a daemon is running, \c false otherwise
  static bool doesDaemonExist() {
    sock::TcpSocketServer socket{};

    if (!socket.init(DAEMON_PORT)) {
      socket.close();
      return true;
    }

    if (!socket.close()) {
      std::cerr << "Failed to close socket on existence check!" << std::endl;
    }

    return false;
  }

  bool status(std::string &response) {
    protocol::Message response_message;
    if (!ExternalCommsHandler::sendExchangeToAgent(
            response_message, protocol::MessageType::REQ, "status", logger_)) {
      response = "not running";
      return false;
    }

    response = "running\n" + response_message.getJsonString();
    return true;
  }

  bool start(bool use_daemon = true) {
    if (doesDaemonExist()) {
      logger_.error("Agent is already running.");
    }

    if (use_daemon) {
      daemonize(); // Fork before binding to the port
    }

    if (!external_comms_.enable()) {
      logger_.error("Failed to enable external communications.");
      return false;
    }

    is_running_ = true;

    logger_.info("started agent");
    while (!agent_data_.isAgentStopRequested()) {
      mainLoop();
      bst::sleep(K_LOOP_DELAY_MS);
    }
    logger_.info("stopping agent");

    is_running_ = false;

    return true;
  }

  bool stop() {
    if (!doesDaemonExist()) {
      std::cerr << "Agent is not running." << std::endl;
      return false;
    }

    protocol::Message response;
    ExternalCommsHandler::sendExchangeToAgent(
        response, protocol::MessageType::SET, "stop", logger_);

    if (response.cde != protocol::ResponseCode::GOOD) {
      std::cerr << "Failed to request stop agent | " << response.getJsonString()
                << std::endl;
    }

    bool stopped = false;
    bst::Timer stop_timeout{2000};
    while (!stopped && !stop_timeout.isDone()) {
      if (!doesDaemonExist()) {
        stopped = true;
      }
      bst::sleep(100); // check every 100ms
    }

    if (!stopped) {
      std::cerr << "Daemon didn't stop." << std::endl;
      return false;
    }

    return true;
  }

  /// @brief Called by main to handle command line configuration requests. The
  /// agent must not be running.
  int commandLineConfigure(const std::string &setting_key,
                           const std::string &setting_value,
                           std::string &response) {
    if (doesDaemonExist()) {
      response = "Agent must be stopped to configure";
      return 1;
    }

    // Lock the port for the duration of the configuration
    {
      sock::TcpSocketServer socket{};
      if (!socket.init(DAEMON_PORT)) {
        response = "Failed to lock port for configuration";
        return 1;
      }

      if (!agent_settings_.setItem(setting_key, setting_value, response)) {
        response = "Failed to set setting";
        return 1;
      }
    }

    return 0;
  }

private:
  void mainLoop() {
    external_comms_.process();

    if (agent_settings_.isGroundStation()) {
      ground_station_agent_.process();
    } else {
      std::cout << "flight system agent not implemented" << std::endl;
    }
  }

  /// @brief Set the process to run as a daemon.
  /// @details https://stackoverflow.com/a/34587333
  void daemonize() {
    // Fork off the parent process
    pid_t pid = fork();

    // Check to see if we have a valid PID
    if (pid < 0) {
      std::cerr << "Failed to fork" << std::endl;
      exit(1);
    }

    // If we are the parent process, then we can exit
    if (pid > 0) {
      exit(EXIT_SUCCESS);
    }

    // Change the file mode mask
    umask(0);

    // Start logging here

    // Create a new session ID for the child process
    if (setsid() < 0) {
      std::cerr << "Failed to setsid" << std::endl;
      exit(1);
    }

    // Change the current working directory
    if (chdir("/") < 0) {
      std::cerr << "Failed to chdir" << std::endl;
      exit(1);
    }

    // Close out the standard file descriptors
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
  }

  bool is_running_ = false;

  giraffe::CommonLogger<100> logger_{
      giraffe::LoggerLevel::INFO, true,
      giraffe::file_paths::getFlightSystemAgentLogFilePath()};

  AgentData agent_data_{};
  AgentSettings agent_settings_{agent_data_, logger_};

  ExternalCommsHandler external_comms_{agent_data_, agent_settings_, logger_};

  GroundStationAgent ground_station_agent_{agent_data_};
};

} // namespace command_line_interface