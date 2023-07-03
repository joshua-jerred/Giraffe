/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   test_bme280.cpp
 * @brief  Test of the BME280 extension.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-07-02
 * @copyright  2023 (license to be defined)
 */

#include "unit_test.hpp"

#include "extension_test_framework.hpp"

#include "bme280.hpp"

TEST(BME280, BME280Test) {
  ExtensionTestFramework test_framework{};
  cfg::ExtensionMetadata metadata{};

  extension::Bme280Extension bme280{test_framework.resources, metadata};
}