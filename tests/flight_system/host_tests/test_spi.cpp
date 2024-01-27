/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   test_spi.cpp
 * @brief  Simple SPI test for the Giraffe flight system. Requires the BMI088
 * IMU to be connected to the SPI bus.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-01-26
 * @copyright  2024 (license to be defined)
 */

#include "unit_test.hpp"

#include "spi_interface.hpp"

using namespace giraffe;

/**
 * @brief Read the ID register of the gyro on the BMI088 IMU.
 */
TEST(SpiInterfaceTest, bmi088_id_register) {
  SpiInterface::Config config{};
  config.bits_per_word = 8;
  config.speed_hz = 100000;
  config.delay_usecs = 0;

  SpiInterface spi{"/dev/spidev0.1", config};
  EXPECT_EQ(spi.openDevice(), true);

  uint8_t tx_buffer[2] = {0, 0};
  uint8_t rx_buffer[2] = {0, 0};

  tx_buffer[0] = 0b10000000;
  tx_buffer[1] = 0x00;

  EXPECT_EQ(spi.transfer(tx_buffer, rx_buffer, 2), true);
  EXPECT_EQ(rx_buffer[1], 0x0F);
}