/**
 * @file test.cpp
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief Unit test for the configuration file handler.
 * @date 2023-05-13
 * @copyright Copyright (c) 2023
 */

#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

#include "configuration.h"
#include "configuration_internal.h"
#include "gtest/gtest.h"

using json = nlohmann::ordered_json;

const std::string meta_data_path = "./gfs_configuration_metadata.json";
json gMetaData;

class Configuration_File : public ::testing::Test {
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

TEST_F(Configuration_File, savesDefaultConfiguration) {
  const std::string path = "default_config.json";
  cfg::file::saveConfiguration(config_, path);

  ASSERT_TRUE(std::filesystem::exists(path))
      << "File does not exist when it should.";

  // Load in the same file
  std::ifstream file(path);
  json saved_cfg = json::parse(file);

  // Verify that those settings were saved, and that they have the appropriate
  // type.
  for (auto &[section_key, section_val] : gMetaData.items()) {
    json section = section_val;
    for (auto &[key, val] : section.items()) {
      std::string setting = key;
      std::string type = val["type"].get<std::string>();
      //std::cout << "[" << section_key << "][" << setting << "] typ:" << type
      //          << std::endl;

      ASSERT_TRUE(saved_cfg[section_key].contains(setting))
          << "Missing: "
          << "[" << section_key << "][" << setting << "] typ:" << type;

      if (type == "string" || type == "enum") {
        EXPECT_TRUE(saved_cfg[section_key][setting].is_string())
            << "[" << section_key << "][" << setting
            << "] is not a string/enum string";
      } else if (type == "bool") {
        EXPECT_TRUE(saved_cfg[section_key][setting].is_boolean())
            << "[" << section_key << "][" << setting << "] is not a boolean";
      } else if (type == "int") {
        EXPECT_TRUE(saved_cfg[section_key][setting].is_number_integer())
            << "[" << section_key << "][" << setting << "] is not an int";
      } else {
        FAIL() << "Missing Type: " << type;
      }
    }
  }

  
}