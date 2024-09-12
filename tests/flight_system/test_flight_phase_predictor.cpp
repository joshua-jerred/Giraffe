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

#include "shared_data_helpers.hpp"
#include "unit_test.hpp"

class FlightPhasePredictorTest : public ::testing::Test {
protected:
  using Parameter = PredictionParameters::Parameter;

  /// @brief When testing, the probability of the other phases should be the
  /// current phase probability minus this value.
  /// This is a nice value to shoot for. The higher this value, the better.
  static constexpr double OTHER_PHASES_PROBABILITY_THRESHOLD = 20.0;

  /// @brief When all parameters are valid and indicate a specific phase, the
  /// probability of that phase should be above this value.
  static constexpr double DETECTED_PHASE_PROBABILITY_THRESHOLD = 50.0;

  static constexpr double PROBABILITY_MAX = 100.0;

  struct TestContainer {
    data::SharedData shared_data{};
    FlightPhasePredictor flight_phase_predictor{shared_data};

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

TEST_F(FlightPhasePredictorTest, enableGpsAndCalculatedData) {

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

  // Throw in some good GPS and calculated data
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

  test_->location_data.set(LOC_DATA);
  test_->calculated_data.set(CALC_DATA);

  tick();

  EXPECT_GT(prob_.data_quality, 50);

  EXPECT_GT(prob_.launch, 50);
  EXPECT_GT(prob_.launch, prob_.descent);
  EXPECT_GT(prob_.launch, prob_.recovery);
  EXPECT_GT(prob_.launch, prob_.ascent);

  EXPECT_EQ(predictor_->getPredictedPhase(), FlightPhase::LAUNCH);
}

TEST_F(FlightPhasePredictorTest, setsToUnknownWhenDataQualityIsLow) {

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

  // Throw in some good GPS and calculated data
  setLocationDataValid(test_->shared_data);
  setCalculatedDataValid(test_->shared_data);

  tick();

  EXPECT_GT(prob_.data_quality, 50);

  EXPECT_GT(prob_.launch, 50);
  EXPECT_GT(prob_.launch, prob_.descent);
  EXPECT_GT(prob_.launch, prob_.recovery);
  EXPECT_GT(prob_.launch, prob_.ascent);

  EXPECT_EQ(predictor_->getPredictedPhase(), FlightPhase::LAUNCH);

  // Throw in some bad GPS and calculated data
  setLocationDataInvalid(test_->shared_data);
  setCalculatedDataInvalid(test_->shared_data);

  tick();

  EXPECT_LT(prob_.data_quality, 50);
  EXPECT_EQ(predictor_->getPredictedPhase(), FlightPhase::UNKNOWN);
}

TEST_F(FlightPhasePredictorTest, launchParameters) {
  setLocationDataLaunch(test_->shared_data);
  setCalculatedDataLaunch(test_->shared_data);

  tick();

  EXPECT_GT(prob_.launch, DETECTED_PHASE_PROBABILITY_THRESHOLD)
      << "Launch probability is too low.";
  EXPECT_LE(prob_.launch, PROBABILITY_MAX) << "Launch probability is too high.";

  const double THRESHOLD = prob_.launch - OTHER_PHASES_PROBABILITY_THRESHOLD;
  EXPECT_LT(prob_.ascent, THRESHOLD);
  EXPECT_LT(prob_.descent, THRESHOLD);
  EXPECT_LT(prob_.recovery, THRESHOLD);
  EXPECT_GT(prob_.data_quality, 75);
  EXPECT_EQ(predictor_->getPredictedPhase(), FlightPhase::LAUNCH);
}

TEST_F(FlightPhasePredictorTest, ascentParameters) {
  setLocationDataAscent(test_->shared_data);
  setCalculatedDataAscent(test_->shared_data);

  tick();

  EXPECT_GT(prob_.ascent, DETECTED_PHASE_PROBABILITY_THRESHOLD)
      << "Launch probability is too low.";
  EXPECT_LT(prob_.ascent, PROBABILITY_MAX) << "Launch probability is too high.";

  const double THRESHOLD = prob_.ascent - OTHER_PHASES_PROBABILITY_THRESHOLD;
  EXPECT_LT(prob_.launch, THRESHOLD);
  EXPECT_LT(prob_.descent, THRESHOLD);
  EXPECT_LT(prob_.recovery, THRESHOLD);
  EXPECT_GT(prob_.data_quality, 75);
  EXPECT_EQ(predictor_->getPredictedPhase(), FlightPhase::ASCENT);
}

TEST_F(FlightPhasePredictorTest, descentParameters) {
  setLocationDataDescent(test_->shared_data);
  setCalculatedDataDescent(test_->shared_data);

  tick();

  EXPECT_GT(prob_.descent, DETECTED_PHASE_PROBABILITY_THRESHOLD)
      << "Launch probability is too low.";
  EXPECT_LE(prob_.descent, PROBABILITY_MAX)
      << "Launch probability is too high.";

  const double THRESHOLD = prob_.descent - OTHER_PHASES_PROBABILITY_THRESHOLD;
  EXPECT_LT(prob_.launch, THRESHOLD);
  EXPECT_LT(prob_.ascent, THRESHOLD);
  EXPECT_LT(prob_.recovery, THRESHOLD);
  EXPECT_GT(prob_.data_quality, 75);
  EXPECT_EQ(predictor_->getPredictedPhase(), FlightPhase::DESCENT);
}

TEST_F(FlightPhasePredictorTest, recoveryParameters) {
  setLocationDataRecovery(test_->shared_data);
  setCalculatedDataRecovery(test_->shared_data);

  tick();

  EXPECT_GT(prob_.recovery, DETECTED_PHASE_PROBABILITY_THRESHOLD)
      << "Launch probability is too low.";
  EXPECT_LE(prob_.recovery, PROBABILITY_MAX)
      << "Launch probability is too high.";

  const double THRESHOLD = prob_.recovery - OTHER_PHASES_PROBABILITY_THRESHOLD;
  EXPECT_LT(prob_.launch, THRESHOLD);
  EXPECT_LT(prob_.ascent, THRESHOLD);
  EXPECT_LT(prob_.descent, THRESHOLD);
  EXPECT_GT(prob_.data_quality, 75);
  EXPECT_EQ(predictor_->getPredictedPhase(), FlightPhase::RECOVERY);
}