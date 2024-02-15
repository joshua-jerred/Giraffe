/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   main.cpp
 * @brief  Giraffe Data Link Server Main File
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-01-13
 * @copyright  2024 (license to be defined)
 */

#include <csignal>

#include "gdl_server.hpp"

giraffe::gdl::GdlServer server{};

auto signalHandler(int signal_number) -> void {
  signal(SIGINT, signalHandler);
  if (signal_number == SIGINT) {
    server.stop();
  }
}

int main() {
  signal(SIGINT, signalHandler); // Register signal handler
  return server.run();
}