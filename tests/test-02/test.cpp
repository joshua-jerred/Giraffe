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

#include "utility-data-stream.h"

#include "module-configuration.h"

class Configuration_Module_1 : public ::testing::Test {
protected:
    virtual void SetUp() {
        p_data_stream_ = new DataStream();
        config_module_ = new ConfigModule(p_data_stream_);
        std::string input_file = "./input-1.json";
        int loaded = config_module_->load(input_file);
     }
    virtual void TearDown() {
        delete config_module_;
        delete p_data_stream_;
    }
    DataStream *p_data_stream_;
    ConfigModule *config_module_;
    ConfigData config_data_;
};

TEST_F(Configuration_Module_1, OpenConfigFile) {
    std::string input_file = "./input-1.json";
    int loaded = config_module_->load(input_file);

    ASSERT_NE(-1, loaded) << "Failed to open file: " << input_file;
}

TEST_F(Configuration_Module_1, CheckForErrors) {
    int errors = config_module_->getNumberOfErrors();

    EXPECT_EQ(0, errors) << "Errors found in file";
}

TEST_F(Configuration_Module_1, GeneralSection) {
    std::string input_file = "./input-1.json";
    config_module_->load(input_file);
    config_data_ = config_module_->getAll();

    EXPECT_EQ("projname", config_data_.general.project_name)
        << "Project name is incorrect";
    EXPECT_EQ(ConfigData::MainboardType::PI_ZERO_W, config_data_.general.main_board)
        << "Mainboard type is incorrect";
    EXPECT_EQ(FlightProcedure::ProcType::TESTING, config_data_.general.starting_proc)
        << "Starting procedure is incorrect";
}

TEST_F(Configuration_Module_1, ExtensionsSection) {
    ConfigData::Extensions extensions = config_module_->getAll().extensions;
    
    EXPECT_EQ(4, extensions.extensions_list.size())
        << "Incorrect number of extensions";

    ExtensionMetadata temp_ext = extensions.extensions_list[0];
    EXPECT_EQ(1, temp_ext.id) << "Extension 1 ID is incorrect";
    EXPECT_EQ("ext1test", temp_ext.name) << "Extension 1 name is incorrect";
    EXPECT_EQ("TEST_EXT", temp_ext.extension_type) << "Extension 1 type is incorrect";
    EXPECT_EQ(ExtensionMetadata::Category::EXTERNAL_SENSOR, temp_ext.category)
        << "Extension 1 category is incorrect";
    EXPECT_EQ(ExtensionMetadata::Interface::ONEWIRE, temp_ext.interface)
        << "Extension 1 interface is incorrect";
    EXPECT_EQ(1000, temp_ext.update_interval) << "Extension 1 update interval is incorrect";
    EXPECT_EQ(0, temp_ext.critical) << "Extension 1 flight criticality is incorrect";
    EXPECT_EQ("28-000010000000", temp_ext.extra_args.one_wire_id)
        << "Extension 1 one wire ID is incorrect";

    temp_ext = extensions.extensions_list[1];
    EXPECT_EQ(2, temp_ext.id) << "Extension 2 ID is incorrect";
    EXPECT_EQ("ext2test", temp_ext.name) << "Extension 2 name is incorrect";
    EXPECT_EQ("BMP180_SIM", temp_ext.extension_type) << "Extension 2 type is incorrect";
    EXPECT_EQ(ExtensionMetadata::Category::EXTERNAL_SENSOR, temp_ext.category)
        << "Extension 2 category is incorrect";
    EXPECT_EQ(ExtensionMetadata::Interface::I2C, temp_ext.interface)
        << "Extension 2 interface is incorrect";
    EXPECT_EQ(2000, temp_ext.update_interval) << "Extension 2 update interval is incorrect";
    EXPECT_EQ(1, temp_ext.critical) << "Extension 2 flight criticality is incorrect";
    EXPECT_EQ(1, temp_ext.extra_args.I2C_bus)
        << "Extension 2 i2c bus is incorrect";
    EXPECT_EQ("0x22", temp_ext.extra_args.I2C_device_address)
        << "Extension 2 i2c address is incorrect";

    temp_ext = extensions.extensions_list[2];
    EXPECT_EQ(3, temp_ext.id) << "Extension 3 ID is incorrect";
    EXPECT_EQ("ext3test", temp_ext.name) << "Extension 3 name is incorrect";
    EXPECT_EQ("DS18B20_SIM", temp_ext.extension_type) << "Extension 3 type is incorrect";
    EXPECT_EQ(ExtensionMetadata::Category::EXTERNAL_SENSOR, temp_ext.category)
        << "Extension 3 category is incorrect";
    EXPECT_EQ(ExtensionMetadata::Interface::ONEWIRE, temp_ext.interface)
        << "Extension 3 interface is incorrect";
    EXPECT_EQ(1500, temp_ext.update_interval) << "Extension 3 update interval is incorrect";
    EXPECT_EQ(0, temp_ext.critical) << "Extension 3 flight criticality is incorrect";
    EXPECT_EQ("28-000000000009", temp_ext.extra_args.one_wire_id)
        << "Extension 1 one wire ID is incorrect";

    temp_ext = extensions.extensions_list[3];
    EXPECT_EQ(4, temp_ext.id) << "Extension 4 ID is incorrect";
    EXPECT_EQ("ext4test", temp_ext.name) << "Extension 4 name is incorrect";
    EXPECT_EQ("SAMM8Q_SIM", temp_ext.extension_type) << "Extension 4 type is incorrect";
    EXPECT_EQ(ExtensionMetadata::Category::GPS, temp_ext.category)
        << "Extension 4 category is incorrect";
    EXPECT_EQ(ExtensionMetadata::Interface::I2C, temp_ext.interface)
        << "Extension 4 interface is incorrect";
    EXPECT_EQ(2500, temp_ext.update_interval) << "Extension 4 update interval is incorrect";
    EXPECT_EQ(1, temp_ext.critical) << "Extension 4 flight criticality is incorrect";
    EXPECT_EQ(1, temp_ext.extra_args.I2C_bus)
        << "Extension 4 i2c bus is incorrect";
    EXPECT_EQ("0x25", temp_ext.extra_args.I2C_device_address)
        << "Extension 4 i2c address is incorrect";
}