/**
 * @file test.cpp
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief Unit test for the configuration class.
 * @date 2023-05-13
 * @copyright Copyright (c) 2023
 */

#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::ordered_json;

#include "configuration.h"
#include "configuration_internal.h"
#include "gtest/gtest.h"
#include "streams.h"

const std::string kDefaultConfigPath = "config.json";

const std::string meta_data_path = "./gfs_configuration_metadata.json";
json gMetaData;

class Configuration_Json : public ::testing::Test {
 protected:
  static void SetUpTestSuite() {
    std::ifstream fs(meta_data_path);
    if (fs.fail()) {
      FAIL() << "Failed to open meta data json file!";
    }
    gMetaData = json::parse(fs);
  }

  virtual void SetUp() {
    streams_ = new data::Streams();
    config_ = new cfg::Configuration(*streams_);
  }
  virtual void TearDown() {
    delete config_;
    delete streams_;
  }

  data::Streams *streams_ = nullptr;
  cfg::Configuration *config_ = nullptr;
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

TEST_F(Configuration_Json, sectionToJson) {
  ASSERT_TRUE(HAS_SETTINGS(gMetaData["general"],
                           cfg::json::generalToJson(config_->getGeneral()), 3));
  ASSERT_TRUE(HAS_SETTINGS(gMetaData["debug"],
                           cfg::json::debugToJson(config_->getDebug()), 3));
  ASSERT_TRUE(HAS_SETTINGS(gMetaData["server"],
                           cfg::json::serverToJson(config_->getServer()), 1));
  ASSERT_TRUE(HAS_SETTINGS(gMetaData["telemetry"],
                           cfg::json::telemetryToJson(config_->getTelemetry()),
                           2));
  ASSERT_TRUE(HAS_SETTINGS(gMetaData["telemetry_aprs"],
                           cfg::json::aprsToJson(config_->getAprs()), 9));
  ASSERT_TRUE(HAS_SETTINGS(gMetaData["telemetry_sstv"],
                           cfg::json::sstvToJson(config_->getSstv()), 4));
  ASSERT_TRUE(
      HAS_SETTINGS(gMetaData["telemetry_data_packets"],
                   cfg::json::dataPacketsToJson(config_->getDataPackets()), 5));
}

class Configuration_Class : public ::testing::Test {
 protected:
  static void SetUpTestSuite() {
    std::ifstream fs(meta_data_path);
    if (fs.fail()) {
      FAIL() << "Failed to open meta data json file!";
    }
    gMetaData = json::parse(fs);
  }

  virtual void SetUp() {
    if (std::filesystem::exists(kDefaultConfigPath)) {
      std::filesystem::remove(kDefaultConfigPath);
    }

    streams_ = new data::Streams();
    config = new cfg::Configuration(*streams_);
  }
  virtual void TearDown() {
    delete streams_;
    delete config;
  }

  template <typename T>
  T getDefault(std::string section, std::string setting_name) {
    return gMetaData[section][setting_name]["default"].get<T>();
  }

  template <typename T>
  void verifyDefaultSetting(json &config, std::string section,
                            std::string setting_name, T setting) {
    EXPECT_EQ(setting, getDefault<T>(section, setting_name))
        << "Setting value does not match default in metadata!"
        << " [" + section + "][" + setting_name + "]";

    EXPECT_EQ(setting, config[section][setting_name].get<T>())
        << "Setting value in the structure does not match the saved JSON."
        << " [" << section << "][" << setting_name << "]";
  }

