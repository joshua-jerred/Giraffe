/**
 * @file test.cpp
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief Unit Tests for the Configuration Definition
 * @date 2023-05-09
 * @copyright Copyright (c) 2023
 */

#include <string>
#include <vector>

#include "gtest/gtest.h"
#include "validators.h"

class TEST_Configuration_Validators : public ::testing::Test {
 protected:
  virtual void SetUp() {
  }
  virtual void TearDown() {
  }
};

TEST_F(TEST_Configuration_Validators, General_ProjectName) {
  std::string error;

  std::vector<std::string> valid_inputs = {
      "a",
      "b",
      "name with spaces",
      "name-with-dashes",
      "a               b",
      "12345678901234567890",
      "MiXeD-Cases_ _123"};
  for (std::string input : valid_inputs) {
    if (!cfg::validator::general::projectName(input, error)) {
      FAIL() << error;
    }
  }

  std::vector<std::string> invalid_inputs = {
      " a",
      " starts with space",
      "ends with space ",
      "",
      "123456789012345678901"};
  for (std::string input : invalid_inputs) {
    EXPECT_FALSE(cfg::validator::general::projectName(input, error)) << input;
  }
}

TEST_F(TEST_Configuration_Validators, General_MainBoard) {
  std::vector<std::string> valid_inputs = {
      "other",
      "pi_zero_w_2",
      "pi_4"};
  for (std::string input : valid_inputs) {
    if (!cfg::validator::general::mainBoard(input)) {
      FAIL();
    }
  }

  std::vector<std::string> invalid_inputs = {
      " a",
      "error",
      "does_not_exist",
      "",
      "1234"};
  for (std::string input : invalid_inputs) {
    EXPECT_FALSE(cfg::validator::general::mainBoard(input)) << input;
  }
}