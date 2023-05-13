/**
 * @file test.cpp
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief Unit Tests for the common configuration scheme. This will help to
 * verify the standardization of setting metadata.
 * @date 2023-05-09
 * @copyright Copyright (c) 2023
 */

#include <array>
#include <fstream>
#include <nlohmann/json.hpp>
#include <string>

#include "gtest/gtest.h"

inline constexpr int setting_name_min_length = 4;
inline constexpr int setting_name_max_length = 25;

inline constexpr int setting_description_min_length = 20;
inline constexpr int setting_description_max_length = 300;

using json = nlohmann::ordered_json;

class GfsMetaData : public ::testing::Test {
 protected:
  virtual void SetUp() {
    std::ifstream fs(file_path_);  // open file
    if (fs.fail()) {
      FAIL() << "Failed to open configuration";
    }
    cfg = json::parse(fs);
  }
  virtual void TearDown() {
  }

  void validateCategory(std::string category_name) {
    ASSERT_TRUE(cfg.contains(category_name)) << "Category " + category_name + " not found.";
    ASSERT_TRUE(cfg[category_name].is_structured()) << "Category " + category_name + " is not an object.";
    category = cfg[category_name];
  }

  void validateSettingMetadata(std::string setting_name) {
    ASSERT_TRUE(category.contains(setting_name)) << "Setting " + setting_name + " not found";
    setting = category[setting_name];
    ASSERT_TRUE(setting.is_structured()) << "Setting " + setting_name + " is not an object.";

    // Check for default value first so the type check can verify that the default value is of appropriate type.
    ASSERT_TRUE(setting.contains("default")) << "Setting " + setting_name + " does not contain a default value.";

    ASSERT_TRUE(setting.contains("type")) << "Setting " + setting_name + " does not contain a type.";
    ASSERT_TRUE(setting["type"].is_string()) << "Setting " + setting_name + " type is not a string.";
    std::string type = setting["type"].get<std::string>();

    if (type == "string") {
      validateStringTypeSetting(setting_name);
    } else if (type == "int") {
      validateIntTypeSetting(setting_name);
    } else if (type == "float") {
      validateFloatTypeSetting(setting_name);
    } else if (type == "bool") {
      validateBoolTypeSetting(setting_name);
    } else if (type == "enum") {
      validateEnumTypeSetting(setting_name);
    } else {
      FAIL() << "Setting " + setting_name + " has an invalid type.";
    }

    ASSERT_TRUE(setting.contains("name")) << "Setting " + setting_name + " does not contain a name.";
    ASSERT_TRUE(setting["name"].is_string()) << "Setting " + setting_name + " name is not a string.";
    int name_len = setting["name"].get<std::string>().length();
    ASSERT_GE(name_len, setting_name_min_length) << "Setting " + setting_name + " name is too short.";
    ASSERT_LE(name_len, setting_name_max_length) << "Setting " + setting_name + " name is too long.";

    ASSERT_TRUE(setting.contains("description")) << "Setting " + setting_name + " does not contain a description.";
    ASSERT_TRUE(setting["description"].is_string()) << "Setting " + setting_name + " description is not a string.";
    int description_len = setting["description"].get<std::string>().length();
    ASSERT_GE(description_len, setting_description_min_length) << "Setting " + setting_name + " description is too short.";
  }

  void validateStringTypeSetting(std::string setting_name) {
    ASSERT_TRUE(setting.contains("min")) << "Setting " + setting_name + " does not contain a min.";
    ASSERT_TRUE(setting["min"].is_number_integer()) << "Setting " + setting_name + " min is not an integer.";

    ASSERT_TRUE(setting.contains("max")) << "Setting " + setting_name + " does not contain a max.";
    ASSERT_TRUE(setting["max"].is_number_integer()) << "Setting " + setting_name + " max is not an integer.";

    ASSERT_GE(setting["min"].get<int>(), 0) << "Setting " + setting_name + " min is not greater than 0.";
    ASSERT_GE(setting["max"].get<int>(), 1) << "Setting " + setting_name + " max is not greater than 1.";
    ASSERT_GE(setting["max"].get<int>(), setting["min"].get<int>()) << "Setting " + setting_name + " max is not greater than min.";

    ASSERT_TRUE(setting.contains("pattern")) << "Setting " + setting_name + " does not contain a pattern.";
    ASSERT_TRUE(setting["pattern"].is_string()) << "Setting " + setting_name + " pattern is not a string.";
  }

  void validateIntTypeSetting(std::string setting_name) {
    ASSERT_TRUE(setting.contains("min")) << "Setting " + setting_name + " does not contain a min.";
    ASSERT_TRUE(setting["min"].is_number_integer()) << "Setting " + setting_name + " min is not an integer.";

    ASSERT_TRUE(setting.contains("max")) << "Setting " + setting_name + " does not contain a max.";
    ASSERT_TRUE(setting["max"].is_number_integer()) << "Setting " + setting_name + " max is not an integer.";

    ASSERT_TRUE(setting.contains("step")) << "Setting " + setting_name + " does not contain a step value.";
    ASSERT_TRUE(setting["step"].is_number_integer()) << "Setting " + setting_name + " step is not an integer.";

    ASSERT_GE(setting["max"].get<int>(), setting["min"].get<int>()) << "Setting " + setting_name + " max is not greater than min.";
    ASSERT_GE(setting["step"].get<int>(), 1) << "Setting " + setting_name + " step is less than 1";
  }

