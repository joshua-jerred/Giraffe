/**
 * @file test.cpp
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief Unit test for the configuration class.
 * @date 2023-05-13
 * @copyright Copyright (c) 2023
 */

#include <time.h>

#include <filesystem>
#include <string>

#include "configuration.h"
#include "configuration_internal.h"
#include "gtest/gtest.h"

namespace cfg { class Configuration; }
void setGeneral(cfg::Configuration &config);

class ConfigurationClass : public ::testing::Test {
 protected:
  virtual void SetUp() {
  }
  virtual void TearDown() {
  }

  cfg::Configuration cfg;
};

TEST_F(ConfigurationClass, SetsDefaults) {
  cfg::General general = cfg.getGeneral();

  // General
  ASSERT_EQ(general.project_name, cfg::general::defaults::project_name);
  ASSERT_EQ(general.main_board_type, cfg::general::defaults::main_board);
  ASSERT_EQ(general.starting_procedure, cfg::general::defaults::starting_procedure);
}