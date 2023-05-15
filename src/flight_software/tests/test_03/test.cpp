/**
 * @file test.cpp
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief Unit test for the configuration file handler.
 * @date 2023-05-13
 * @copyright Copyright (c) 2023
 */

#include <string>

#include "configuration.h"
#include "configuration_internal.h"
#include "gtest/gtest.h"

class ConfigurationClass : public ::testing::Test {
 protected:
  virtual void SetUp() {
  }
  virtual void TearDown() {
  }

  cfg::Configuration config;
};

TEST_F(ConfigurationClass, General_SetsDefaults) {
  cfg::General general = config.getGeneral();

  ASSERT_EQ(general.project_name, cfg::general::defaults::project_name);
  ASSERT_EQ(general.main_board_type, cfg::general::defaults::main_board);
  ASSERT_EQ(general.starting_procedure, cfg::general::defaults::starting_procedure);
}

TEST_F(ConfigurationClass, General_Setter_ProjectName_Valid) {
  const std::vector<std::string> valid_names = {
      "Project Name",
      "Another Project Name",
      "valid-project-name"};

  cfg::General default_general = config.getGeneral();
  for (const std::string &project_name : valid_names) {
    std::string error = "";

    cfg::General new_general = default_general;
    new_general.project_name = project_name;
    bool set = config.setGeneral(new_general, error);
    cfg::General current_general = config.getGeneral();

    ASSERT_TRUE(set) << "Setter reported that it did not set the value!";
    ASSERT_EQ(project_name, current_general.project_name);
    ASSERT_EQ(error, "");
  }
}

TEST_F(ConfigurationClass, General_Setter_ProjectName_Invalid) {
  const std::vector<std::string> invalid_names = {
      " illegal name",
      "bad : character",
      ""};

  cfg::General default_general = config.getGeneral();
  for (const std::string &project_name : invalid_names) {
    std::string error;

    cfg::General new_general = default_general;
    new_general.project_name = project_name;
    bool set = config.setGeneral(new_general, error);
    cfg::General current_general = config.getGeneral();

    ASSERT_FALSE(set) << "Setter reported that it did set the illegal value: " << project_name;
    ASSERT_NE(project_name, current_general.project_name);
    ASSERT_EQ(default_general.project_name, current_general.project_name);
    ASSERT_NE(error, "");
  }
}

TEST_F(ConfigurationClass, General_Setter_SetsValidFields) {
  std::string error;

  cfg::General initial_config = config.getGeneral();
  cfg::General new_general;
  new_general.project_name = "  Invalid Name";
  new_general.main_board_type = cfg::General::MainBoard::PI_4;
  new_general.starting_procedure = cfg::Procedure::Type::TESTING;

  config.setGeneral(new_general, error);
  cfg::General updated_general = config.getGeneral();

  // Verify that the new general contains different values from the current ones
  ASSERT_NE(initial_config.project_name, new_general.project_name);
  ASSERT_NE(initial_config.main_board_type, new_general.main_board_type);
  ASSERT_NE(initial_config.starting_procedure, new_general.starting_procedure);

  // Verify that all valid field have been updated, but not the invalid ones.
  ASSERT_NE(updated_general.project_name, new_general.project_name);
  ASSERT_EQ(updated_general.main_board_type, new_general.main_board_type);
  ASSERT_EQ(updated_general.starting_procedure, new_general.starting_procedure);
}

TEST_F(ConfigurationClass, SetsDefaults_Debug) {
  cfg::Debug debug = config.getDebug();

  ASSERT_EQ(debug.console_enabled, cfg::debug::defaults::console_enabled);
  ASSERT_EQ(debug.console_update_interval, cfg::debug::defaults::console_update_interval);
  ASSERT_EQ(debug.print_errors, cfg::debug::defaults::print_errors);
}

TEST_F(ConfigurationClass, SetsDefaults_Server) {
  cfg::Server server = config.getServer();

  ASSERT_EQ(server.tcp_socket_port, cfg::server::defaults::tcp_socket_port);
}

TEST_F(ConfigurationClass, SetsDefaults_Telemetry) {
  cfg::Telemetry telem = config.getTelemetry();

  ASSERT_EQ(telem.telemetry_enabled, cfg::telemetry::defaults::telemetry_enabled);
  ASSERT_EQ(telem.call_sign, cfg::telemetry::defaults::call_sign);
}

TEST_F(ConfigurationClass, SetsDefaults_Aprs) {
  cfg::Aprs aprs = config.getAprs();

  ASSERT_EQ(aprs.telemetry_packets, cfg::aprs::defaults::telemetry_packets);
  ASSERT_EQ(aprs.position_packets, cfg::aprs::defaults::position_packets);
  ASSERT_EQ(aprs.frequency.getFrequency(), cfg::aprs::defaults::frequency);
  ASSERT_EQ(aprs.ssid, cfg::aprs::defaults::ssid);
  ASSERT_EQ(aprs.destination_address, cfg::aprs::defaults::destination_address);
  ASSERT_EQ(aprs.symbol_table, cfg::aprs::defaults::symbol_table);
  ASSERT_EQ(aprs.symbol, cfg::aprs::defaults::symbol);
  ASSERT_EQ(aprs.comment, cfg::aprs::defaults::comment);
}

TEST_F(ConfigurationClass, SetsDefaults_Sstv) {
  cfg::Sstv sstv = config.getSstv();

  ASSERT_EQ(sstv.enabled, cfg::sstv::defaults::enabled);
  ASSERT_EQ(sstv.frequency.getFrequency(), cfg::sstv::defaults::frequency);
  ASSERT_EQ(sstv.mode, cfg::sstv::defaults::mode);
  ASSERT_EQ(sstv.overlay_data, cfg::sstv::defaults::overlay_data);
}

TEST_F(ConfigurationClass, SetsDefaults_DataPackets) {
  cfg::DataPackets data_packets = config.getDataPackets();

  ASSERT_EQ(data_packets.enabled, cfg::data_packets::defaults::enabled);
  ASSERT_EQ(data_packets.frequency.getFrequency(), cfg::data_packets::defaults::frequency);
  ASSERT_EQ(data_packets.mode, cfg::data_packets::defaults::mode);
  ASSERT_EQ(data_packets.morse_call_sign, cfg::data_packets::defaults::morse_call_sign);
  ASSERT_EQ(data_packets.comment, cfg::data_packets::defaults::comment);
}
