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

#include "flight_phase_manager.hpp"
#include "unit_test.hpp"

class FlightPhaseManagerTest : public ::testing::Test {
protected:
  using Parameter = PredictionParameters::Parameter;

  static constexpr size_t FILTER_RATE = 5;

  struct TestContainer {
    data::SharedData shared_data{};
    FlightPhaseManager flight_phase_manager{shared_data, 0, FILTER_RATE};
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

  void tick(size_t ticks = 1) {
    for (size_t i = 0; i < ticks; i++) {
      manager_->update();
    }
  }

  TestContainer *test_ = nullptr;
  FlightPhaseManager *manager_ = nullptr;
};

TEST_F(FlightPhaseManagerTest, initialState) {
  EXPECT_EQ(manager_->getCurrentFlightPhase(), FlightPhase::UNKNOWN);
  tick();
  EXPECT_EQ(manager_->getCurrentFlightPhase(), FlightPhase::UNKNOWN);
}