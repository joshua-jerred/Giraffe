/// =*========GIRAFFE========*=
/// A Unified Flight Command and Control System
/// https://github.com/joshua-jerred/Giraffe
/// https://giraffe.joshuajer.red/
/// =*=======================*=
///
/// @file   main.cpp
///
/// =*=======================*=
/// @author     Joshua Jerred (https://joshuajer.red)
/// @date       2024-11-23
/// @copyright  2024 (license to be defined)

#include <iostream>
#include <string>
#include <vector>

#include <sys/stat.h>

#include <BoosterSeat/sleep.hpp>

#include "daemon.hpp"

static flight_system_agent::Daemon g_daemon{};

void printUsage() {
  std::cout << "Usage: fsa <command> <option>\n";
  std::cout << "Commands: status, start, stop\n";
  std::cout << "Options: \n";
  std::cout
      << "  -no-daemon | -nd : Run the agent in the foreground, stopping when "
         "the CLI exits\n";
  std::cout << std::endl;
}

bool isRunning() {
  // Check to see if the process is running
  return false;
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printUsage();
    return 1;
  }

  std::vector<std::string> args(&argv[1], &argv[argc]);
  const std::string command = args[0];

  bool no_daemon = false;
  for (auto &arg : args) {
    if (arg == "-no-daemon" || arg == "-nd") {
      no_daemon = true;
    }
  }

  if (command == "status") {
    std::string response{};
    g_daemon.status(response);
    std::cout << "Agent status: " << response << std::endl;
  } else if (command == "start") {
    if (flight_system_agent::Daemon::doesDaemonExist()) {
      std::cout << "Agent already running" << std::endl;
      return 1;
    }

    if (!g_daemon.start(!no_daemon)) {
      std::cerr << "Failed to start daemon" << std::endl;
      return 1;
    }
  } else if (command == "stop") {
    if (!g_daemon.stop()) {
      std::cerr << "Failed to stop daemon" << std::endl;
      return 1;
    }
    std::cout << "Daemon stopped" << std::endl;
  } else if (command == "restart") {
    std::cout << "not implemented" << std::endl;
  } else {
    printUsage();
    return 1;
  }

  // daemonize();
  // mainLoop();

  return 0;
}