  void validateFloatTypeSetting(std::string setting_name) {
    ASSERT_TRUE(setting.contains("min")) << "Setting " + setting_name + " does not contain a min.";
    ASSERT_TRUE(setting["min"].is_number_float()) << "Setting " + setting_name + " min is not a float.";

    ASSERT_TRUE(setting.contains("max")) << "Setting " + setting_name + " does not contain a max.";
    ASSERT_TRUE(setting["max"].is_number_float()) << "Setting " + setting_name + " max is not a float.";

    ASSERT_TRUE(setting.contains("step")) << "Setting " + setting_name + " does not contain a step value.";
    ASSERT_TRUE(setting["step"].is_number_float()) << "Setting " + setting_name + " step is not a float.";

    ASSERT_GE(setting["max"].get<float>(), setting["min"].get<float>()) << "Setting " + setting_name + " max is not greater than min.";
    ASSERT_GT(setting["step"].get<float>(), 0.0) << "Setting " + setting_name + " step is less than 0.0";
  }

  void validateBoolTypeSetting(std::string setting_name) {
    ASSERT_TRUE(setting.contains("true")) << "Setting " + setting_name + " does not contain a true string.";
    ASSERT_TRUE(setting["true"].is_string()) << "Setting " + setting_name + " true is not a string.";

    ASSERT_TRUE(setting.contains("false")) << "Setting " + setting_name + " does not contain a false string.";
    ASSERT_TRUE(setting["false"].is_string()) << "Setting " + setting_name + " false is not a string.";
  }

  void validateEnumTypeSetting(std::string setting_name) {
    ASSERT_TRUE(setting.contains("options")) << "Setting " + setting_name + " does not contain an options array.";
    ASSERT_TRUE(setting["options"].is_array()) << "Setting " + setting_name + " options is not an array.";

    std::vector<std::string> options = setting["options"].get<std::vector<std::string>>();
    ASSERT_GE(options.size(), 1) << "Setting " + setting_name + " options array is empty.";
    for (std::string &option : options) {
      ASSERT_TRUE(option.length() > 0) << "Setting " + setting_name + " option is empty.";
    }
  }

  json cfg;
  json category;
  json setting;
  const std::string file_path_ = "gfs_configuration_metadata.json";

  std::vector<std::string> valid_settings_;
};

TEST_F(GfsMetaData, general_section) {
  std::array<std::string, 3> sections = {
      "project_name",
      "main_board",
      "starting_procedure"};

  validateCategory("general");

  for (std::string &section : sections) {
    validateSettingMetadata(section);
  }
}

TEST_F(GfsMetaData, debug_section) {
  std::array<std::string, 3> sections = {
      "print_errors",
      "console_enabled",
      "console_update_interval"};

  validateCategory("debug");
  for (std::string &section : sections) {
    validateSettingMetadata(section);
  }
}

TEST_F(GfsMetaData, server_section) {
  std::array<std::string, 1> sections = {
    "tcp_socket_port"
  };

  validateCategory("server");
  for (std::string &section : sections) {
    validateSettingMetadata(section);
  }
}

TEST_F(GfsMetaData, telemetry_section) {
  std::array<std::string, 2> sections = {
      "telemetry_enabled",
      "call_sign"};

  validateCategory("telemetry");

  for (std::string &section : sections) {
    validateSettingMetadata(section);
  }
}

TEST_F(GfsMetaData, telemetry_aprs_section) {
  std::array<std::string, 10> sections = {
      "telemetry_packets",
      "position_packets",
      "frequency",
      "ssid",
      "destination_address",
      "destination_ssid",
      "symbol_table",
      "symbol",
      "telemetry_destination",
      "comment"
      };

  validateCategory("telemetry_aprs");

  for (std::string &section : sections) {
    validateSettingMetadata(section);
  }
}

TEST_F(GfsMetaData, telemetry_sstv_section) {
  std::array<std::string, 4> sections = {
      "enabled",
      "frequency",
      "mode",
      "overlay_data"};

  validateCategory("telemetry_sstv");

  for (std::string &section : sections) {
    validateSettingMetadata(section);
  }
}

TEST_F(GfsMetaData, data_packets_section) {
  std::array<std::string, 5> sections = {
      "enabled",
      "frequency",
      "mode",
      "morse_callsign",
      "comment"};

  validateCategory("telemetry_data_packets");

  for (std::string &section : sections) {
    validateSettingMetadata(section);
  }
}

TEST_F(GfsMetaData, radios_section) {
  std::array<std::string, 0> sections = {};

  validateCategory("radios");

  for (std::string &section : sections) {
    validateSettingMetadata(section);
  }
}

TEST_F(GfsMetaData, flight_procedures) {
  std::array<std::string, 0> sections = {};

  validateCategory("flight_procedures");

  for (std::string &section : sections) {
    validateSettingMetadata(section);
  }
}

TEST_F(GfsMetaData, extensions_section) {
  std::array<std::string, 0> sections = {};

  validateCategory("extensions");

  for (std::string &section : sections) {
    validateSettingMetadata(section);
  }
}