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

static command_line_interface::Daemon g_daemon{};

void printUsage() {
  std::cout << "Usage: gcli <command> <option>\n";
  std::cout << "Commands:\n status\n start\n stop\n version\n configure <key> "
               "<value>    "
               "- only works when the agent is stopped";
  std::cout << "Options: \n";
  std::cout
      << "  -no-daemon | -nd : Run the agent in the foreground, stopping when "
         "the CLI exits\n";
  std::cout << "  -allow-keep-state | -aks : If command is start or stop, exit "
               "gracefully if the agent is already in the desired state\n";
  std::cout << std::endl;
}

void printStatus() {
  std::string response{};
  g_daemon.status(response);
  std::cout << "agent status: " << response << std::endl;
}

bool isRunning() {
  // Check to see if the process is running
  return false;
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << "usage: gcli help" << std::endl;
    printStatus();
    return 0;
  }

  std::vector<std::string> args(&argv[1], &argv[argc]);
  const std::string command = args[0];

  bool no_daemon = false;
  bool allow_keep_state = false;
  for (auto &arg : args) {
    if (arg == "-no-daemon" || arg == "-nd") {
      no_daemon = true;
    }
    if (arg == "-allow-keep-state" || arg == "-aks") {
      allow_keep_state = true;
    }
  }

  if (command == "status") {
    printStatus();
  } else if (command == "help") {
    printUsage();
  } else if (command == "start") {
    if (command_line_interface::Daemon::doesDaemonExist()) {
      std::cout << "Agent already running" << std::endl;
      return allow_keep_state ? 0 : 1;
    }

    if (!g_daemon.start(!no_daemon)) {
      std::cerr << "Failed to start daemon" << std::endl;
      return 1;
    }
  } else if (command == "stop") {
    if (!command_line_interface::Daemon::doesDaemonExist()) {
      std::cout << "Agent not running" << std::endl;
      return allow_keep_state ? 0 : 1;
    }

    if (!g_daemon.stop()) {
      std::cerr << "Failed to stop daemon" << std::endl;
      return 1;
    }
    std::cout << "Daemon stopped" << std::endl;
  } else if (command == "restart") {
    std::cout << "not implemented" << std::endl;
    return 1;
  } else if (command == "configure") {
    if (args.size() < 3) {
      std::cout << "need <key> <value> arguments" << std::endl;
      return 1;
    }

    if (g_daemon.doesDaemonExist()) {
      std::cout << "Agent must be stopped to configure" << std::endl;
      return 1;
    }

    const std::string &setting_key = args.at(1);
    const std::string &setting_value = args.at(2);

    std::string config_response{};
    int return_code = g_daemon.commandLineConfigure(setting_key, setting_value,
                                                    config_response);
    std::cout << config_response << std::endl;
    return return_code;
  } else if (command == "version") {
    std::cout << GIRAFFE_VERSION_NUMBER << "-" << GIRAFFE_VERSION_STAGE
              << std::endl;
  } else {
    std::cout << "usage: gcli help" << std::endl;
    return 1;
  }

  return 0;
}