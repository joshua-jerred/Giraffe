/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   test_flight_phase_manager.cpp
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-09-10
 * @copyright  2024 (license to be defined)
 */

#include <sstream>

#include "flight_phase_manager.hpp"
#include "shared_data_helpers.hpp"
#include "to_string.hpp"
#include "unit_test.hpp"

class FlightPhaseManagerTest : public ::testing::Test {
protected:
  using Parameter = PredictionParameters::Parameter;

  static constexpr size_t FILTER_RATE = 5;

  struct TestContainer {
    FakeSharedData shared_data{};
    FlightPhaseManager flight_phase_manager{shared_data, FlightPhase::UNKNOWN,
                                            0, FILTER_RATE};
    PredictionParameters detection_data{shared_data};
    data::FlightData &flight_data = shared_data.flight_data;
    data::blocks::Block<data::blocks::CalculatedData> &calculated_data =
        shared_data.blocks.calculated_data;
    data::blocks::Block<data::blocks::LocationData> &location_data =
        shared_data.blocks.location_data;
    data::blocks::Block<data::blocks::ImuData> &imu_data =
        shared_data.blocks.imu_data;
  };

  virtual void SetUp() {
    test_ = new TestContainer{};
    manager_ = &test_->flight_phase_manager;
  }

  virtual void TearDown() {
    // predictor_ = nullptr;
    delete test_;
  }

  bool validateNoError() {
    if (!test_->shared_data.hasLogItems()) {
      return true;
    }

    data::LogPacket packet;
    while (test_->shared_data.streams.log.getPacket(packet)) {
      std::cout << util::to_string(packet) << std::endl;
    }
    return false;
  }

  bool validateHasError(DiagnosticId error_id) {
    if (!test_->shared_data.hasLogItems()) {
      return false;
    }

    std::stringstream ss;

    data::LogPacket packet;
    while (test_->shared_data.streams.log.getPacket(packet)) {
      if (packet.id == error_id) {
        return true;
      }

      ss << util::to_string(packet) << std::endl;
    }

    std::cout << ss.str();
    return false;
  }

  bool validateHasDataItem(data::DataId data_id) {
    if (!test_->shared_data.hasDataItems()) {
      return false;
    }

    std::stringstream ss;

    data::DataPacket packet;
    while (test_->shared_data.streams.data.getPacket(packet)) {
      if (packet.identifier == data_id) {
        return true;
      }

      ss << util::to_string(packet) << std::endl;
    }

    std::cout << ss.str();
    return false;
  }

  bool validateNoData() {
    if (!test_->shared_data.hasDataItems()) {
      return true;
    }

    data::DataPacket packet;
    while (test_->shared_data.streams.data.getPacket(packet)) {
      std::cout << util::to_string(packet) << std::endl;
    }
    return false;
  }

  void dumpDataStream() {
    data::DataPacket packet;
    while (test_->shared_data.streams.data.getPacket(packet)) {
      std::cout << util::to_string(packet) << std::endl;
    }
  }

  void tick(size_t ticks = 1) {
    for (size_t i = 0; i < ticks; i++) {
      manager_->update();
    }
  }

  void setParamsInvalid() {
    setLocationDataInvalid(test_->shared_data);
    setCalculatedDataInvalid(test_->shared_data);
  }

  void setParamsLaunch() {
    setLocationDataLaunch(test_->shared_data);
    setCalculatedDataLaunch(test_->shared_data);
  }

  void setParamsAscent() {
    setLocationDataAscent(test_->shared_data);
    setCalculatedDataAscent(test_->shared_data);
  }

  void setParamsDescent() {
    setLocationDataDescent(test_->shared_data);
    setCalculatedDataDescent(test_->shared_data);
  }

  void setParamsRecovery() {
    setLocationDataRecovery(test_->shared_data);
    setCalculatedDataRecovery(test_->shared_data);
  }

  void bringToLaunch() {
    test_->flight_phase_manager.setPreLaunch();
    ASSERT_EQ(manager_->getCurrentFlightPhase(), FlightPhase::PRE_LAUNCH);
    test_->flight_phase_manager.requestLaunch();
    ASSERT_EQ(manager_->getCurrentFlightPhase(), FlightPhase::LAUNCH);
    ASSERT_EQ(test_->flight_data.getFlightPhase(), FlightPhase::LAUNCH);

    ASSERT_EQ(validateNoError(), true);

    // Clear the streams
    test_->shared_data.streams.data.reset();
    test_->shared_data.streams.log.reset();
  }

