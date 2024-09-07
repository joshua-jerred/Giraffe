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

#include "detection_data.hpp"

#include "unit_test.hpp"

class DetectionDataTest : public ::testing::Test {
protected:
  using Parameter = DetectionData::Parameter;

  struct TestContainer {
    data::SharedData shared_data{};
    DetectionData detection_data{shared_data};

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
  }

  virtual void TearDown() {
    delete test_;
  }

  TestContainer *test_ = nullptr;

  void validateParametersInInitialState() {
    for (uint8_t i = 0; i < static_cast<uint8_t>(Parameter::Id::NUM_PARAMETERS);
         ++i) {
      auto parameter =
          test_->detection_data.getParameter(static_cast<Parameter::Id>(i));
      EXPECT_EQ(parameter.id, static_cast<Parameter::Id>(i));
      EXPECT_FALSE(parameter.is_valid);
      EXPECT_EQ(parameter.value, 0.0);
      EXPECT_FALSE(parameter.binary_value);
    }
  };
};

TEST_F(DetectionDataTest, initialState) {
  validateParametersInInitialState();
}

TEST_F(DetectionDataTest, updateParameters_Good_Values) {
  const data::blocks::CalculatedData CALC_DATA{
      .pressure_altitude_m = 100,
      .pressure_altitude_valid = true,
      .pressure_vertical_speed = 4,
      .pressure_vertical_speed_valid = true,
      .pressure_vertical_speed_mps_1min = 5,
      .pressure_vertical_speed_1min_valid = true,
      .distance_traveled_m = 1000,
      .distance_traveled_valid = true,
      .distance_from_launch_m = 100,
      .distance_from_launch_valid = true,
      .distance_from_ground_m = 100,
      .distance_from_ground_valid = true,
      .average_speed_valid = true,
      .average_horiz_speed_mps_1min = 100,
      .average_vert_speed_mps_1min = 100,
      .max_horizontal_speed_mps = 100,
      .max_vertical_speed_mps = 100,
      .max_speed_valid = true,
  };

  const data::blocks::LocationData LOC_DATA{
      .have_gps_source = true,
      .current_gps_fix = data::GpsFix::FIX_3D,
      .last_valid_gps_fix = data::GpsFix::FIX_3D,
      .last_valid_gps_frame =
          data::GpsFrame{
              .gps_utc_time = bst::clck::TimePoint(),
              .is_valid = true,
              .fix = data::GpsFix::FIX_3D,
              .num_satellites = 1,
              .latitude = 2.0,
              .longitude = 3.0,
              .horz_accuracy = 4.0,
              .altitude = 5.0,
              .vert_accuracy = 6.0,
              .vertical_speed = 7.0,
              .horizontal_speed = 8.0,
              .speed_accuracy = 9.0,
              .heading_of_motion = 10.0,
              .heading_accuracy = 11.0,
          },
  };

  // anything we want in test_->flight_data is already initialized
  test_->calculated_data.set(CALC_DATA);
  test_->location_data.set(LOC_DATA);
  test_->imu_data.set(data::blocks::ImuData{}); /// @todo add imu data

  // Validate that the parameters are still in the initial state
  validateParametersInInitialState();
  // Update the parameters
  test_->detection_data.updateParameters();

  // Validate that the parameters have been updated
  using Id = Parameter::Id;
  auto &detect = test_->detection_data;
  // EXPECT_TRUE(detect.getParameter(Id::MISSION_CLOCK).is_valid);
  // EXPECT_TRUE(detect.getParameter(Id::MISSION_CLOCK_RUNNING).is_valid);
  // EXPECT_TRUE(detect.getParameter(Id::MISSION_CLOCK_RUNNING).binary_value);
  // EXPECT_TRUE(detect.getParameter(Id::UP_TIME).is_valid);

  EXPECT_TRUE(detect.getParameter(Id::GPS_ALTITUDE).is_valid);
  EXPECT_EQ(detect.getParameter(Id::GPS_ALTITUDE).value,
            LOC_DATA.last_valid_gps_frame.altitude);

  EXPECT_TRUE(detect.getParameter(Id::GPS_VERTICAL_SPEED).is_valid);
  EXPECT_EQ(detect.getParameter(Id::GPS_VERTICAL_SPEED).value,
            LOC_DATA.last_valid_gps_frame.vertical_speed);

  EXPECT_TRUE(detect.getParameter(Id::GPS_VERTICAL_SPEED_1MIN).is_valid);
  EXPECT_EQ(detect.getParameter(Id::GPS_VERTICAL_SPEED_1MIN).value,
            CALC_DATA.average_vert_speed_mps_1min);

  EXPECT_TRUE(detect.getParameter(Id::GPS_HORIZONTAL_SPEED).is_valid);
  EXPECT_EQ(detect.getParameter(Id::GPS_HORIZONTAL_SPEED).value,
            LOC_DATA.last_valid_gps_frame.horizontal_speed);

  EXPECT_TRUE(detect.getParameter(Id::GPS_HORIZONTAL_SPEED_1MIN).is_valid);
  EXPECT_EQ(detect.getParameter(Id::GPS_HORIZONTAL_SPEED_1MIN).value,
            CALC_DATA.average_horiz_speed_mps_1min);

  EXPECT_TRUE(detect.getParameter(Id::PRESSURE_ALTITUDE).is_valid);
  EXPECT_EQ(detect.getParameter(Id::PRESSURE_ALTITUDE).value,
            CALC_DATA.pressure_altitude_m);

  EXPECT_TRUE(detect.getParameter(Id::PRESSURE_VERTICAL_SPEED).is_valid);
  EXPECT_EQ(detect.getParameter(Id::PRESSURE_VERTICAL_SPEED).value,
            CALC_DATA.pressure_vertical_speed);

  EXPECT_TRUE(detect.getParameter(Id::PRESSURE_VERTICAL_SPEED_1MIN).is_valid);
  EXPECT_EQ(detect.getParameter(Id::PRESSURE_VERTICAL_SPEED_1MIN).value,
            CALC_DATA.pressure_vertical_speed_mps_1min);

  EXPECT_TRUE(detect.getParameter(Id::DISTANCE_FROM_GROUND).is_valid);
  EXPECT_EQ(detect.getParameter(Id::DISTANCE_FROM_GROUND).value,
            CALC_DATA.distance_from_ground_m);

  EXPECT_TRUE(detect.getParameter(Id::DISTANCE_TRAVELED).is_valid);
  EXPECT_EQ(detect.getParameter(Id::DISTANCE_TRAVELED).value,
            CALC_DATA.distance_traveled_m);

  EXPECT_TRUE(detect.getParameter(Id::DISTANCE_FROM_LAUNCH).is_valid);
  EXPECT_EQ(detect.getParameter(Id::DISTANCE_FROM_LAUNCH).value,
            CALC_DATA.distance_from_launch_m);
}

