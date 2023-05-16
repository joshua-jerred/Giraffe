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

#include "streams.h"
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
    es_.reset();
  }
  virtual void TearDown() {
  }

  data::ErrorStream es_;

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
  cfg::Configuration config = cfg::Configuration(es_);
  const std::string path = "default_config.json";
  cfg::file::saveConfiguration(es_, config, path);

  ASSERT_TRUE(std::filesystem::exists(path))
      << "File does not exist when it should.";

  // Load in the same file
  std::ifstream file(path);
  json saved_cfg = json::parse(file);

  verifyFieldsAndTypes(saved_cfg);
}

TEST_F(Configuration_File, savesChangedConfiguration) {
  cfg::Configuration config = cfg::Configuration(es_);
  const std::string path = "changed_config.json";

  cfg::General new_general = config.getGeneral();
  new_general.project_name = "New Project Name";
  new_general.main_board_type = cfg::General::MainBoard::PI_4;
  new_general.starting_procedure = cfg::Procedure::Type::ASCENT;
  ASSERT_TRUE(config.setGeneral(new_general));

  cfg::Debug new_debug = config.getDebug();
  new_debug.console_enabled = true;
  new_debug.print_errors = true;
  new_debug.console_update_interval = 1100;
  ASSERT_TRUE(config.setDebug(new_debug));

  cfg::Server new_server = config.getServer();
  new_server.tcp_socket_port = 7800;
  ASSERT_TRUE(config.setServer(new_server));

  cfg::Telemetry new_telemetry = config.getTelemetry();
  new_telemetry.telemetry_enabled = true;
  new_telemetry.call_sign = "CA2LL";
  ASSERT_TRUE(config.setTelemetry(new_telemetry));

  cfg::Aprs new_aprs = config.getAprs();
  new_aprs.telemetry_packets = true;
  new_aprs.position_packets = true;
  new_aprs.frequency.setFrequency("000.0000");
  new_aprs.ssid = 2;
  new_aprs.destination_address = "TEST";
  new_aprs.destination_ssid = 3;
  new_aprs.symbol_table = cfg::Aprs::SymbolTable::ALTERNATE;
  new_aprs.symbol = 'a';
  new_aprs.comment = "Hello";
  ASSERT_TRUE(config.setAprs(new_aprs));

  cfg::Sstv new_sstv = config.getSstv();
  new_sstv.enabled = true;
  new_sstv.frequency.setFrequency("100.0000");
  new_sstv.overlay_data = false;
  ASSERT_TRUE(config.setSstv(new_sstv));

  cfg::DataPackets new_data_packets = config.getDataPackets();
  new_data_packets.enabled = true;
  new_data_packets.frequency.setFrequency("200.0000");
  new_data_packets.mode = cfg::DataPackets::Mode::BPSK1000;
  new_data_packets.morse_call_sign = false;
  new_data_packets.comment = "Hello BPSK!";
  ASSERT_TRUE(config.setDataPackets(new_data_packets));

  cfg::file::saveConfiguration(es_, config, path);

  ASSERT_TRUE(std::filesystem::exists(path))
      << "File does not exist when it should.";

  // Load in the same file
  std::ifstream file(path);
  json saved_cfg = json::parse(file);
  verifyFieldsAndTypes(saved_cfg);

  EXPECT_EQ(saved_cfg["general"]["project_name"].get<std::string>(), new_general.project_name);
  EXPECT_EQ(saved_cfg["general"]["main_board"].get<std::string>(), "pi_4");
  EXPECT_EQ(saved_cfg["general"]["starting_procedure"].get<std::string>(), "ascent");

  EXPECT_EQ(saved_cfg["debug"]["console_enabled"].get<bool>(), new_debug.console_enabled);
  EXPECT_EQ(saved_cfg["debug"]["print_errors"].get<bool>(), new_debug.print_errors);
  EXPECT_EQ(saved_cfg["debug"]["console_update_interval"].get<int>(), new_debug.console_update_interval);

  EXPECT_EQ(saved_cfg["server"]["tcp_socket_port"].get<int>(), new_server.tcp_socket_port);

  EXPECT_EQ(saved_cfg["telemetry"]["telemetry_enabled"].get<bool>(), new_telemetry.telemetry_enabled);

  EXPECT_EQ(saved_cfg["telemetry_aprs"]["telemetry_packets"].get<bool>(), new_aprs.telemetry_packets);

  EXPECT_EQ(saved_cfg["telemetry_sstv"]["enabled"].get<bool>(), new_sstv.enabled);

  EXPECT_EQ(saved_cfg["telemetry_data_packets"]["enabled"].get<bool>(), new_data_packets.enabled);
}

TEST_F(Configuration_File, loadsChangedConfiguration) {
  cfg::Configuration config = cfg::Configuration(es_);
  const std::string path = "changed_config.json";

  ASSERT_TRUE(std::filesystem::exists(path))
      << "File does not exist when it should.";

  // Load in the same file
  cfg::Configuration new_config = cfg::Configuration(es_);
  cfg::file::loadConfiguration(es_, new_config, path);

  cfg::General gen = new_config.getGeneral();
  EXPECT_EQ(gen.project_name, "New Project Name");
  EXPECT_EQ(gen.main_board_type, cfg::General::MainBoard::PI_4);
  EXPECT_EQ(gen.starting_procedure, cfg::Procedure::Type::ASCENT);

  cfg::Debug dbg = new_config.getDebug();
  EXPECT_EQ(dbg.console_enabled, true);
  EXPECT_EQ(dbg.print_errors, true);
  EXPECT_EQ(dbg.console_update_interval, 1100);

  cfg::Server srv = new_config.getServer();
  EXPECT_EQ(srv.tcp_socket_port, 7800);

  cfg::Telemetry tlm = new_config.getTelemetry();
  EXPECT_EQ(tlm.telemetry_enabled, true);
  EXPECT_EQ(tlm.call_sign, "CA2LL");

  cfg::Aprs aprs = new_config.getAprs();
  EXPECT_EQ(aprs.telemetry_packets, true);
  EXPECT_EQ(aprs.position_packets, true);
  EXPECT_EQ(aprs.frequency.getFrequency(), "000.0000");
  EXPECT_EQ(aprs.ssid, 2);
  EXPECT_EQ(aprs.destination_address, "TEST");
  EXPECT_EQ(aprs.destination_ssid, 3);
  EXPECT_EQ(aprs.symbol_table, cfg::Aprs::SymbolTable::ALTERNATE);
  EXPECT_EQ(aprs.symbol, 'a');
  EXPECT_EQ(aprs.comment, "Hello");

  cfg::Sstv sstv = new_config.getSstv();
  EXPECT_EQ(sstv.enabled, true);
  EXPECT_EQ(sstv.frequency.getFrequency(), "100.0000");
  EXPECT_EQ(sstv.overlay_data, false);

  cfg::DataPackets data_packets = new_config.getDataPackets();
  EXPECT_EQ(data_packets.enabled, true);
  EXPECT_EQ(data_packets.frequency.getFrequency(), "200.0000");
  EXPECT_EQ(data_packets.mode, cfg::DataPackets::Mode::BPSK1000);
  EXPECT_EQ(data_packets.morse_call_sign, false);
  EXPECT_EQ(data_packets.comment, "Hello BPSK!");

  EXPECT_EQ(es_.getTotalPackets(), 0);

  while (es_.getNumPackets() != 0) {
    data::ErrorStreamPacket pkt;
    es_.getPacket(pkt);
    std::cout << pkt << std::endl;
  }
}