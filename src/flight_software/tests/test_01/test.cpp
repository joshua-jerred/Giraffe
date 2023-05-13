/**
 * @file test.cpp
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief Unit Tests for the Configuration Validators
 * @date 2023-05-09
 * @copyright Copyright (c) 2023
 */

#include <string>
#include <vector>

#include "gtest/gtest.h"
#include "configuration_internal.h"

class ConfigurationValidators : public ::testing::Test {
 protected:
  virtual void SetUp() {
    error = "";
  }
  virtual void TearDown() {
  }

  std::string error;
};

TEST_F(ConfigurationValidators, General_projectName) {
  
  std::vector<std::string> valid_inputs = {
      "a",
      "b",
      "name with spaces",
      "name-with-dashes",
      "a               b",
      "12345678901234567890",
      "MiXeD-Cases_ _123"};
  for (std::string input : valid_inputs) {
    if (!cfg::general::validators::projectName(input, error)) {
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
    EXPECT_FALSE(cfg::general::validators::projectName(input, error)) << input;
  }
}

TEST_F(ConfigurationValidators, General_mainBoard) {
  std::vector<std::string> valid_inputs = {
      "other",
      "pi_zero_w_2",
      "pi_4"};
  for (std::string input : valid_inputs) {
    if (!cfg::general::validators::mainBoard(input, error)) {
      FAIL();
    }
  }

  std::vector<std::string> invalid_inputs = {
      "not_an_option",
      "another",
      "does_not_exist",
      "",
      "ascent"};
  for (std::string input : invalid_inputs) {
    EXPECT_FALSE(cfg::general::validators::mainBoard(input, error)) << input;
  }
}

TEST_F(ConfigurationValidators, General_startingProcedure) {
  std::vector<std::string> valid_inputs = {
      "testing",
      "pre_launch",
      "ascent",
      "descent",
      "recovery",
      "failsafe"};
  for (std::string input : valid_inputs) {
    if (!cfg::general::validators::startingProcedure(input, error)) {
      FAIL();
    }
  }

  std::vector<std::string> invalid_inputs = {
      "not_an_option",
      "another",
      "does_not_exist",
      "pi_zero_w_2",
      "1234"};
  for (std::string input : invalid_inputs) {
    EXPECT_FALSE(cfg::general::validators::startingProcedure(input, error)) << input;
  }
}

TEST_F(ConfigurationValidators, Interface_consoleUpdateInterval) {
  std::vector<int> valid_inputs = {100, 200, 300, 400, 500, 1000, 5000, 10000};
  for (int input : valid_inputs) {
    if (!cfg::debug::validators::consoleUpdateInterval(input, error)) {
      FAIL();
    }
  }

  std::vector<int> invalid_inputs = {50, 79, 101, 5050, 10100};
  for (int input : invalid_inputs) {
    EXPECT_FALSE(cfg::debug::validators::consoleUpdateInterval(input, error)) << input;
  }
}

TEST_F(ConfigurationValidators, Server_tcpSocketPortNumber) {
  std::vector<int> valid_inputs = {1024, 65535, 2000, 2512, 8291, 5000, 10000};
  for (int input : valid_inputs) {
    if (!cfg::server::validators::tcpSocketPort(input, error)) {
      FAIL();
    }
  }

  std::vector<int> invalid_inputs = {50, 79, 101, 1023, 65536};
  for (int input : invalid_inputs) {
    EXPECT_FALSE(cfg::server::validators::tcpSocketPort(input, error)) << input;
  }
}