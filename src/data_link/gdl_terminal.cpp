/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   gdl_terminal.cpp
 * @brief  GDL Terminal
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-01-18
 * @copyright  2024 (license to be defined)
 */

#include <chrono>
#include <iostream>
#include <thread>

#include "giraffe_data_link.hpp"

using namespace giraffe::gdl;

int main(int argc, char **argv) {
  std::cout << "GDL Terminal\n";
  bool is_controller = false;
  if (argc > 1) {
    is_controller = true;
    std::cout << "Controller Mode\n";
  }

  Config config{is_controller};

  if (is_controller) {
    config.setCallSign("CONT");
    config.setRemoteCallSign("REMO");
  } else {
    config.setCallSign("REMO");
    config.setRemoteCallSign("CONT");
  }

  DataLink gdl{config};

  gdl.enable();

  while (true) {
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }

  gdl.disable();
}