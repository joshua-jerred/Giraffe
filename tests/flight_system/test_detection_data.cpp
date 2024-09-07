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

TEST(DetectionDataTest, initialState) {
  data::FlightData flight_data{};
  data::blocks::Block<data::blocks::CalculatedData> calculated_data{};
  data::blocks::Block<data::blocks::LocationData> location_data{};
  data::blocks::Block<data::blocks::ImuData> imu_data{};

  DetectionData detection_data{flight_data, calculated_data, location_data,
                               imu_data};

  for (uint8_t i = 0;
       i < static_cast<uint8_t>(DetectionData::Parameter::Id::NUM_PARAMETERS);
       ++i) {
    auto parameter = detection_data.getParameter(
        static_cast<DetectionData::Parameter::Id>(i));
    EXPECT_FALSE(parameter.is_valid);
    EXPECT_EQ(parameter.id, static_cast<DetectionData::Parameter::Id>(i));
    EXPECT_EQ(parameter.value, 0.0);
  }
}