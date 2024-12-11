/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   test_extension_module.cpp
 * @brief  Extension module unit tests.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-02-20
 * @copyright  2024 (license to be defined)
 */

#include <BoosterSeat/sleep.hpp>

#include "extension_module.hpp"
#include "to_string.hpp"
#include "unit_test.hpp"

#include "shared_data_helpers.hpp"

class ExtensionModuleTests : public ::testing::Test {
protected:
  virtual void SetUp() {
    // Set to a low value to speed up the tests.
    config_.extension_module.setStatusPollingRate(5);
  }

  void cycleExtensionModule() {
    extension_module_.start();
    bst::sleep(150);
    extension_module_.stop();
  }

  FakeSharedData shared_data_;
  cfg::Configuration config_{shared_data_.streams};
  modules::ExtensionModule extension_module_{shared_data_, config_};
};

TEST_F(ExtensionModuleTests, pre_configured_extensions) {
  cycleExtensionModule();

  // Ensure no extensions are configured
  auto data = shared_data_.blocks.extension_module_stats.get();
  EXPECT_EQ(data.num_active, 0);
  EXPECT_EQ(data.num_configured, 0);
  EXPECT_EQ(data.num_inactive, 0);

  EXPECT_EQ(shared_data_.streams.log.getNumPackets(), 0);

  const std::vector<std::string> EXTENSIONS_TO_ADD = {"samm8q", "bme280"};

  cmd::Command cmd;
  cmd.command_id = cmd::CommandId::EXTENSION_MODULE_addPreConfiguredExtension;
  cmd.destination = node::Identification::EXTENSION_MODULE;

  for (const auto &ext_name : EXTENSIONS_TO_ADD) {
    cmd.str_arg = ext_name;
    extension_module_.addCommand(cmd);
  }

  cycleExtensionModule();

  data::LogPacket packet;
  while (shared_data_.streams.log.getPacket(packet)) {
    EXPECT_NE(packet.level, data::LogPacket::Level::ERROR)
        << util::to_string(packet);
    EXPECT_NE(packet.level, data::LogPacket::Level::WARN)
        << util::to_string(packet);
  }

  auto new_cfg = config_.extensions.getExtensions();
  EXPECT_EQ(new_cfg.size(), 2);

  data = shared_data_.blocks.extension_module_stats.get();
  EXPECT_EQ(data.num_configured, 2);
}

TEST_F(ExtensionModuleTests, stop_extension) {
  cfg::ExtensionMetadata extension_metadata{
      .name = "test ext",
      .enabled = true,
      .type = cfg::gEnum::ExtensionType::BME280,
      .update_interval = 100,
      .critical = false,
      .extra_args = ""};
  config_.extensions.addExtension(extension_metadata);

  extension_module_.start();
  bst::sleep(500);

  const auto data = shared_data_.blocks.extension_module_stats.get();
  EXPECT_EQ(data.num_configured, 1);
  EXPECT_EQ(data.num_active, 1);

  cmd::Command cmd;
  cmd.command_id = cmd::CommandId::EXTENSION_MODULE_stopExtension;
  cmd.destination = node::Identification::EXTENSION_MODULE;
  cmd.str_arg = "test ext";

  bst::sleep(500);

  extension_module_.stop();

  EXPECT_FALSE(shared_data_.hasLogItems()) << shared_data_.dumpLog();
  EXPECT_TRUE(shared_data_.hasDataItems());
}