/**
 * @file test.cpp
 * @author Joshua Jerred (github.com/joshua-jerred)
 * @brief Tests for the I2C Utility
 * @date 2022-12-26
 * @copyright Copyright (c) 2022
 * @version 0.1.0
 */

#include "gtest/gtest.h"

#include "ubx.h"

#define BUS_NUMBER 1
#define DEVICE_ADDRESS 0x42

class UBXTest : public ::testing::Test {
protected:
    virtual void SetUp() {
    }
    virtual void TearDown() {
    }
};

TEST_F(UBXTest, UBXTestConfigure) {
    I2C i2c(BUS_NUMBER, DEVICE_ADDRESS);
}