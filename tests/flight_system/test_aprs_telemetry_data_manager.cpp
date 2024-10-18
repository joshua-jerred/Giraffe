/// =*========GIRAFFE========*=
/// A Unified Flight Command and Control System
/// https://github.com/joshua-jerred/Giraffe
/// https://giraffe.joshuajer.red/
/// =*=======================*=
///
/// @file   test_aprs_telemetry_data_manager.cpp
///
/// =*=======================*=
/// @author     Joshua Jerred (https://joshuajer.red)
/// @date       2024-10-17
/// @copyright  2024 (license to be defined)

#include "unit_test.hpp"

#include "aprs_telemetry_data_manager.hpp"
#include "shared_data_helpers.hpp"

using namespace giraffe;

class IDataLinkStub : public gdl::IDataLink {
  bool sendTelemetryData(TelemetryData &telemetry_data,
                         uint32_t message_id) override {
    (void)telemetry_data;
    (void)message_id;
    return true;
  }
  bool sendTelemetryCoefficients(TelemetryData &telemetry_data) override {
    (void)telemetry_data;
    return true;
  }
  bool sendTelemetryParameterNames(TelemetryData &telemetry_data) override {
    (void)telemetry_data;
    return true;
  }
  bool sendTelemetryUnitsAndLabels(TelemetryData &telemetry_data) override {
    (void)telemetry_data;
    return true;
  }
  bool sendTelemetryBitSense(TelemetryData &telemetry_data) override {
    (void)telemetry_data;
    return true;
  }
};

class AprsTelemetryDataManagerTest : public ::testing::Test {
protected:
  virtual void SetUp() {
  }

  virtual void TearDown() {
  }

  FakeSharedData shared_data_{};
  cfg::Configuration config_{shared_data_.streams};
  IDataLinkStub data_link_{};

  AprsTelemetryDataManager aprs_telemetry_data_manager_{shared_data_, config_,
                                                        data_link_};
};

TEST_F(AprsTelemetryDataManagerTest, initialState) {
  EXPECT_FALSE(shared_data_.hasLogItems()) << shared_data_.dumpLog();
}