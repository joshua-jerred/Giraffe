/**
 * @file test.cpp
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief Unit Tests for the Configuration Definition
 * @date 2023-05-09
 * @copyright Copyright (c) 2023
 */

#include <time.h>

#include <string>
#include <iostream> // @todo remove this
#include <filesystem>

#include "configuration.h"
#include "configuration_defaults.h"
#include "gtest/gtest.h"
#include "yaml-cpp/yaml.h"

class TEST_Configuration : public ::testing::Test {
 protected:
  virtual void SetUp() {
  }
  virtual void TearDown() {
  }
};

TEST_F(TEST_Configuration, LoadFromFile_DoesNotExist) {
  Configuration config;
  try {
    config.loadFromFile("test.yaml");
    FAIL() << "Exception should have been thrown";
  } catch (ConfigurationError &e) {
  }
}

TEST_F(TEST_Configuration, LoadFromFile_EmptyFile) {
  Configuration config;
  try {
    config.loadFromFile("01_empty_config.yaml");
    FAIL() << "Exception should have been thrown";
  } catch (ConfigurationError &e) {
  }
}

TEST_F(TEST_Configuration, LoadFromFile_Invalid) {
  Configuration config;
  try {
    config.loadFromFile("01_invalid_config.yaml");
    FAIL() << "Exception should have been thrown";
  } catch (ConfigurationError &e) {
  }
}

TEST_F(TEST_Configuration, LoadFromFile_Valid) {
  Configuration config;
  try {
    config.loadFromFile("01_valid_config.yaml");
  } catch (ConfigurationError &e) {
    FAIL() << "Exception should not have been thrown";
  }
}

TEST_F(TEST_Configuration, LoadFromFile_DefaultsLoadedOnFailure) {
  Configuration config;
  try {
    config.loadFromFile("non_existant_file.yaml");
    FAIL() << "Exception should not have been thrown";
  } catch (ConfigurationError &e) {
  }

  cfg::General gen = config.getGeneral();
  EXPECT_EQ(cfg::defaults::general::project_name, gen.project_name);
  EXPECT_EQ(cfg::defaults::general::main_board, gen.main_board_type);
}

TEST_F(TEST_Configuration, SaveToFile_AfterLoadFailure) {
  Configuration config;
  try {
    config.loadFromFile("non_existant_file.yaml");
    FAIL() << "Exception should not have been thrown";
  } catch (ConfigurationError &e) {
  }

  const std::string out_file = "01_out_1.yaml";
  config.saveToFile(out_file);
  if (!std::filesystem::exists(out_file)) {
    FAIL() << "saveToFile did not save a file.";
  }
}