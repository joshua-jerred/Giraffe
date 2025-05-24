/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   extension_test_framework.hpp
 * @brief  Basic helpers for unit testing extensions on the host.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-07-02
 * @copyright  2023 (license to be defined)
 */

#ifndef EXTENSION_TEST_FRAMEWORK_HPP_
#define EXTENSION_TEST_FRAMEWORK_HPP_

#include "configuration_enums.hpp"
#include "extension_base.hpp"
#include "status_led.hpp"

class ExtensionTestFramework {
public:
  void printStreams();
  int getTotalDataPackets();
  int getTotalLogPackets();
  int getTotalGpsPackets();
  int getTotalImuPackets();
  void sleep(int ms);
  void runExtensionFor(extension::Extension &ext, int ms);

  data::Streams streams{};
  giraffe::StatusLedState status_led{};
  extension::ExtensionResources resources{streams, cfg::gEnum::I2CBus::I2C_1,
                                          status_led};
  cfg::ExtensionMetadata meta{};
};

#endif /* EXTENSION_TEST_FRAMEWORK_HPP_ */