TEST_F(DetectionDataTest, valueValidation_IN_RANGE_and_DO_NOT_UPDATE_filter) {
  data::blocks::LocationData location_data{
      .have_gps_source = true,
      .current_gps_fix = data::GpsFix::FIX_3D,
      .last_valid_gps_fix = data::GpsFix::FIX_3D,
      .last_valid_gps_frame = data::GpsFrame{.is_valid = true,
                                             .fix = data::GpsFix::FIX_3D,
                                             .altitude = 0.0},
  };
  // Validate that the parameters are still in the initial state
  validateParametersInInitialState();

  // Update the parameters
  test_->location_data.set(location_data);
  test_->detection_data.updateParameters();

  // Validate that the parameters have been updated
  using Id = Parameter::Id;
  auto &detect = test_->detection_data;

  // Test the IN_RANGE validation and DO_NOT_UPDATE filter
  EXPECT_TRUE(detect.getParameter(Id::GPS_ALTITUDE).is_valid);
  EXPECT_NEAR(detect.getParameter(Id::GPS_ALTITUDE).value, 0.0, 0.1);
  // Update with valid data
  location_data.last_valid_gps_frame.altitude = 100.0;
  test_->location_data.set(location_data);
  test_->detection_data.updateParameters();
  EXPECT_TRUE(detect.getParameter(Id::GPS_ALTITUDE).is_valid);
  EXPECT_NEAR(detect.getParameter(Id::GPS_ALTITUDE).value, 100.0, 0.1);
  // Bring the value closer to the max
  location_data.last_valid_gps_frame.altitude = 48001.0;
  test_->location_data.set(location_data);
  test_->detection_data.updateParameters();
  EXPECT_TRUE(detect.getParameter(Id::GPS_ALTITUDE).is_valid);
  EXPECT_NEAR(detect.getParameter(Id::GPS_ALTITUDE).value, 48001.0, 0.1);
  // Bring the value over the max
  location_data.last_valid_gps_frame.altitude = 50001.0;
  test_->location_data.set(location_data);
  test_->detection_data.updateParameters();
  EXPECT_FALSE(detect.getParameter(Id::GPS_ALTITUDE).is_valid);
  EXPECT_NEAR(detect.getParameter(Id::GPS_ALTITUDE).value, 48001.0, 0.1);
  // Set the value to the upper limit
  location_data.last_valid_gps_frame.altitude = 50000.0;
  test_->location_data.set(location_data);
  test_->detection_data.updateParameters();
  EXPECT_TRUE(detect.getParameter(Id::GPS_ALTITUDE).is_valid);
  EXPECT_NEAR(detect.getParameter(Id::GPS_ALTITUDE).value, 50000.0, 0.1);
  // Bring the value under the min
  location_data.last_valid_gps_frame.altitude = -451.0;
  test_->location_data.set(location_data);
  test_->detection_data.updateParameters();
  EXPECT_FALSE(detect.getParameter(Id::GPS_ALTITUDE).is_valid);
  EXPECT_NEAR(detect.getParameter(Id::GPS_ALTITUDE).value, 50000.0, 0.1);
  // Set the value to the lower limit
  location_data.last_valid_gps_frame.altitude = -450.0;
  test_->location_data.set(location_data);
  test_->detection_data.updateParameters();
  EXPECT_TRUE(detect.getParameter(Id::GPS_ALTITUDE).is_valid);
  EXPECT_NEAR(detect.getParameter(Id::GPS_ALTITUDE).value, -450.0, 0.1);
}