  data::Streams *streams_ = nullptr;
  cfg::Configuration *config = nullptr;
};

TEST_F(Configuration_Class, GeneratesDefaultConfiguration) {
  ASSERT_TRUE(std::filesystem::exists(kDefaultConfigPath))
      << "New config file did not save!";

  std::ifstream fs(kDefaultConfigPath);
  json saved_config = json::parse(fs);

  cfg::General general = config->getGeneral();

  verifyDefaultSetting<std::string>(saved_config, "general", "project_name",
                                    general.project_name);

  EXPECT_EQ(general.main_board_type, cfg::General::MainBoard::OTHER)
      << "May need to update default.";

  EXPECT_EQ(general.starting_procedure, cfg::Procedure::Type::FAILSAFE)
      << "May need to update default";

  cfg::Debug debug = config->getDebug();
  verifyDefaultSetting<bool>(saved_config, "debug", "console_enabled",
                             debug.console_enabled);
  verifyDefaultSetting<bool>(saved_config, "debug", "print_errors",
                             debug.print_errors);
  verifyDefaultSetting<int>(saved_config, "debug", "console_update_interval",
                            debug.console_update_interval);

  cfg::Server server = config->getServer();
  verifyDefaultSetting<int>(saved_config, "server", "tcp_socket_port",
                            server.tcp_socket_port);

  cfg::Telemetry telem = config->getTelemetry();
  verifyDefaultSetting<bool>(saved_config, "telemetry", "telemetry_enabled",
                             telem.telemetry_enabled);
  verifyDefaultSetting<std::string>(saved_config, "telemetry", "call_sign",
                                    telem.call_sign);

  cfg::Aprs aprs = config->getAprs();
  verifyDefaultSetting<bool>(saved_config, "telemetry_aprs",
                             "telemetry_packets", aprs.telemetry_packets);

  // ASSERT_EQ(aprs.telemetry_packets, cfg::aprs::defaults::telemetry_packets);
  // ASSERT_EQ(aprs.position_packets, cfg::aprs::defaults::position_packets);
  // ASSERT_EQ(aprs.frequency.getFrequency(), cfg::aprs::defaults::frequency);
  // ASSERT_EQ(aprs.ssid, cfg::aprs::defaults::ssid);
  // ASSERT_EQ(aprs.destination_address,
  // cfg::aprs::defaults::destination_address); ASSERT_EQ(aprs.symbol_table,
  // cfg::aprs::defaults::symbol_table); ASSERT_EQ(aprs.symbol,
  // cfg::aprs::defaults::symbol); ASSERT_EQ(aprs.comment,
  // cfg::aprs::defaults::comment);

  // cfg::Sstv sstv = config.getSstv();
  // ASSERT_EQ(sstv.enabled, cfg::sstv::defaults::enabled);
  // ASSERT_EQ(sstv.frequency.getFrequency(), cfg::sstv::defaults::frequency);
  // ASSERT_EQ(sstv.mode, cfg::sstv::defaults::mode);
  // ASSERT_EQ(sstv.overlay_data, cfg::sstv::defaults::overlay_data);

  // cfg::DataPackets data_packets = config.getDataPackets();

  // ASSERT_EQ(data_packets.enabled, cfg::data_packets::defaults::enabled);
  // ASSERT_EQ(data_packets.frequency.getFrequency(),
  //           cfg::data_packets::defaults::frequency);
  // ASSERT_EQ(data_packets.mode, cfg::data_packets::defaults::mode);
  // ASSERT_EQ(data_packets.morse_call_sign,
  //           cfg::data_packets::defaults::morse_call_sign);
  // ASSERT_EQ(data_packets.comment, cfg::data_packets::defaults::comment);
}

// TEST_F(Configuration_Class, General_Setter_ProjectName_Valid) {
//   const std::vector<std::string> valid_names = {
//       "Project Name", "Another Project Name", "valid-project-name"};

//   cfg::General default_general = config.getGeneral();
//   for (const std::string &project_name : valid_names) {
//     cfg::General new_general = default_general;
//     new_general.project_name = project_name;
//     bool set = config.setGeneral(new_general);
//     cfg::General current_general = config.getGeneral();

//     ASSERT_TRUE(set) << "Setter reported that it did not set the value!";
//     ASSERT_EQ(project_name, current_general.project_name);
//     ASSERT_EQ(es_.getTotalPackets(), 0);
//   }
// }

// TEST_F(Configuration_Class, General_Setter_ProjectName_Invalid) {
//   const std::vector<std::string> invalid_names = {" illegal name",
//                                                   "bad : character", ""};

//   cfg::General default_general = config.getGeneral();
//   for (const std::string &project_name : invalid_names) {
//     cfg::General new_general = default_general;
//     new_general.project_name = project_name;
//     bool set = config.setGeneral(new_general);
//     cfg::General current_general = config.getGeneral();

//     ASSERT_FALSE(set) << "Setter reported that it did set the illegal value:
//     "
//                       << project_name;
//     ASSERT_NE(project_name, current_general.project_name);
//     ASSERT_EQ(default_general.project_name, current_general.project_name);
//     ASSERT_EQ(es_.getTotalPackets(), 1);
//     es_.reset();
//   }
// }

// TEST_F(Configuration_Class, General_Setter_SetsValidFields) {
//   cfg::General initial_config = config.getGeneral();
//   cfg::General new_general;
//   new_general.project_name = "  Invalid Name";
//   new_general.main_board_type = cfg::General::MainBoard::PI_4;
//   new_general.starting_procedure = cfg::Procedure::Type::TESTING;

//   config.setGeneral(new_general);
//   cfg::General updated_general = config.getGeneral();

//   // Verify that the new general contains different values from the current
//   ones ASSERT_NE(initial_config.project_name, new_general.project_name);
//   ASSERT_NE(initial_config.main_board_type, new_general.main_board_type);
//   ASSERT_NE(initial_config.starting_procedure,
//   new_general.starting_procedure);

//   // Verify that all valid field have been updated, but not the invalid ones.
//   ASSERT_NE(updated_general.project_name, new_general.project_name);
//   ASSERT_EQ(updated_general.main_board_type, new_general.main_board_type);
//   ASSERT_EQ(updated_general.starting_procedure,
//   new_general.starting_procedure);

//   ASSERT_EQ(es_.getTotalPackets(), 1);
// }