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

  // Verify that those settings exist, and that they have the appropriate
  // type.
  void verifyFieldsAndTypes(json cfg_to_check) {
    for (auto &[section_key, section_val] : gMetaData.items()) {
      json section = section_val;
      for (auto &[key, val] : section.items()) {
        std::string setting = key;
        std::string type = val["type"].get<std::string>();

        ASSERT_TRUE(cfg_to_check[section_key].contains(setting))
            << "Missing: "
            << "[" << section_key << "][" << setting << "] typ:" << type;

        if (type == "string" || type == "enum") {
          EXPECT_TRUE(cfg_to_check[section_key][setting].is_string())
              << "[" << section_key << "][" << setting
              << "] is not a string/enum string";
        } else if (type == "bool") {
          EXPECT_TRUE(cfg_to_check[section_key][setting].is_boolean())
              << "[" << section_key << "][" << setting << "] is not a boolean";
        } else if (type == "int") {
          EXPECT_TRUE(cfg_to_check[section_key][setting].is_number_integer())
              << "[" << section_key << "][" << setting << "] is not an int";
        } else {
          FAIL() << "Missing Type: " << type;
        }
      }
    }
  }
};

TEST_F(Configuration_File, savesDefaultConfiguration) {
  cfg::Configuration config;
  const std::string path = "default_config.json";
  cfg::file::saveConfiguration(config, path);

  ASSERT_TRUE(std::filesystem::exists(path))
      << "File does not exist when it should.";

  // Load in the same file
  std::ifstream file(path);
  json saved_cfg = json::parse(file);

  verifyFieldsAndTypes(saved_cfg);
}

TEST_F(Configuration_File, savesChangedConfiguration) {
  cfg::Configuration config;
  const std::string path = "changed_config.json";

  cfg::General new_general;
  new_general.project_name = "New Project Name";
  new_general.main_board_type = cfg::General::MainBoard::PI_4;
  new_general.starting_procedure = cfg::Procedure::Type::ASCENT;

  std::string error;
  ASSERT_TRUE(config.setGeneral(new_general, error)) << "Error: " << error;

  cfg::file::saveConfiguration(config, path);

  ASSERT_TRUE(std::filesystem::exists(path))
      << "File does not exist when it should.";

  // Load in the same file
  std::ifstream file(path);
  json saved_cfg = json::parse(file);
  verifyFieldsAndTypes(saved_cfg);
}

TEST_F(Configuration_File, loadsChangedConfiguration) {
  cfg::Configuration config;
  const std::string path = "config_to_load.json";

  cfg::General new_general;
  new_general.project_name = "New Project Name";
  new_general.main_board_type = cfg::General::MainBoard::PI_4;
  new_general.starting_procedure = cfg::Procedure::Type::ASCENT;

  std::string error;
  ASSERT_TRUE(config.setGeneral(new_general, error)) << "Error: " << error;

  cfg::file::saveConfiguration(config, path);

  ASSERT_TRUE(std::filesystem::exists(path))
      << "File does not exist when it should.";

  // Load in the same file
  cfg::Configuration new_config;
  cfg::file::loadConfiguration(new_config, path);
  cfg::General loaded_general = new_config.getGeneral();

  EXPECT_EQ(loaded_general.project_name, new_general.project_name);
  EXPECT_EQ(loaded_general.main_board_type, new_general.main_board_type);
  EXPECT_EQ(loaded_general.starting_procedure, new_general.starting_procedure);
}