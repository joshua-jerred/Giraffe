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
#include <sys/types.h>
#include <thread>

#include "gdl_message.hpp"
#include "giraffe_data_link.hpp"
#include "software_physical_layer.hpp"

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

  SoftwarePhysicalLayer physical_layer{config};
  DataLink gdl{config, physical_layer};

  gdl.enable();
  if (is_controller) {
    for (uint32_t i = 0; i < 5; i++) {
      Message message;
      message.setExchangeMessage("hewo_there", i);
      gdl.sendMessage(message);
    }
  }

  while (true) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    Message received_message;
    if (gdl.receiveMessage(received_message)) {
      if (received_message.getType() == Message::Type::EXCHANGE) {
        std::cout << "Received Exchange Message: " << received_message.getData()
                  << "\n";
      } else {
        std::cout << "Received other type of message\n";
      }
    }
  }

  gdl.disable();
}