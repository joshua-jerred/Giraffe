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

#include "i2c_interface.hpp"

inline constexpr uint8_t kBme280Address = 0x76;
inline constexpr uint8_t kBme280IdRegister = 0xD0;
inline constexpr uint8_t kBme280IdValue = 0x60;

TEST(I2cInterfaceTest, Bme280IdRegisterRead) {
  std::mutex bus_lock{};
  I2cInterface i2c{cfg::gEnum::I2CBus::I2C_1, kBme280Address, bus_lock};
  EXPECT_EQ(i2c.getAddress(), kBme280Address);
  EXPECT_EQ(i2c.connect(), I2cInterface::Result::SUCCESS);

  uint8_t id_value = 0;
  I2cInterface::Result result =
      i2c.readByteFromReg(id_value, kBme280IdRegister);

  EXPECT_EQ(result, I2cInterface::Result::SUCCESS);
  EXPECT_EQ(id_value, kBme280IdValue);
}