  void bringToAscent() {
    bringToLaunch();
    setParamsAscent();
    tick(FILTER_RATE);
    ASSERT_EQ(manager_->getCurrentFlightPhase(), FlightPhase::ASCENT);
    ASSERT_EQ(test_->flight_data.getFlightPhase(), FlightPhase::ASCENT);
    ASSERT_EQ(validateNoError(), true);

    // Clear the streams
    test_->shared_data.streams.data.reset();
    test_->shared_data.streams.log.reset();
  }

  void bringToDescent() {
    bringToAscent();
    setParamsDescent();
    tick(FILTER_RATE);
    ASSERT_EQ(manager_->getCurrentFlightPhase(), FlightPhase::DESCENT);
    ASSERT_EQ(test_->flight_data.getFlightPhase(), FlightPhase::DESCENT);
    ASSERT_EQ(validateNoError(), true);

    // Clear the streams
    test_->shared_data.streams.data.reset();
    test_->shared_data.streams.log.reset();
  }

  TestContainer *test_ = nullptr;
  FlightPhaseManager *manager_ = nullptr;
};

TEST_F(FlightPhaseManagerTest, initialState) {
  EXPECT_EQ(manager_->getCurrentFlightPhase(), FlightPhase::UNKNOWN);
  tick();
  EXPECT_EQ(manager_->getCurrentFlightPhase(), FlightPhase::UNKNOWN);

  EXPECT_TRUE(validateHasError(DiagnosticId::FLIGHT_RUNNER_flightPhaseUnknown));
}

TEST_F(FlightPhaseManagerTest, setPrelaunch) {
  EXPECT_EQ(manager_->getCurrentFlightPhase(), FlightPhase::UNKNOWN);
  test_->shared_data.streams.data.reset(); // Unknown reported on startup

  test_->flight_phase_manager.setPreLaunch();
  EXPECT_EQ(manager_->getCurrentFlightPhase(), FlightPhase::PRE_LAUNCH);

  tick(FILTER_RATE);

  EXPECT_EQ(manager_->getCurrentFlightPhase(), FlightPhase::PRE_LAUNCH);
  EXPECT_EQ(test_->flight_data.getFlightPhase(), FlightPhase::PRE_LAUNCH);

  EXPECT_EQ(test_->shared_data.streams.data.getNumPackets(), 1);
  EXPECT_TRUE(
      validateHasDataItem(data::DataId::FLIGHT_RUNNER_flightPhaseChange));

  // FLIGHT_RUNNER_flightPhaseUnknown should not be logged as it's suppressed
  // when in pre-launch.
  EXPECT_TRUE(validateNoError());
}

TEST_F(FlightPhaseManagerTest, prelaunchToAscent) {
  test_->shared_data.streams.data.reset(); // Unknown reported on startup
  test_->flight_phase_manager.setPreLaunch();
  EXPECT_TRUE(
      validateHasDataItem(data::DataId::FLIGHT_RUNNER_flightPhaseChange));
  setParamsAscent();
  tick(FILTER_RATE);

  EXPECT_EQ(manager_->getCurrentFlightPhase(), FlightPhase::ASCENT);
  EXPECT_TRUE(
      validateHasDataItem(data::DataId::FLIGHT_RUNNER_flightPhaseChange));
  EXPECT_TRUE(
      validateHasError(DiagnosticId::FLIGHT_RUNNER_phaseChangeUnexpected));

  tick(FILTER_RATE);

  EXPECT_TRUE(validateNoError());

  // No transition, so now new data.
  EXPECT_TRUE(validateNoData());

  EXPECT_EQ(manager_->getCurrentFlightPhase(), FlightPhase::ASCENT);
}

TEST_F(FlightPhaseManagerTest, setLaunch) {
  EXPECT_NE(manager_->getCurrentFlightPhase(), FlightPhase::PRE_LAUNCH);
  EXPECT_FALSE(manager_->requestLaunch());
  test_->shared_data.streams.data.reset();

  test_->flight_phase_manager.setPreLaunch();
  EXPECT_EQ(manager_->getCurrentFlightPhase(), FlightPhase::PRE_LAUNCH);
  EXPECT_EQ(test_->shared_data.streams.data.getNumPackets(), 1);

  EXPECT_TRUE(manager_->requestLaunch());
  EXPECT_EQ(manager_->getCurrentFlightPhase(), FlightPhase::LAUNCH);
  EXPECT_EQ(test_->shared_data.streams.data.getNumPackets(), 2);

  // FLIGHT_RUNNER_flightPhaseUnknown should not be logged as it's suppressed
  // when in pre-launch.
  EXPECT_TRUE(validateNoError());
}

