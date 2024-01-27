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

TEST(I2cInterfaceTest, idRegisterRead) {
  std::mutex bus_lock{};
  I2cInterface i2c{cfg::gEnum::I2CBus::I2C_1, kAddress, bus_lock};
  EXPECT_EQ(i2c.getAddress(), kAddress);
  EXPECT_EQ(i2c.connect(), I2cInterface::Result::SUCCESS);

  uint8_t id_value = 0;
  I2cInterface::Result result = i2c.readByteFromReg(id_value, kIdRegister);

  EXPECT_EQ(result, I2cInterface::Result::SUCCESS);
  EXPECT_EQ(id_value, kIdValue);
}