/**
 * @file test.cpp
 * @author Joshua Jerred (github.com/joshua-jerred)
 * @brief Unit test of the ConfigModule
 * @version 0.1
 * @date 2022-09-29
 * 
 * @copyright Copyright (c) 2022
 */

#include <iostream>
#include <string>

#include "gtest/gtest.h"

#include "module-configuration.h"

ConfigData *confdata = NULL;

TEST(ConfigurationModule, LoadConfig) {
    ConfigModule config_module;
    std::string input_file = "./input-1.json";
    int loaded = config_module.load(input_file);
    ASSERT_EQ(loaded, 0) << "Failed to load input-1.json";

    confdata = new ConfigData{config_module.getAll()};
    
    std::vector<std::string> errors = config_module.getErrors();
    EXPECT_EQ(errors.size(), 0) << "Errors found in input-1.json";
}

TEST(ConfigurationModule, General) {
    EXPECT_EQ(confdata->general.project_name, "Test Project");
    EXPECT_EQ(confdata->general.main_board, ConfigData::MainboardType::kPi_zero_w);
    EXPECT_EQ(confdata->general.starting_loop, 1);
}

TEST(ConfigurationModule, Extensions) {
    std::vector <ExtensionMetadata> extensions = confdata->extensions.extensions_list;
    EXPECT_EQ(confdata->extensions.extensions_list.size(), 1);

    ExtensionMetadata ext1 = extensions[0];
    EXPECT_EQ(ext1.id, 1);
    EXPECT_EQ(ext1.name, "temp1");
    EXPECT_EQ(ext1.extension_type, "DS18B20");
    EXPECT_EQ(ext1.category, ExtensionMetadata::Category::kExternalSensor);
    EXPECT_EQ((int) ext1.interface, 5);
    EXPECT_EQ(ext1.update_interval, 10);
    EXPECT_EQ(ext1.critical, 0);
    EXPECT_EQ(ext1.address, "28-000000000000");
}

TEST(ConfigurationModule, Server) {
    EXPECT_EQ(confdata->server.web_server_enabled, 0);
}

TEST(ConfigurationModule, Telemetry) {
    EXPECT_EQ(confdata->telemetry.telemetry_enabled, 0);
}

TEST(ConfigurationModule, DataTypes) {
    std::vector <ConfigData::DataTypes::ExtensionDataType> data_types = confdata->data_types.types;
    EXPECT_EQ(data_types.size(), 1);

    ConfigData::DataTypes::ExtensionDataType data_type1 = data_types[0];
    EXPECT_EQ(data_type1.name, "TEMP_F");
    EXPECT_EQ(data_type1.unit, "F");
}

TEST(ConfigurationModule, FlightLoops) {
    FlightLoop loop1 = confdata->flight_loops.testing;
    EXPECT_EQ(loop1.type, FlightLoop::LoopType::kTesting);
    EXPECT_EQ(loop1.intervals.data_log, 10);
    EXPECT_EQ(loop1.intervals.server_update, 2);
}