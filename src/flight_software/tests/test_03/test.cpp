/**
 * @file test.cpp
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief Unit test for the configuration file handler.
 * @date 2023-05-13
 * @copyright Copyright (c) 2023
 */

#include <string>

#include "configuration.h"
#include "configuration_internal.h"
#include "gtest/gtest.h"

class ConfigurationFile : public ::testing::Test {
 protected:
  virtual void SetUp() {
  }
  virtual void TearDown() {
  }

  cfg::Configuration config;
};

TEST_F(ConfigurationFile, SavesAFile) {
  cfg::General general = config.getGeneral();

  ASSERT_EQ(general.project_name, cfg::general::defaults::project_name);
  ASSERT_EQ(general.main_board_type, cfg::general::defaults::main_board);
  ASSERT_EQ(general.starting_procedure, cfg::general::defaults::starting_procedure);
}