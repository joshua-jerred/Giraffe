/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   extension_test_framework.cpp
 * @brief  Test framework for testing extensions on the host.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-07-03
 * @copyright  2023 (license to be defined)
 */

#include <iostream>

#include <BoosterSeat/sleep.hpp>

#include "to_string.hpp"

#include "extension_test_framework.hpp"

void ExtensionTestFramework::printStreams() {
  auto &ds = streams.data;
  auto &ls = streams.log;

  std::cout << "Data Packets:" << std::endl;
  data::DataPacket d_packet;
  while (ds.getPacket(d_packet)) {
    std::cout << util::to_string(d_packet) << std::endl;
  }

  std::cout << "Log Packets:" << std::endl;
  data::LogPacket l_packet;
  while (ls.getPacket(l_packet)) {
    std::cout << util::to_string(l_packet) << std::endl;
  }
}

int ExtensionTestFramework::getTotalDataPackets() {
  return streams.data.getTotalPackets();
}

int ExtensionTestFramework::getTotalLogPackets() {
  return streams.log.getTotalPackets();
}

void ExtensionTestFramework::sleep(int ms) {
  BoosterSeat::threadSleep(ms);
}

void ExtensionTestFramework::runExtensionFor(extension::Extension &ext,
                                             int ms) {
  ext.start();
  sleep(ms);
  ext.stop();
}