/**
 * @file test.cpp
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief Unit test for the configuration json handlers.
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
#include "streams.h"

using json = nlohmann::ordered_json;

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
    es_.reset();
  }
  virtual void TearDown() {
  }

  data::ErrorStream es_;
  cfg::Configuration config_ = cfg::Configuration(es_);
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
                           cfg::json::generalToJson(config_.getGeneral()), 3));
  ASSERT_TRUE(HAS_SETTINGS(gMetaData["debug"],
                           cfg::json::debugToJson(config_.getDebug()), 3));
  ASSERT_TRUE(HAS_SETTINGS(gMetaData["server"],
                           cfg::json::serverToJson(config_.getServer()), 1));
  ASSERT_TRUE(HAS_SETTINGS(gMetaData["telemetry"],
                           cfg::json::telemetryToJson(config_.getTelemetry()),
                           2));
  ASSERT_TRUE(HAS_SETTINGS(gMetaData["telemetry_aprs"],
                           cfg::json::aprsToJson(config_.getAprs()), 9));
  ASSERT_TRUE(HAS_SETTINGS(gMetaData["telemetry_sstv"],
                           cfg::json::sstvToJson(config_.getSstv()), 4));
  ASSERT_TRUE(
      HAS_SETTINGS(gMetaData["telemetry_data_packets"],
                   cfg::json::dataPacketsToJson(config_.getDataPackets()), 5));
}

TEST_F(Configuration_Json, allToJson) {
  json all = cfg::json::allToJson(config_);
  for (auto &[key, val] : gMetaData.items()) {
    EXPECT_TRUE(all.contains(key))
        << "The JSON does not contain the key: " << key;
  }
}