TEST_F(DetectionDataTest, valueValidation_CLAMP_IN_RANGE_filter) {
  data::blocks::CalculatedData CALC_DATA{.pressure_altitude_m = 0,
                                         .pressure_altitude_valid = true,
                                         .pressure_vertical_speed_mps_1min = 0,
                                         .pressure_vertical_speed_1min_valid =
                                             true,
                                         .distance_from_ground_m = 0,
                                         .distance_from_ground_valid = true};

  // Validate that the parameters are still in the initial state
  validateParametersInInitialState();

  // Update the parameters
  test_->calculated_data.set(CALC_DATA);
  test_->detection_data.updateParameters();

  // Validate that the parameters have been updated
  using Id = Parameter::Id;
  auto &detect = test_->detection_data;

  // Test the CLAMP_IN_RANGE filter
  EXPECT_TRUE(detect.getParameter(Id::PRESSURE_ALTITUDE).is_valid);
  EXPECT_NEAR(detect.getParameter(Id::PRESSURE_ALTITUDE).value, 0.0, 0.1);
  // Update with valid data
  CALC_DATA.pressure_altitude_m = 100.0;
  test_->calculated_data.set(CALC_DATA);
  test_->detection_data.updateParameters();
  EXPECT_TRUE(detect.getParameter(Id::PRESSURE_ALTITUDE).is_valid);
  EXPECT_NEAR(detect.getParameter(Id::PRESSURE_ALTITUDE).value, 100.0, 0.1);
  // Above the max
  CALC_DATA.pressure_altitude_m = 50001.0;
  test_->calculated_data.set(CALC_DATA);
  test_->detection_data.updateParameters();
  EXPECT_FALSE(detect.getParameter(Id::PRESSURE_ALTITUDE).is_valid);
  EXPECT_NEAR(detect.getParameter(Id::PRESSURE_ALTITUDE).value, 50000.0, 0.1);
  // Below the min
  CALC_DATA.pressure_altitude_m = -451.0;
  test_->calculated_data.set(CALC_DATA);
  test_->detection_data.updateParameters();
  EXPECT_FALSE(detect.getParameter(Id::PRESSURE_ALTITUDE).is_valid);
  EXPECT_NEAR(detect.getParameter(Id::PRESSURE_ALTITUDE).value, -450.0, 0.1);
  // Back to the middle
  CALC_DATA.pressure_altitude_m = 100.0;
  test_->calculated_data.set(CALC_DATA);
  test_->detection_data.updateParameters();
  EXPECT_TRUE(detect.getParameter(Id::PRESSURE_ALTITUDE).is_valid);
  EXPECT_NEAR(detect.getParameter(Id::PRESSURE_ALTITUDE).value, 100.0, 0.1);
}

