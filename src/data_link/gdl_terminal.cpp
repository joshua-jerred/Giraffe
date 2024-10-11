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

void userInput(std::atomic<bool> &running, std::mutex &input_mutex,
               std::string &input) {
  while (running) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::cout << " >> ";

    std::string input_buffer;
    std::getline(std::cin, input_buffer);

    if (input_buffer == "q") {
      running = false;
      std::cout << "Quitting\n";
      break;
    }

    input_mutex.lock();
    input = input_buffer;
    input_mutex.unlock();
  }
}

int main(int argc, char **argv) {
  (void)argv;
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

  std::shared_ptr<SoftwarePhysicalLayer> physical_layer =
      std::make_shared<SoftwarePhysicalLayer>(config);
  DataLink gdl{config, physical_layer};

  AprsTelemetryManager::TelemetryData telemetry_data;
  using ParId = signal_easel::aprs::telemetry::Parameter::Id;
  telemetry_data.getAnalog(ParId::A1).setRawValue(20);
  telemetry_data.getAnalog(ParId::A1).setCoefficientB("2");
  telemetry_data.getAnalog(ParId::A1).setName("test");
  telemetry_data.getAnalog(ParId::A1).setUnitOrLabel("m");
  telemetry_data.getAnalog(ParId::A2).setRawValue(50);
  telemetry_data.setComment("GDL Terminal");
  telemetry_data.setProjectTitle("GDL Terminal");
  telemetry_data.getDigital(ParId::B1).setName("b1");
  telemetry_data.getDigital(ParId::B1).setUnitOrLabel("b1");
  telemetry_data.getDigital(ParId::B1).setBitSense(false);
  telemetry_data.getDigital(ParId::B2).setBitSense(true);
  telemetry_data.getDigital(ParId::B3).setBitSense(false);

  gdl.enable();

  // Input Thread
  std::atomic<bool> running = true;
  std::mutex input_mutex;
  std::string input;
  std::thread input_thread(userInput, std::ref(running), std::ref(input_mutex),
                           std::ref(input));

  static int count = 0;
  while (running) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    Message received_message;
    if (gdl.receiveMessage(received_message)) {
      if (received_message.getType() == Message::Type::EXCHANGE) {
        std::cout << "Received Exchange Message: " << received_message.getData()
                  << "\n";
      } else if (received_message.getType() == Message::Type::BROADCAST) {
        std::cout << "Received Broadcast Message: "
                  << received_message.getData() << "\n";
      } else if (received_message.getType() == Message::Type::TELEMETRY) {
        std::cout << "Received Telemetry Message\n";
      } else {
        std::cout << "Received other type of message\n";
      }
    }

    input_mutex.lock();
    if (input.empty()) {
      input_mutex.unlock();
      continue;
    }
    std::string input_copy = input;
    input.clear();
    input_mutex.unlock();

    if (input_copy.substr(0, 2) == "id") {
      is_controller = !is_controller;
      if (is_controller) {
        config.setCallSign("CONT");
        config.setRemoteCallSign("REMO");
        std::cout << "Controller Mode\n";
      } else {
        config.setCallSign("REMO");
        config.setRemoteCallSign("CONT");
        std::cout << "Remote Mode\n";
      }
    } else if (input_copy.at(0) == 'e') {
      Message message;
      message.setExchangeMessage(input_copy.substr(2), count++);
      gdl.sendMessage(message);
      std::cout << "added exchange" << std::endl;
    } else if (input_copy.at(0) == 'b') {
      Message message;
      message.setBroadcastMessage(input_copy.substr(2), count++);
      gdl.sendMessage(message);
      std::cout << "added" << std::endl;
    } else if (input_copy == "td") {
      Message message;
      gdl.sendTelemetryData(telemetry_data, 0);
      std::cout << "added" << std::endl;
    } else if (input_copy == "tp") {
      Message message;
      gdl.sendTelemetryParameterNames(telemetry_data);
      std::cout << "added" << std::endl;
    } else if (input_copy == "tu") {
      Message message;
      gdl.sendTelemetryUnitsAndLabels(telemetry_data);
      std::cout << "added" << std::endl;
    } else if (input_copy == "tc") {
      Message message;
      gdl.sendTelemetryCoefficients(telemetry_data);
      std::cout << "added" << std::endl;
    } else if (input_copy == "tb") {
      Message message;
      gdl.sendTelemetryBitSense(telemetry_data);
      std::cout << "added" << std::endl;
    } else {
      std::cout << "Commands:\n";
      std::cout << "id - Toggle Controller/Remote\n";
      std::cout << "e <message> - Send Exchange Message\n";
      std::cout << "b <message> - Send Broadcast Message\n";
      std::cout << "td - Send Telemetry Data\n";
      std::cout << "tp - Send Telemetry Parameter Names\n";
      std::cout << "tu - Send Telemetry Units/Labels\n";
      std::cout << "tc - Send Telemetry Coefficients\n";
      std::cout << "tb - Send Telemetry Bit Sense\n";
    }
  }

  input_thread.join();
  gdl.disable();
}