/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   test_detection_data.cpp
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-09-07
 * @copyright  2024 (license to be defined)
 */

#include "flight_phase_predictor.hpp"
#include "prediction_parameters.hpp"

#include "unit_test.hpp"

class FlightPhasePredictorTest : public ::testing::Test {
protected:
  using Parameter = PredictionParameters::Parameter;

  struct TestContainer {
    data::SharedData shared_data{};
    FlightPhasePredictor flight_phase_predictor{shared_data};
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
    predictor_ = &test_->flight_phase_predictor;
    prob_ = predictor_->getPhaseProbability();
  }

  virtual void TearDown() {
    // predictor_ = nullptr;
    delete test_;
  }

  void tick() {
    predictor_->update();
    prob_ = predictor_->getPhaseProbability();
  }

  TestContainer *test_ = nullptr;
  FlightPhasePredictor *predictor_ = nullptr;

  FlightPhasePredictor::Probability prob_{};
};

TEST_F(FlightPhasePredictorTest, initialState) {
  EXPECT_EQ(prob_.launch, 0.0);
  EXPECT_EQ(prob_.ascent, 0.0);
  EXPECT_EQ(prob_.descent, 0.0);
  EXPECT_EQ(prob_.recovery, 0.0);
  EXPECT_EQ(prob_.data_quality, 0.0);
  EXPECT_EQ(predictor_->getPredictedPhase(), FlightPhase::UNKNOWN);

  tick();

  EXPECT_EQ(prob_.launch, 0.0);
  EXPECT_EQ(prob_.ascent, 0.0);
  EXPECT_EQ(prob_.descent, 0.0);
  EXPECT_EQ(prob_.recovery, 0.0);
  EXPECT_EQ(prob_.data_quality, 0.0);
  EXPECT_EQ(predictor_->getPredictedPhase(), FlightPhase::UNKNOWN);
}

TEST_F(FlightPhasePredictorTest, enableGps) {

  tick();

  EXPECT_EQ(prob_.launch, 0.0);
  EXPECT_EQ(prob_.ascent, 0.0);
  EXPECT_EQ(prob_.descent, 0.0);
  EXPECT_EQ(prob_.recovery, 0.0);
  EXPECT_EQ(prob_.data_quality, 0.0);
  EXPECT_EQ(predictor_->getPredictedPhase(), FlightPhase::UNKNOWN);

  tick();

  EXPECT_EQ(prob_.launch, 0.0);
  EXPECT_EQ(prob_.ascent, 0.0);
  EXPECT_EQ(prob_.descent, 0.0);
  EXPECT_EQ(prob_.recovery, 0.0);
  EXPECT_EQ(prob_.data_quality, 0.0);
  EXPECT_EQ(predictor_->getPredictedPhase(), FlightPhase::UNKNOWN);

  // Throw in some good GPS data
  const data::blocks::LocationData LOC_DATA{
      .have_gps_source = true,
      .current_gps_fix = data::GpsFix::FIX_3D,
      .last_valid_gps_fix = data::GpsFix::FIX_3D,
      .last_valid_gps_frame =
          data::GpsFrame{
              .gps_utc_time = bst::clck::TimePoint(),
              .is_valid = true,
              .fix = data::GpsFix::FIX_3D,
              .num_satellites = 10,
              .horz_accuracy = 0.1,
              .altitude = 0.0,
              .vert_accuracy = 0.0,
              .vertical_speed = 0.0,
              .horizontal_speed = 0.0,
              .speed_accuracy = 0.0,
              .heading_accuracy = 11.0,
          },
  };
  test_->location_data.set(LOC_DATA);

  tick();

  const data::blocks::CalculatedData CALC_DATA{
      .pressure_altitude_m = 0,
      .pressure_altitude_valid = true,
      .pressure_vertical_speed = 4,
      .pressure_vertical_speed_valid = true,
      .pressure_vertical_speed_mps_1min = 5,
      .pressure_vertical_speed_1min_valid = true,
      .distance_traveled_m = 0,
      .distance_traveled_valid = true,
      .distance_from_launch_m = 0,
      .distance_from_launch_valid = true,
      .distance_from_ground_m = 0,
      .distance_from_ground_valid = true,
      .average_speed_valid = true,
      .average_horiz_speed_mps_1min = 0,
      .average_vert_speed_mps_1min = 0,
      .max_horizontal_speed_mps = 0,
      .max_vertical_speed_mps = 0,
      .max_speed_valid = true,
  };
  test_->calculated_data.set(CALC_DATA);

  tick();
  EXPECT_GT(prob_.data_quality, 50);

  EXPECT_GT(prob_.launch, 50);
  EXPECT_GT(prob_.launch, prob_.descent);
  EXPECT_GT(prob_.launch, prob_.recovery);
  EXPECT_GT(prob_.launch, prob_.ascent);

  EXPECT_EQ(predictor_->getPredictedPhase(), FlightPhase::LAUNCH);
}