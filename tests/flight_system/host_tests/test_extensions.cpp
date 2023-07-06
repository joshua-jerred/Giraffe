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

#include "extension_test_framework.hpp"
#include "unit_test.hpp"

#include "bme280.hpp"
#include "ds18b20.hpp"
#include "mcp3021.hpp"
#include "samm8q.hpp"

#define BME280_TEST_ENABLED 0
#define DS18B20_TEST_ENABLED 0
#define SAMM8Q_TEST_ENABLED 0
#define MCP3021_TEST_ENABLED 1

#if BME280_TEST_ENABLED
TEST(ExtensionsTest, BME280Test) {
  ExtensionTestFramework tf{};
  tf.meta.update_interval = 1000;

  extension::Bme280Extension bme280{tf.resources, tf.meta};

  tf.runExtensionFor(bme280, 2000);

  EXPECT_GT(tf.getTotalDataPackets(), 0);
  EXPECT_EQ(tf.getTotalLogPackets(), 0);

  tf.printStreams();
}
#endif

#if DS18B20_TEST_ENABLED
TEST(ExtensionsTest, DS18B20Test) {
  ExtensionTestFramework tf{};
  tf.meta.update_interval = 1000;
  tf.meta.extra_args = "28-03176038beff";

  extension::Ds18b20Extension ds18b20{tf.resources, tf.meta};

  tf.runExtensionFor(ds18b20, 2000);

  EXPECT_GT(tf.getTotalDataPackets(), 0);
  EXPECT_EQ(tf.getTotalLogPackets(), 0);

  tf.printStreams();
}
#endif

#if SAMM8Q_TEST_ENABLED
TEST(ExtensionsTest, SAMM8QTest) {
  ExtensionTestFramework tf{};
  tf.meta.update_interval = 1000;

  extension::SamM8qExtension sam_m8q{tf.resources, tf.meta};

  tf.runExtensionFor(sam_m8q, 10000);

  EXPECT_GT(tf.getTotalGpsPackets(), 0);
  EXPECT_EQ(tf.getTotalLogPackets(), 0);

  tf.printStreams();
}
#endif

#if MCP3021_TEST_ENABLED
TEST(ExtensionsTest, MCP3021Test) {
  ExtensionTestFramework tf{};
  tf.meta.update_interval = 500;
  tf.meta.extra_args = "type=v_div,r1=30000,r2=7500,v_ref=3385";
  extension::Mcp3021Extension mcp3021{tf.resources, tf.meta};

  tf.runExtensionFor(mcp3021, 2000);

  EXPECT_GT(tf.getTotalDataPackets(), 0);
  EXPECT_EQ(tf.getTotalLogPackets(), 0);

  tf.printStreams();
}
#endif