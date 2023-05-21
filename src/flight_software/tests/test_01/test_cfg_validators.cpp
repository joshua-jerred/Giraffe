/**
 * @file test.cpp
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief Unit Tests for the Configuration Validators
 * @date 2023-05-09
 * @copyright Copyright (c) 2023
 */

#include <string>
#include <vector>

#include "configuration_internal.h"
#include "gtest/gtest.h"

class Configuration_Validators : public ::testing::Test {
 protected:
  virtual void SetUp() {
    error = "";
  }
  virtual void TearDown() {
  }

  std::string error = "";
};

TEST_F(Configuration_Validators, General_projectName) {
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

TEST_F(Configuration_Validators, General_mainBoard) {
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

TEST_F(Configuration_Validators, General_startingProcedure) {
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

TEST_F(Configuration_Validators, Interface_consoleUpdateInterval) {
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

TEST_F(Configuration_Validators, Server_tcpSocketPortNumber) {
  std::vector<int> valid_inputs = {1024, 65535, 2000, 2512, 8291, 5000, 10000};
  for (int input : valid_inputs) {
    if (!cfg::server::validators::tcpSocketPort(input, error)) {
      FAIL() << input;
    }
  }

  std::vector<int> invalid_inputs = {50, 79, 101, 1023, 65536};
  for (int input : invalid_inputs) {
    EXPECT_FALSE(cfg::server::validators::tcpSocketPort(input, error)) << input;
  }
}

TEST_F(Configuration_Validators, Telemetry_CallSign) {
  std::vector<std::string> valid_inputs = {
      "A4X",
      "A2AA",
      "A2AAA",
      "AA9AAA"};
  for (std::string input : valid_inputs) {
    if (!cfg::telemetry::validators::callSign(input, error)) {
      FAIL() << input;
    }
  }

  std::vector<std::string> invalid_inputs = {
      "not_an_option",
      "A4",
      "KD1AAAA",
      "AA",
      "A"};
  for (std::string input : invalid_inputs) {
    EXPECT_FALSE(cfg::telemetry::validators::callSign(input, error)) << input;
  }
}

TEST_F(Configuration_Validators, Telemetry_Frequency) {
  std::vector<std::string> valid_inputs = {
      "000.0000",
      "999.9999",
      "123.4567",
  };
  for (std::string input : valid_inputs) {
    if (!cfg::telemetry::validators::frequency(input, error)) {
      FAIL() << input;
    }
  }

  std::vector<std::string> invalid_inputs = {
      "000.1",
      "ABC.efgh",
      "12.1234",
      "123.123"};
  for (std::string input : invalid_inputs) {
    EXPECT_FALSE(cfg::telemetry::validators::frequency(input, error)) << input;
  }
}

TEST_F(Configuration_Validators, APRS_SSID) {
  std::vector<int> valid_inputs = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
  for (int input : valid_inputs) {
    if (!cfg::aprs::validators::ssid(input, error)) {
      FAIL() << input;
    }
  }

  std::vector<int> invalid_inputs = {-1, 16, 17, 18, 20};
  for (int input : invalid_inputs) {
    EXPECT_FALSE(cfg::aprs::validators::ssid(input, error)) << input;
  }
}

/**
 * @todo APRS Symbol CFG Validator unit test. Update with symbol pattern.
 */
TEST_F(Configuration_Validators, APRS_Symbol) {
  std::vector<std::string> valid_inputs = {
      "a",
      "b",
      "c",
      "d"};
  for (std::string input : valid_inputs) {
    if (!cfg::aprs::validators::symbol(input, error)) {
      FAIL() << input << " error: " << error;
    }
  }

  std::vector<std::string> invalid_inputs = {
      "",
      "aa",
      "",
      "aaa",
      "////"};
  for (std::string input : invalid_inputs) {
    EXPECT_FALSE(cfg::aprs::validators::symbol(input, error)) << input;
  }
}

/**
 * @todo APRS Comment CFG Validator unit test. Update with pattern.
 */
TEST_F(Configuration_Validators, APRS_Comment) {
  std::vector<std::string> valid_inputs = {
      "this is a comment",
      "",
      "this is another comment",
      "a"};
  for (std::string input : valid_inputs) {
    if (!cfg::aprs::validators::comment(input, error)) {
      FAIL() << input << " error: " << error;
    }
  }

  std::vector<std::string> invalid_inputs = {
      "this comment is too long.                     "};
  for (std::string input : invalid_inputs) {
    EXPECT_FALSE(cfg::aprs::validators::comment(input, error)) << input;
  }
}

TEST_F(Configuration_Validators, SSTV_Mode) {
  std::vector<std::string> valid_inputs = {
      "robot36"
  };
  for (std::string input : valid_inputs) {
    if (!cfg::sstv::validators::mode(input, error)) {
      FAIL() << input << " error: " << error;
    }
  }

  std::vector<std::string> invalid_inputs = {
      "bpsk125",
      "bpsk",
      "not_an_option"
  };
  for (std::string input : invalid_inputs) {
    EXPECT_FALSE(cfg::sstv::validators::mode(input, error)) << input;
  }
}

TEST_F(Configuration_Validators, DataPackets_Mode) {
  std::vector<std::string> valid_inputs = {
      "bpsk125",
      "bpsk250",
      "bpsk500",
      "bpsk1000",
      "qpsk125",
      "qpsk250",
      "qpsk500",
      "afsk_ax25"};
  for (std::string input : valid_inputs) {
    if (!cfg::data_packets::validators::mode(input, error)) {
      FAIL() << input << " error: " << error;
    }
  }

  std::vector<std::string> invalid_inputs = {
      "bpsk120",
      "bpsk",
      "not_an_option"};
  for (std::string input : invalid_inputs) {
    EXPECT_FALSE(cfg::data_packets::validators::mode(input, error)) << input;
  }
}

/**
 * @todo Data Packets Comment CFG Validator unit test. Update with pattern.
 */
TEST_F(Configuration_Validators, DataPackets_Comment) {
  std::vector<std::string> valid_inputs = {
      "this is a comment",
      "",
      "this is another comment",
      "a"};
  for (std::string input : valid_inputs) {
    if (!cfg::data_packets::validators::comment(input, error)) {
      FAIL() << input << " error: " << error;
    }
  }

  std::vector<std::string> invalid_inputs = {
      "this comment is too long.                                        more input.            comments may only be less than 200 characters;. or they can be empty.                                            "};
  for (std::string input : invalid_inputs) {
    EXPECT_FALSE(cfg::data_packets::validators::comment(input, error)) << input;
  }
}