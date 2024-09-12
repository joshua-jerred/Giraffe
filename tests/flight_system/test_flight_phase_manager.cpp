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

  void tick(size_t ticks = 1) {
    for (size_t i = 0; i < ticks; i++) {
      manager_->update();
    }
  }

  void setLaunch() {
    setLocationDataLaunch(test_->shared_data);
    setCalculatedDataAscent(test_->shared_data);
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

  test_->flight_phase_manager.setPreLaunch();
  EXPECT_EQ(manager_->getCurrentFlightPhase(), FlightPhase::PRE_LAUNCH);

  tick();

  EXPECT_EQ(manager_->getCurrentFlightPhase(), FlightPhase::PRE_LAUNCH);
  EXPECT_EQ(test_->flight_data.getFlightPhase(), FlightPhase::PRE_LAUNCH);

  // FLIGHT_RUNNER_flightPhaseUnknown should not be logged as it's suppressed
  // when in pre-launch.
  EXPECT_TRUE(validateNoError());
}