TEST_F(DetectionDataTest, valueValidation_ZERO_IF_INVALID_filter) {
  data::blocks::CalculatedData CALC_DATA{.pressure_vertical_speed_mps_1min = 0,
                                         .pressure_vertical_speed_1min_valid =
                                             true};

  // Validate that the parameters are still in the initial state
  validateParametersInInitialState();

  // Update the parameters
  test_->calculated_data.set(CALC_DATA);
  test_->detection_data.updateParameters();

  // Validate that the parameters have been updated
  using Id = Parameter::Id;
  auto &detect = test_->detection_data;

  // Test the ZERO_IF_INVALID filter
  EXPECT_TRUE(detect.getParameter(Id::PRESSURE_VERTICAL_SPEED_1MIN).is_valid);
  EXPECT_NEAR(detect.getParameter(Id::PRESSURE_VERTICAL_SPEED_1MIN).value, 0.0,
              0.1);
  // Update with valid data
  CALC_DATA.pressure_vertical_speed_mps_1min = 50.0;
  test_->calculated_data.set(CALC_DATA);
  test_->detection_data.updateParameters();
  EXPECT_TRUE(detect.getParameter(Id::PRESSURE_VERTICAL_SPEED_1MIN).is_valid);
  EXPECT_NEAR(detect.getParameter(Id::PRESSURE_VERTICAL_SPEED_1MIN).value, 50.0,
              0.1);
  // Set the value to invalid
  CALC_DATA.pressure_vertical_speed_1min_valid = false;
  test_->calculated_data.set(CALC_DATA);
  test_->detection_data.updateParameters();
  EXPECT_FALSE(detect.getParameter(Id::PRESSURE_VERTICAL_SPEED_1MIN).is_valid);
  EXPECT_NEAR(detect.getParameter(Id::PRESSURE_VERTICAL_SPEED_1MIN).value, 0.0,
              0.1);
  // Update with valid data
  CALC_DATA.pressure_vertical_speed_mps_1min = 100.0;
  CALC_DATA.pressure_vertical_speed_1min_valid = true;
  test_->calculated_data.set(CALC_DATA);
  test_->detection_data.updateParameters();
  EXPECT_TRUE(detect.getParameter(Id::PRESSURE_VERTICAL_SPEED_1MIN).is_valid);
  EXPECT_NEAR(detect.getParameter(Id::PRESSURE_VERTICAL_SPEED_1MIN).value,
              100.0, 0.1);
  // Set below the min
  CALC_DATA.pressure_vertical_speed_mps_1min = -101.0;
  test_->calculated_data.set(CALC_DATA);
  test_->detection_data.updateParameters();
  EXPECT_FALSE(detect.getParameter(Id::PRESSURE_VERTICAL_SPEED_1MIN).is_valid);
  EXPECT_NEAR(detect.getParameter(Id::PRESSURE_VERTICAL_SPEED_1MIN).value, 0.0,
              0.1);
  // Set above the max
  CALC_DATA.pressure_vertical_speed_mps_1min = 101.0;
  test_->calculated_data.set(CALC_DATA);
  test_->detection_data.updateParameters();
  EXPECT_FALSE(detect.getParameter(Id::PRESSURE_VERTICAL_SPEED_1MIN).is_valid);
  EXPECT_NEAR(detect.getParameter(Id::PRESSURE_VERTICAL_SPEED_1MIN).value, 0.0,
              0.1);
  // Set to a valid value
  CALC_DATA.pressure_vertical_speed_mps_1min = 50.0;
  test_->calculated_data.set(CALC_DATA);
  test_->detection_data.updateParameters();
  EXPECT_TRUE(detect.getParameter(Id::PRESSURE_VERTICAL_SPEED_1MIN).is_valid);
  EXPECT_NEAR(detect.getParameter(Id::PRESSURE_VERTICAL_SPEED_1MIN).value, 50.0,
              0.1);
}

