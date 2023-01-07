/**
 * @file test.cpp
 * @author Joshua Jerred (https://joshuajer.red/)
 * @brief Configuration module tests
 * 
 * @version 0.0.9
 * @date 2022-09-29
 * @copyright Copyright (c) 2022
 * @todo This must be redone to use the new configuration module
 */

#include <string>

#include "gtest/gtest.h"

#include "data-stream.h"

#include "modules.h"

class Configuration_Module_1 : public ::testing::Test {
protected:
    virtual void SetUp() {
        p_data_stream_ = new DataStream();
        config_module_ = new modules::ConfigModule(p_data_stream_);
        std::string input_file = "./input-1.json";
        int loaded = config_module_->load(input_file);
     }
    virtual void TearDown() {
        delete config_module_;
        delete p_data_stream_;
    }
    DataStream *p_data_stream_ = nullptr;
    modules::ConfigModule *config_module_ = nullptr;
    ConfigData config_data_ = {};
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

TEST_F(Configuration_Module_1, Debugging) {
    ConfigData::Debugging debugging = config_module_->getAll().debug;

    EXPECT_EQ(1, debugging.console_enabled) << "Console debugging is incorrect";
    EXPECT_EQ(1002, debugging.console_update_interval) 
        << "Console update interval is incorrect";

    EXPECT_EQ(1, debugging.web_server_enabled) 
        << "Web server debugging is incorrect";
    EXPECT_EQ(1005, debugging.web_server_update_interval)
        << "Web server update interval is incorrect";
}

TEST_F(Configuration_Module_1, TelemetrySection) {
    ConfigData::Telemetry telemetry = config_module_->getAll().telemetry;

    EXPECT_EQ(1 , telemetry.telemetry_enabled)
        << "Telemetry enabled is incorrect";
    EXPECT_EQ("TESTCALL", telemetry.call_sign)
        << "Callsign is incorrect";

    EXPECT_EQ(1, telemetry.aprs_enabled)
        << "APRS enabled is incorrect";
    EXPECT_EQ("144.390", telemetry.aprs_freq)
        << "APRS frequency is incorrect";
    EXPECT_EQ(11, telemetry.aprs_ssid)
        << "APRS SSID is incorrect";
    EXPECT_EQ("O", telemetry.aprs_symbol)
        << "APRS symbol is incorrect";
    EXPECT_EQ("Test Memo", telemetry.aprs_memo)
        << "APRS memo is incorrect";
    
    EXPECT_EQ(1, telemetry.sstv_enabled)
        << "SSTV enabled is incorrect";
    EXPECT_EQ("100.000", telemetry.sstv_freq)
        << "SSTV frequency is incorrect";

    EXPECT_EQ(1, telemetry.afsk_enabled)
        << "AFSK enabled is incorrect";
    EXPECT_EQ("105.000", telemetry.afsk_freq)
        << "AFSK frequency is incorrect";

    EXPECT_EQ(1, telemetry.psk_enabled)
        << "PSK enabled is incorrect";
    EXPECT_EQ("109.000", telemetry.psk_freq)
        << "PSK frequency is incorrect";
    EXPECT_EQ("bpsk125", telemetry.psk_mode)
        << "PSK mode is incorrect";
}

TEST_F(Configuration_Module_1, FlightProcsSection) {
    ConfigData::Procs procs = config_module_->getAll().flight_procs;
    FlightProcedure test_proc = procs.testing;

    EXPECT_EQ(1, test_proc.enabled)
        << "Testing procedure enabled is incorrect";

    EXPECT_EQ(test_proc.type, FlightProcedure::ProcType::TESTING)
        << "Testing procedure type is incorrect";

    FlightProcedure::Intervals intervals = test_proc.intervals;

    EXPECT_EQ(2, intervals.data_log)
        << "Testing procedure data log interval is incorrect";
    EXPECT_EQ(5, intervals.data_packet)
        << "Testing procedure data packet interval is incorrect";
    EXPECT_EQ(10, intervals.sstv)
        << "Testing procedure sstv interval is incorrect";
    EXPECT_EQ(5, intervals.aprs)
        << "Testing procedure aprs interval is incorrect";
    EXPECT_EQ(50, intervals.picture)
        << "Testing procedure picture interval is incorrect";
    EXPECT_EQ(10, intervals.health_check)
        << "Testing procedure health check interval is incorrect";
}