TEST_F(FlightPhaseManagerTest, launchErrorOnUnknown) {
  bringToLaunch();
  tick(FILTER_RATE);
  EXPECT_TRUE(validateHasError(DiagnosticId::FLIGHT_RUNNER_flightPhaseUnknown));
  EXPECT_EQ(manager_->getCurrentFlightPhase(), FlightPhase::LAUNCH);

  // Verify that the error is not logged again.
  tick(FILTER_RATE);
  EXPECT_TRUE(validateNoError());
  EXPECT_EQ(test_->flight_data.getPredictedPhase(), FlightPhase::UNKNOWN);

  // Clear the error
  setParamsLaunch();
  tick(FILTER_RATE);
  EXPECT_EQ(test_->flight_data.getPredictedPhase(), FlightPhase::LAUNCH);

  // Verify that the error is logged again.
  setParamsInvalid();
  tick(FILTER_RATE);
  EXPECT_TRUE(validateHasError(DiagnosticId::FLIGHT_RUNNER_flightPhaseUnknown));
  EXPECT_EQ(manager_->getCurrentFlightPhase(), FlightPhase::LAUNCH);
}

TEST_F(FlightPhaseManagerTest, launchToAscent) {
  bringToLaunch();
  setParamsAscent();
  tick(FILTER_RATE);
  EXPECT_EQ(manager_->getCurrentFlightPhase(), FlightPhase::ASCENT);
  EXPECT_TRUE(
      validateHasDataItem(data::DataId::FLIGHT_RUNNER_flightPhaseChange));
  EXPECT_TRUE(validateNoError());

  tick(FILTER_RATE);

  EXPECT_TRUE(validateNoError());

  // No transition, so now new data.
  EXPECT_TRUE(validateNoData());

  EXPECT_EQ(manager_->getCurrentFlightPhase(), FlightPhase::ASCENT);
}

TEST_F(FlightPhaseManagerTest, ascentToDescent) {
  bringToAscent();
  setParamsDescent();
  tick(FILTER_RATE);
  EXPECT_EQ(manager_->getCurrentFlightPhase(), FlightPhase::DESCENT);
  EXPECT_TRUE(
      validateHasDataItem(data::DataId::FLIGHT_RUNNER_flightPhaseChange));
  EXPECT_TRUE(validateNoError());

  tick(FILTER_RATE);

  EXPECT_TRUE(validateNoError());

  // No transition, so now new data.
  EXPECT_TRUE(validateNoData());

  EXPECT_EQ(manager_->getCurrentFlightPhase(), FlightPhase::DESCENT);
}

TEST_F(FlightPhaseManagerTest, ascentToRecovery) {
  bringToAscent();
  setParamsRecovery();
  tick(FILTER_RATE);
  EXPECT_EQ(manager_->getCurrentFlightPhase(), FlightPhase::RECOVERY);
  EXPECT_TRUE(
      validateHasDataItem(data::DataId::FLIGHT_RUNNER_flightPhaseChange));
  EXPECT_TRUE(
      validateHasError(DiagnosticId::FLIGHT_RUNNER_phaseChangeUnexpected));

  tick(FILTER_RATE);

  EXPECT_TRUE(validateNoError());

  // No transition, so now new data.
  EXPECT_TRUE(validateNoData());

  EXPECT_EQ(manager_->getCurrentFlightPhase(), FlightPhase::RECOVERY);
}

TEST_F(FlightPhaseManagerTest, descentToRecovery) {
  bringToDescent();
  setParamsRecovery();
  tick(FILTER_RATE);
  EXPECT_EQ(manager_->getCurrentFlightPhase(), FlightPhase::RECOVERY);
  EXPECT_TRUE(
      validateHasDataItem(data::DataId::FLIGHT_RUNNER_flightPhaseChange));

  tick(FILTER_RATE);

  EXPECT_TRUE(validateNoError());

  // No transition, so no new data.
  EXPECT_TRUE(validateNoData());

  EXPECT_EQ(manager_->getCurrentFlightPhase(), FlightPhase::RECOVERY);
}