TEST_F(DetectionDataTest, valueValidation_ALWAYS_UPDATE_filter) {
  data::blocks::CalculatedData CALC_DATA{.distance_from_ground_m = 0,
                                         .distance_from_ground_valid = true};

  // Validate that the parameters are still in the initial state
  validateParametersInInitialState();

  // Update the parameters
  test_->calculated_data.set(CALC_DATA);
  test_->detection_data.updateParameters();

  // Validate that the parameters have been updated
  using Id = Parameter::Id;
  auto &detect = test_->detection_data;

  // Test the ALWAYS_UPDATE filter. Always update the value, even if it's
  // invalid.
  EXPECT_TRUE(detect.getParameter(Id::DISTANCE_FROM_GROUND).is_valid);
  EXPECT_NEAR(detect.getParameter(Id::DISTANCE_FROM_GROUND).value, 0.0, 0.1);
  // Update with valid data
  CALC_DATA.distance_from_ground_m = 100.0;
  test_->calculated_data.set(CALC_DATA);
  test_->detection_data.updateParameters();
  EXPECT_TRUE(detect.getParameter(Id::DISTANCE_FROM_GROUND).is_valid);
  EXPECT_NEAR(detect.getParameter(Id::DISTANCE_FROM_GROUND).value, 100.0, 0.1);
  // Set the value to invalid
  CALC_DATA.distance_from_ground_valid = false;
  CALC_DATA.distance_from_ground_m = 250.0;
  test_->calculated_data.set(CALC_DATA);
  test_->detection_data.updateParameters();
  EXPECT_FALSE(detect.getParameter(Id::DISTANCE_FROM_GROUND).is_valid);
  EXPECT_NEAR(detect.getParameter(Id::DISTANCE_FROM_GROUND).value, 250.0, 0.1);
  // Set the value to valid and bring it outside the range.
  CALC_DATA.distance_from_ground_valid = true;
  CALC_DATA.distance_from_ground_m = 50001.0;
  test_->calculated_data.set(CALC_DATA);
  test_->detection_data.updateParameters();
  EXPECT_FALSE(detect.getParameter(Id::DISTANCE_FROM_GROUND).is_valid);
  EXPECT_NEAR(detect.getParameter(Id::DISTANCE_FROM_GROUND).value, 50001.0,
              0.1);
}