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

#include "extension_module.hpp"
#include "to_string.hpp"
#include "unit_test.hpp"
#include <BoosterSeat/sleep.hpp>

class ExtensionModuleTests : public ::testing::Test {
protected:
  virtual void SetUp() {
    sd_ = new data::SharedData();
    cfg_ = new cfg::Configuration(sd_->streams);

    cfg_->extension_module.setStatusPollingRate(100);

    extension_module_ = new modules::ExtensionModule(*sd_, *cfg_);
  }
  virtual void TearDown() {
    delete extension_module_;
    delete cfg_;
    delete sd_;
  }

  void cycleExtensionModule() {
    extension_module_->start();
    bst::sleep(2000);
    extension_module_->stop();
  }

  data::SharedData *sd_ = nullptr;
  cfg::Configuration *cfg_ = nullptr;
  modules::ExtensionModule *extension_module_ = nullptr;
};

TEST_F(ExtensionModuleTests, pre_configured_extensions) {
  cycleExtensionModule();

  // Ensure no extensions are configured
  auto data = sd_->blocks.extension_module_stats.get();
  EXPECT_EQ(data.num_active, 0);
  EXPECT_EQ(data.num_configured, 0);
  EXPECT_EQ(data.num_inactive, 0);

  EXPECT_EQ(sd_->streams.log.getNumPackets(), 0);

  const std::vector<std::string> EXTENSIONS_TO_ADD = {"samm8q", "bme280"};

  cmd::Command cmd;
  cmd.command_id = cmd::CommandId::EXTENSION_MODULE_addPreConfiguredExtension;
  cmd.destination = node::Identification::EXTENSION_MODULE;

  for (const auto &ext_name : EXTENSIONS_TO_ADD) {
    cmd.str_arg = ext_name;
    extension_module_->addCommand(cmd);
  }

  cycleExtensionModule();

  data::LogPacket packet;
  while (sd_->streams.log.getPacket(packet)) {
    EXPECT_NE(packet.level, data::LogPacket::Level::ERROR)
        << util::to_string(packet);
    EXPECT_NE(packet.level, data::LogPacket::Level::WARN)
        << util::to_string(packet);
  }

  auto new_cfg = cfg_->extensions.getExtensions();
  EXPECT_EQ(new_cfg.size(), 2);

  data = sd_->blocks.extension_module_stats.get();
  EXPECT_EQ(data.num_configured, 2);
}
