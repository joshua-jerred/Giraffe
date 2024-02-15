/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   test_data_module.cpp
 * @brief  Data module unit tests.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-02-15
 * @copyright  2024 (license to be defined)
 */

#include "data_module.hpp"
#include "unit_test.hpp"
#include <BoosterSeat/sleep.hpp>

class DataModuleTests : public ::testing::Test {
protected:
  virtual void SetUp() {
    sd_ = new data::SharedData();
    cfg_ = new cfg::Configuration(sd_->streams);

    cfg_->data_module_data.setLogDataToFile(false);
    cfg_->data_module_log.setLogToFile(false);

    data_module_ = new modules::DataModule(*sd_, *cfg_);
  }
  virtual void TearDown() {
    delete data_module_;
    delete cfg_;
    delete sd_;
  }

  void cycleDataModule() {
    data_module_->start();
    bst::sleep(120); // it's run interval is 100ms
    data_module_->stop();
  }

  data::SharedData *sd_ = nullptr;
  cfg::Configuration *cfg_ = nullptr;
  modules::DataModule *data_module_ = nullptr;
};

TEST_F(DataModuleTests, environmental_data) {
  const auto K_NODE_ID = node::Identification::EXTENSION;

  data::DataPacket hum_packet;
  hum_packet.source = K_NODE_ID;
  hum_packet.secondary_identifier = "hum_test";
  hum_packet.identifier = data::DataId::ENVIRONMENTAL_humidity;
  hum_packet.value = "10.0";
  // add the same packet multiple times
  sd_->streams.data.addPacket(hum_packet);
  sd_->streams.data.addPacket(hum_packet);
  sd_->streams.data.addPacket(hum_packet);

  data::DataPacket temp_packet;
  temp_packet.source = K_NODE_ID;
  temp_packet.secondary_identifier = "temp_test";
  temp_packet.identifier = data::DataId::ENVIRONMENTAL_temperature;
  temp_packet.value = "20.0";
  sd_->streams.data.addPacket(temp_packet);

  data::DataPacket pres_packet;
  pres_packet.source = K_NODE_ID;
  pres_packet.secondary_identifier = "pres_test";
  pres_packet.identifier = data::DataId::ENVIRONMENTAL_pressure;
  pres_packet.value = "30.0";
  sd_->streams.data.addPacket(pres_packet);

  cycleDataModule();

  const auto hum_data = sd_->frames.env_hum.getAll();
  const auto temp_data = sd_->frames.env_temp.getAll();
  const auto pres_data = sd_->frames.env_pres.getAll();

  EXPECT_EQ(hum_data.size(), 1);
  EXPECT_EQ(temp_data.size(), 1);
  EXPECT_EQ(pres_data.size(), 1);
  EXPECT_EQ(hum_data.at("hum_test").value, "10.0");
  EXPECT_EQ(temp_data.at("temp_test").value, "20.0");
  EXPECT_EQ(pres_data.at("pres_test").value, "30.0");

  hum_packet.value = "40.0";
  sd_->streams.data.addPacket(hum_packet);
  cycleDataModule();
  const auto hum_data2 = sd_->frames.env_hum.getAll();
  EXPECT_EQ(hum_data2.size(), 1);
  EXPECT_EQ(hum_data2.at("hum_test").value, "40.0");
}
