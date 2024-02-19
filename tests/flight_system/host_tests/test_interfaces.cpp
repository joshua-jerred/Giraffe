/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   test_i2c_interface.cpp
 * @brief  Host test for the I2C interface.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-07-02
 * @copyright  2023 (license to be defined)
 */

#include "unit_test.hpp"
#include <BoosterSeat/sleep.hpp>

#include "gpio.hpp"
#include "i2c_interface.hpp"
#include "spi_interface.hpp"

using namespace giraffe;

#define I2C_TEST
#define SPI_TEST
// #define GPIO_TEST

// choose the sensor for the i2c test
// #define BME280
#define BME280

#ifdef BME280
inline constexpr uint8_t kAddress = 0x76;
inline constexpr uint8_t kIdRegister = 0xD0;
inline constexpr uint8_t kIdValue = 0x60;
#elif defined BMP180
inline constexpr uint8_t kAddress = 0x77;
inline constexpr uint8_t kIdRegister = 0xD0;
inline constexpr uint8_t kIdValue = 0x55;
#else
#error "No sensor defined"
#endif

#ifdef I2C_TEST
TEST(InterfaceTest, i2cInterfaceTest) {
  std::mutex bus_lock{};
  I2cInterface i2c{cfg::gEnum::I2CBus::I2C_1, kAddress, bus_lock};
  EXPECT_EQ(i2c.getAddress(), kAddress);
  EXPECT_EQ(i2c.connect(), I2cInterface::Result::SUCCESS);

  uint8_t id_value = 0;
  I2cInterface::Result result = i2c.readByteFromReg(id_value, kIdRegister);

  EXPECT_EQ(result, I2cInterface::Result::SUCCESS);
  EXPECT_EQ(id_value, kIdValue);
}
#endif

#ifdef SPI_TEST
/**
 * @brief Read the ID register of the gyro on the BMI088 IMU.
 */
TEST(InterfaceTest, spiInterfaceTest) {
  SpiInterface::Config config{};
  config.bits_per_word = 8;
  config.speed_hz = 100000;
  config.delay_usecs = 0;

  SpiInterface spi{config};
  EXPECT_EQ(spi.openDevice("/dev/spidev0.1"), true);

  uint8_t tx_buffer[2] = {0, 0};
  uint8_t rx_buffer[2] = {0, 0};

  tx_buffer[0] = 0b10000000;
  tx_buffer[1] = 0x00;

  EXPECT_EQ(spi.transfer(tx_buffer, rx_buffer, 2), true);
  EXPECT_EQ(rx_buffer[1], 0x0F);
}
#endif

#ifdef GPIO_TEST
TEST(InterfaceTest, gpioInterfaceTest) {
#warning "GPIO test is not complete"

  Gpio::initialize();
  Gpio gpio{};

  // Gpio::Pin pin17{17, Gpio::PinMode::OUTPUT};
  // Gpio::Pin pin27{27, Gpio::PinMode::OUTPUT};
  // Gpio::Pin pin22{22, Gpio::PinMode::OUTPUT};
  gpio.setupPin(pin17);
  gpio.setupPin(pin27);
  gpio.setupPin(pin22);

  gpio.write(pin17, true);
  gpio.write(pin27, true);
  gpio.write(pin22, true);
  bst::sleep(1000);
  gpio.write(pin17, false);
  gpio.write(pin27, false);
  gpio.write(pin22, false);
  bst::sleep(1000);
  gpio.write(pin17, true);
  gpio.write(pin27, true);
  gpio.write(pin22, true);
  bst::sleep(1000);
  gpio.write(pin17, false);
  gpio.write(pin27, false);
  gpio.write(pin22, false);

  // Gpio::Pin pin13{13, Gpio::PinMode::OUTPUT};
  // Gpio::Pin pin12{12, Gpio::PinMode::INPUT};
  //
  // gpio.setupPin(pin13);
  // gpio.setupPin(pin12);
  //
  // gpio.write(pin13, false);
  // EXPECT_EQ(gpio.read(pin12), false);
  //
  // gpio.write(pin13, true);
  // EXPECT_EQ(gpio.read(pin12), true);
  //
  // gpio.write(pin13, false);
  // EXPECT_EQ(gpio.read(pin12), false);
}
#endif