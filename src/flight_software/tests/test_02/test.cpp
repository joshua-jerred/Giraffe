/**
 * @file test.cpp
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief Unit test for the configuration file handler.
 * @date 2023-05-13
 * @copyright Copyright (c) 2023
 */

#include <string>

#include "gtest/gtest.h"

#include "gfs_exception.h"

class Exception : public ::testing::Test {
 protected:
  virtual void SetUp() {
  }
  virtual void TearDown() {
  }
};

TEST_F(Exception, baseException) {
  std::string source = "SRC";
  std::string error_code = "E_CODE_1";
  std::string info = "This is the info";
  try {
    throw GfsException(source, error_code, info);
    FAIL();
  } catch (GfsException &e) {
    EXPECT_EQ(e.error_string(), source + "_" + error_code);
    EXPECT_EQ(e.info(), info);
    EXPECT_EQ(std::string(e.what()), "SRC_E_CODE_1 : This is the info");
  }
}