/// =*========GIRAFFE========*=
/// A Unified Flight Command and Control System
/// https://github.com/joshua-jerred/Giraffe
/// https://giraffe.joshuajer.red/
/// =*=======================*=
///
/// @file   test_aprs_manager.cpp
///
/// =*=======================*=
/// @author     Joshua Jerred (https://joshuajer.red)
/// @date       2024-10-17
/// @copyright  2024 (license to be defined)

#include "unit_test.hpp"

#include "aprs_manager.hpp"
#include "i_giraffe_data_link.hpp"
#include "shared_data_helpers.hpp"

using namespace giraffe;

class MockIDataLink final : public gdl::IDataLink {
public:
  gdl::Statistics getStatistics() override {
    return stats_;
  }

  gdl::Config &getConfig() override {
    return config_;
  }

  void updateTelemetryData(const gdl::IDataLink::TelemetryData &data) override {
    telemetry_data_ = data;
  }

  MOCK_METHOD(bool, sendTelemetryData,
              (gdl::IDataLink::TelemetryData & data, uint32_t message_id),
              (override));
  MOCK_METHOD(bool, sendTelemetryCoefficients,
              (gdl::IDataLink::TelemetryData & data), (override));
  MOCK_METHOD(bool, sendTelemetryParameterNames,
              (gdl::IDataLink::TelemetryData & data), (override));
  MOCK_METHOD(bool, sendTelemetryUnitsAndLabels,
              (gdl::IDataLink::TelemetryData & data), (override));
  MOCK_METHOD(bool, sendTelemetryBitSense,
              (gdl::IDataLink::TelemetryData & data), (override));

  gdl::IDataLink::TelemetryData &getTelemetryData() {
    return telemetry_data_;
  }

private:
  gdl::Statistics stats_{};
  gdl::Config config_{false}; // Not the controller
  gdl::IDataLink::TelemetryData telemetry_data_{};
};

class AprsTelemetryDataManagerTest : public ::testing::Test {
protected:
  static constexpr int POSITION_PACKET_INTERVAL = 5;
  static constexpr int TELEMETRY_PACKET_INTERVAL = 5;

  virtual void SetUp() {
    config_.telemetry_aprs.setPositionPackets(true);
    config_.telemetry_aprs.setPositionPacketInterval(POSITION_PACKET_INTERVAL);
    config_.telemetry_aprs.setTelemetryPackets(true);
    config_.telemetry_aprs.setTelemetryPacketInterval(
        TELEMETRY_PACKET_INTERVAL);

    aprs_manager_.loadConfiguration();

    EXPECT_TRUE(data_link_.getConfig().isAprsTelemetrySenderEnabled());
    EXPECT_EQ(POSITION_PACKET_INTERVAL * 1000,
              data_link_.getConfig().getAprsTelemetryDataIntervalMs());
  }

  virtual void TearDown() {
  }

  void causeDataUpdate() {
    data_update_timer_.setDone();
    aprs_manager_.update();
  }

  FakeSharedData shared_data_{};
  cfg::Configuration config_{shared_data_.streams};
  MockIDataLink data_link_{};
  bst::SoftwareTimer data_update_timer_{};

  AprsManager aprs_manager_{shared_data_, config_, data_link_,
                            data_update_timer_};
};

TEST_F(AprsTelemetryDataManagerTest, initialState) {
  EXPECT_FALSE(shared_data_.hasLogItems()) << shared_data_.dumpLog();
}

TEST_F(AprsTelemetryDataManagerTest, sendOnAirDefinitionPackets) {
  EXPECT_CALL(data_link_, sendTelemetryParameterNames(testing::_))
      .WillOnce(testing::Return(true));
  EXPECT_CALL(data_link_, sendTelemetryUnitsAndLabels(testing::_))
      .WillOnce(testing::Return(true));
  EXPECT_CALL(data_link_, sendTelemetryCoefficients(testing::_))
      .WillOnce(testing::Return(true));
  EXPECT_CALL(data_link_, sendTelemetryBitSense(testing::_))
      .WillOnce(testing::Return(true));

  EXPECT_TRUE(aprs_manager_.sendOnAirDefinitionPackets());
}

TEST_F(AprsTelemetryDataManagerTest, updatesTelemetryData) {
  data_update_timer_.setNotDone();
  aprs_manager_.update();

  auto calculated_data = shared_data_.blocks.calculated_data.get();
  calculated_data.battery_voltage_mv = 5000;
  calculated_data.external_temperature_c = 25;
  shared_data_.blocks.calculated_data.set(calculated_data);

  causeDataUpdate();

  auto &telemetry_data = data_link_.getTelemetryData();
  EXPECT_EQ(config_.general.getProjectName(), telemetry_data.getProjectTitle());

  auto &a1 = telemetry_data.getAnalog(
      signal_easel::aprs::telemetry::Parameter::Id::A1);
  EXPECT_EQ("VBatt", a1.getName());

  uint8_t raw_value = a1.getValue();
  EXPECT_EQ(41, raw_value); // (5000 mV / 1000) * 0.024 + 4;
}