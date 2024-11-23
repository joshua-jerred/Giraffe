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

#include "agent_network.hpp"
#include "common_logger.hpp"
#include "giraffe_file_paths.hpp"
#include "socket.hpp"

namespace flight_system_agent {

class Daemon {
private:
  constexpr static int K_LOOP_DELAY_MS = 100;
  const std::string K_ADDRESS = "127.0.0.1";

public:
  Daemon() = default;
  ~Daemon() = default;

  /// @brief Check if a daemon is already running
  /// @return \c true if a daemon is running, \c false otherwise
  static bool doesDaemonExist() {
    sock::TcpSocketServer socket{};

    if (!socket.init(DAEMON_PORT)) {
      return true;
    }

    if (!socket.close()) {
      std::cerr << "Failed to close socket on existence check!" << std::endl;
    }

    return false;
  }

  bool status(std::string &response) {
    sock::TcpSocketClient client(logger_);
    if (client.connect(K_ADDRESS, DAEMON_PORT)) {
      if (!client.transaction("status", response)) {
        response = "failed transaction - response: " + response;
        return false;
      }
      return response == "running";
    }

    response = "not running";
    return false;
  }

  bool start() {
    if (is_running_) {
      return false;
    }

    if (!socket_.init(DAEMON_PORT)) {
      return false;
    }

    // As soon as we bind the port, we can daemonize
    // daemonize();

    is_running_ = true;
    stop_flag_ = false;

    logger_.info("started agent");
    while (!stop_flag_) {
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

    sock::TcpSocketClient client(logger_);
    if (client.connect("127.0.0.1", DAEMON_PORT)) {
      std::string request = "stop";
      std::string response;

      if (!client.transaction(request, response)) {
        std::cerr << "Failed to send stop command." << std::endl;
        return false;
      }

      if (response != "ok") {
        std::cerr << "Failed to stop agent." << std::endl;
        return false;
      }
    }

    return true;
  }

private:
  void handleRequest(const std::string &request, std::string &response) {
    logger_.info("Received request: " + request);

    if (request == "stop") {
      stop_flag_ = true;
      response = "ok";
    } else if (request == "status") {
      response = "running";
    } else {
      response = "unknown command";
    }

    logger_.info("Sending response: " + response);
  }

  void processSocket() {
    sock::TcpSocketServer client; // Create client socket

    // Check for a new connection (non-blocking)
    if (socket_.accept(client)) {
      std::string request;
      if (client.receive(request)) {
        if (stop_flag_) { // Helps with hanging on shutdown
          client.close();
          return;
        }

        std::string response{};
        handleRequest(request, response);
        if (!client.send(response)) {
          logger_.error("Failed to send response");
        }
        client.close();
      }
    }
  }

  void mainLoop() {
    processSocket();
  }

  /// @brief Set the process to run as a daemon
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

  bool stop_flag_ = false;

  bool is_running_ = false;

  sock::TcpSocketServer socket_{};

  giraffe::CommonLogger<100> logger_{
      giraffe::LoggerLevel::DEBUG, true,
      giraffe::file_paths::getFlightSystemAgentLogFilePath()};
};

} // namespace flight_system_agent