/**
 * @file test.cpp
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief Unit Tests for the common configuration scheme. This will help to
 * verify the standardization of setting metadata.
 * @date 2023-05-09
 * @copyright Copyright (c) 2023
 */

#include <iostream>
#include <regex>
#include <string>

#include "gtest/gtest.h"
#include "yaml-cpp/yaml.h"

class TEST_Configuration_Definition : public ::testing::Test {
 protected:
  virtual void SetUp() {
  }
  virtual void TearDown() {
  }

  /**
   * @brief Verify that a key is contained with a node.
   * @param node
   * @param key
   */
  void C_EXPECT_DEFINED(YAML::Node &node, std::string key) {
    EXPECT_TRUE(node[key].IsDefined()) << "Key " + key + " is not defined.";
  }

  /**
   * @brief Verify that a key is not defined within a node.
   * @param node
   * @param key
   */
  void C_EXPECT_NOT_DEFINED(YAML::Node &node, std::string key) {
    EXPECT_FALSE(node[key].IsDefined()) << "Key " + key + " is not defined.";
  }

  /**
   * @brief Verify that every string in a vector matches the pattern.
   * @param pattern 
   * @param inputs 
   */
  void C_VERIFY_VALID_STRINGS(std::string pattern, std::vector<std::string> &inputs) {
    for (std::string input : inputs) {
      EXPECT_TRUE(std::regex_match(input, std::regex(pattern))) << "Input " + input + " does not match pattern " + pattern + " - this is a valid input.";
    }
  }

  /**
   * @brief Verify that every string in a vector does *not* match the pattern.
   * @param pattern 
   * @param inputs 
   */
  void C_VERIFY_INVALID_STRINGS(std::string pattern, std::vector<std::string> &inputs) {
    for (std::string input : inputs) {
      EXPECT_FALSE(std::regex_match(input, std::regex(pattern))) << "Input " + input + " matches the pattern " + pattern + " - this is not a valid input.";
    }
  }

  /**
   * @brief Validate a regex pattern against a vector of valid inputs and a vector of invalid inputs.
   * @param pattern 
   * @param valid_inputs 
   * @param invalid_inputs 
   */
  void C_VERIFY_STRINGS_MATCH(std::string pattern, std::vector<std::string> &valid_inputs, std::vector<std::string> &invalid_inputs) {
    C_VERIFY_VALID_STRINGS(pattern, valid_inputs);
    C_VERIFY_INVALID_STRINGS(pattern, invalid_inputs);
  }

  /**
   * @brief Verify that the minimum fields for a given setting exist along
   * with type verification.
   * @param node The setting node
   */
  void C_CHECK_MINIMUM_SETTING_FIELDS(YAML::Node &setting) {
    ASSERT_FALSE(setting.IsScalar());

    C_EXPECT_DEFINED(setting, "name");
    ASSERT_TRUE(setting["name"].IsScalar());

    C_EXPECT_DEFINED(setting, "description");
    ASSERT_TRUE(setting["description"].IsScalar());

    C_EXPECT_DEFINED(setting, "type");
    ASSERT_TRUE(setting["type"].IsScalar());

    std::string type = setting["type"].as<std::string>();
    if (type == "string") {
      C_CHECK_STRING_TYPE_SETTING(setting);
    } else if (type == "int") {
      C_CHECK_INT_TYPE_SETTING(setting);
    } else if (type == "float") {
      C_CHECK_FLOAT_TYPE_SETTING(setting);
    } else if (type == "bool") {
      C_CHECK_BOOLEAN_TYPE_SETTING(setting);
    } else if (type == "enum") {
      C_CHECK_ENUM_TYPE_SETTING(setting);
    } else {
      FAIL() << "Invalid type: " + type;
    }

    C_EXPECT_DEFINED(setting, "default");
    ASSERT_TRUE(setting["default"].IsScalar());
  }

/**
 * @brief Verify that a string valued setting has it's needed additional parameters. (min/max length or regex pattern).
 * This will also verify that the default value is a valid input.
 * @param setting 
 */
  void C_CHECK_STRING_TYPE_SETTING(YAML::Node &setting) {
    bool has_max = setting["max"].IsDefined();
    bool has_min = setting["min"].IsDefined();
    bool has_pattern = setting["pattern"].IsDefined();

    if (!has_max && !has_min && !has_pattern) {
      FAIL() << "String type settings must have either a max and min or a pattern.";
    } else if ((has_max && !has_min) || (!has_max && has_min)) {
      FAIL() << "If a string type setting can not have only a max or a min, it must have both.";
    }

    // Verify that the default value is a valid setting.
    std::string default_value = setting["default"].as<std::string>();
    if (has_max && has_min) {
      int min = setting["min"].as<int>();
      if (min < 0) {
        FAIL() << "The minimum value for a string needs to be set equal to or greater than 0.";
      }
      int max = setting["max"].as<int>();
      if (default_value.length() < (unsigned int)min || default_value.length() > (unsigned int)max) {
        FAIL() << "Default value for string type setting must be between min and max length.";
      }
    } 
    
    if (has_pattern) {
      std::string pattern = setting["pattern"].as<std::string>();
      if (!std::regex_match(default_value, std::regex(pattern))) {
        FAIL() << "Default value for string type setting must match the pattern.";
      }
    }
  }

  void C_CHECK_INT_TYPE_SETTING(YAML::Node &setting) {
    (void)setting;
  }

  void C_CHECK_FLOAT_TYPE_SETTING(YAML::Node &setting) {
    (void)setting;
  }

  void C_CHECK_BOOLEAN_TYPE_SETTING(YAML::Node &setting) {
    (void)setting;
  }

  void C_CHECK_ENUM_TYPE_SETTING(YAML::Node &setting) {
    (void)setting;
  }

  YAML::Node doc = YAML::LoadFile("gfs_configuration_metadata.yaml");
};

TEST_F(TEST_Configuration_Definition, CategoriesExist) {
  C_EXPECT_DEFINED(doc, "general");
}

TEST_F(TEST_Configuration_Definition, General_SettingsExist) {
  YAML::Node general = doc["general"];
  C_EXPECT_DEFINED(general, "project_name");
}

TEST_F(TEST_Configuration_Definition, General_ProjectName) {
  YAML::Node project_name = doc["general"]["project_name"];
  C_CHECK_MINIMUM_SETTING_FIELDS(project_name);

  std::string pattern = project_name["pattern"].as<std::string>();

  // Between 1 and 20 characters long, can't start or end with a space.
  // Valid Character <space>, a-z, A-Z, `-`, `_`, 0-9.
  std::vector<std::string> valid_inputs = {
      "a",
      "b",
      "name with spaces",
      "name-with-dashes",
      "a               b",
      "12345678901234567890",
      "MiXeD-Cases_ _123"};

  std::vector<std::string> invalid_inputs = {
      " a",
      " starts with space",
      "ends with space ",
      "",
      "123456789012345678901"};

  C_VERIFY_STRINGS_MATCH(pattern, valid_inputs, invalid_inputs);
}