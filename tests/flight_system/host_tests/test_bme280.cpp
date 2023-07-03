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

#include <BoosterSeat/sleep.hpp>
#include <iostream>

#include "unit_test.hpp"

#include "bme280.hpp"
#include "extension_test_framework.hpp"
#include "to_string.hpp"

TEST(BME280, BME280Test) {
  ExtensionTestFramework test_framework{};
  cfg::ExtensionMetadata metadata{};
  metadata.update_interval = 1000;

  extension::Bme280Extension bme280{test_framework.resources, metadata};

  bme280.start();
  BoosterSeat::threadSleep(2000);
  bme280.stop();

  auto &ds = test_framework.resources.streams.data;
  auto &ls = test_framework.resources.streams.log;

  EXPECT_GT(ds.getNumPackets(), 0);
  EXPECT_EQ(ls.getNumPackets(), 0);

  data::DataPacket d_packet;

  while (ds.getPacket(d_packet)) {
    std::cout << util::to_string(d_packet) << std::endl;
  }

  data::LogPacket l_packet;
  while (ls.getPacket(l_packet)) {
    std::cout << util::to_string(l_packet) << std::endl;
  }
}