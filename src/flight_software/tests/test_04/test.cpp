/**
 * @file test.cpp
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief Unit test for the configuration file handler.
 * @date 2023-05-13
 * @copyright Copyright (c) 2023
 */

#include <fstream>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

#include "configuration.h"
#include "configuration_internal.h"
#include "gtest/gtest.h"

using json = nlohmann::ordered_json;

const std::string meta_data_path = "./gfs_configuration_metadata.json";
json gMetaData;

class ConfigurationJson : public ::testing::Test {
 protected:
  static void SetUpTestSuite() {
    std::ifstream fs(meta_data_path);
    if (fs.fail()) {
      FAIL() << "Failed to open meta data json file!";
    }
    gMetaData = json::parse(fs);
  }

  virtual void SetUp() {
  }
  virtual void TearDown() {
  }

  cfg::Configuration config_;
};

testing::AssertionResult HAS_SETTINGS(const json &section_metadata,
                                      const json &section, int num_settings) {
  int settings_count = 0;
  for (auto &[key, val] : section_metadata.items()) {
    settings_count++;
    if (!section.contains(key)) {
      return testing::AssertionFailure()
             << "The key '" << key << "' is missing.";
    }
  }
  EXPECT_EQ(num_settings, settings_count);
  return testing::AssertionSuccess();
}

TEST_F(ConfigurationJson, savesDefaultConfiguration) {
  const std::string path = "default_config.json";
  cfg::file::saveConfiguration(config_, path);
}