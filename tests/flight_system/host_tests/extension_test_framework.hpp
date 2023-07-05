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
#include "extension.hpp"

class ExtensionTestFramework {
public:
  void printStreams();
  int getTotalDataPackets();
  int getTotalLogPackets();
  int getTotalGpsPackets();
  void sleep(int ms);
  void runExtensionFor(extension::Extension &ext, int ms);

  data::Streams streams{};
  extension::ExtensionResources resources{streams, cfg::gEnum::I2CBus::I2C_1};
  cfg::ExtensionMetadata meta{};
};

#endif /* EXTENSION_TEST_FRAMEWORK_HPP_ */