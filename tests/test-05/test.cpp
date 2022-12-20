/**
 * @file test.cpp
 * @author Joshua Jerred (github.com/joshua-jerred)
 * @brief Tests for the I2C Utility
 * @date 2022-12-19
 * @copyright Copyright (c) 2022
 * @version 0.1.0
 */

#include "gtest/gtest.h"

#include "utility-i2c.h"

#define BAD_BUS_NUMBER 0
#define GOOD_BUS_NUMBER 1
#define NO_DEVICE_ADDRESS 0x09
#define TEST_DEVICE_ADDRESS 0x77 // BMP180

class I2CUtilityTest : public ::testing::Test {
protected:
    virtual void SetUp() {
    }
    virtual void TearDown() {
    }
};

TEST_F(I2CUtilityTest, I2CTestBadBusNumber) {
    I2C i2c(0, 0x00);
    EXPECT_EQ(i2c.status(), I2C_STATUS::NOT_CONNECTED);
}

TEST_F(I2CUtilityTest, I2CTestBadAddress) {
    I2C i2c(GOOD_BUS_NUMBER, 0x00);
    EXPECT_EQ(i2c.status(), I2C_STATUS::NOT_CONNECTED);
    EXPECT_EQ(i2c.connect(), -1);
    EXPECT_EQ(i2c.status(), I2C_STATUS::CONFIG_ERROR_ADDRESS);
}

TEST_F(I2CUtilityTest, I2CTestNoDevice) {
    I2C i2c(GOOD_BUS_NUMBER, NO_DEVICE_ADDRESS);
    EXPECT_EQ(i2c.connect(), -1);
    EXPECT_EQ(i2c.status(), I2C_STATUS::ADDRESS_ERROR);
}

TEST_F(I2CUtilityTest, I2CBMP180Read) {
    I2C i2c(GOOD_BUS_NUMBER, TEST_DEVICE_ADDRESS);
    EXPECT_EQ(i2c.connect(), 0);
    EXPECT_EQ(i2c.status(), I2C_STATUS::OK);
    EXPECT_EQ(i2c.disconnect(), 0);
}