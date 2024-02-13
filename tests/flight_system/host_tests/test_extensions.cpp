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
#include "bmi088.hpp"
#include "ds18b20.hpp"
#include "mcp3021.hpp"
#include "rgb_status_led.hpp"
#include "samm8q.hpp"

#define BME280_TEST_ENABLED 0
#define DS18B20_TEST_ENABLED 0
#define SAMM8Q_TEST_ENABLED 0
#define MCP3021_TEST_ENABLED 0
#define BMI088_TEST_ENABLED 0
#define RGB_STATUS_LED_TEST_ENABLED 0

#if BME280_TEST_ENABLED
TEST(ExtensionsTest, BME280Test) {
  ExtensionTestFramework tf{};
  tf.meta.update_interval = 500;

  extension::Bme280 bme280{tf.resources, tf.meta};

  tf.runExtensionFor(bme280, 1000);

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
  tf.meta.update_interval = 500;

  extension::SamM8q sam_m8q{tf.resources, tf.meta};

  tf.runExtensionFor(sam_m8q, 5000);

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

#if BMI088_TEST_ENABLED
TEST(ExtensionsTest, BMI088Test) {
  ExtensionTestFramework tf{};
  tf.meta.update_interval = 500;
  tf.meta.extra_args = "/dev/spidev0.0,/dev/spidev0.1";
  extension::Bmi088 bmi088{tf.resources, tf.meta};

  tf.runExtensionFor(bmi088, 2000);

  EXPECT_GT(tf.getTotalImuPackets(), 0);
  EXPECT_EQ(tf.getTotalLogPackets(), 0);

  tf.printStreams();
}
#endif

#if RGB_STATUS_LED_TEST_ENABLED
#include "gpio.hpp"

TEST(ExtensionsTest, RgbStatusLedTest) {
  giraffe::Gpio::initialize();

  ExtensionTestFramework tf{};
  tf.status_led.setRed(giraffe::StatusLedState::State::BLINK);
  tf.status_led.setGreen(giraffe::StatusLedState::State::ON);
  tf.status_led.setBlue(giraffe::StatusLedState::State::BLINK);
  tf.meta.update_interval = 500;
  tf.meta.extra_args = "17,27,22";
  extension::RgbStatusLed rgb_status_led{tf.resources, tf.meta};

  tf.runExtensionFor(rgb_status_led, 2000);

  EXPECT_GT(tf.getTotalLogPackets(), 0);

  tf.printStreams();
}
#endif