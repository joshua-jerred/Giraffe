/**
 * @file test.cpp
 * @author Joshua Jerred (github.com/joshua-jerred)
 * @brief Configuration module tests
 * 
 * @version 0.0.9
 * @date 2022-09-29
 * @copyright Copyright (c) 2022
 * @todo This must be redone to use the new configuration module
 */

#include <string>

#include "gtest/gtest.h"

#include "module-configuration.h"

class Configuration_Module_1 : public ::testing::Test {
protected:
    virtual void SetUp() { 
        config_module_ = new ConfigModule();
        config_data_ = new ConfigData();
     }
    ConfigModule *config_module_;
    ConfigData *config_data_;
};

TEST_F(Configuration_Module_1, OpenConfigFile) {
    std::string input_file = "./input-1.json";
    int loaded = config_module_->load(input_file);
    ASSERT_NE(loaded, -1) << "Failed to open file: " << input_file;
}