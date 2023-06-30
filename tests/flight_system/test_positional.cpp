/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   test_positional.cpp
 * @brief  Test of imu and gps data handling.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-06-29
 * @copyright  2023 (license to be defined)
 */

#include <BoosterSeat/clock.hpp> // for gps time
#include <chrono>                // for chrono literals

#include "positional.hpp"

#include "unit_test.hpp"

/**
 * @brief Verify that an empty frame is invalid.
 */
TEST(isGpsFrameValidTest, emptyFrameInvalid) {
  data::GpsFrame frame;
  EXPECT_FALSE(data::isGpsFrameValid(frame));
}

/**
 * @brief A frame with all zeros, and a good fix should be valid.
 */
TEST(isGpsFrameValidTest, validFrame) {
  data::GpsFrame frame{
      BoosterSeat::clck::now(), // time now
      data::GpsFix::FIX_3D,     // fix
      10,                       // num_satellites
      0.0,                      // latitude
      0.0,                      // longitude
      0.0,                      // horz_accuracy
      0.0,                      // altitude
      0.0,                      // vert_accuracy
      0.0,                      // vertical_speed,
      0.0,                      // horizontal_speed
      0.0,                      // speed_accuracy
      0.0,                      // heading_of_motion
      0.0                       // heading_accuracy
  };
  EXPECT_TRUE(data::isGpsFrameValid(frame));
}

/**
 * @brief Test to make sure that time is checked. Clock skew is ok,
 * but it should not be too far off (1 day limit for now?)
 */
TEST(isGpsFrameValidTest, invalidTime) {
  BoosterSeat::clck::TimePoint time_too_old = BoosterSeat::clck::now();
  time_too_old -= std::chrono::days(2);

  BoosterSeat::clck::TimePoint time_too_new{};
  time_too_new += std::chrono::days(2);

  data::GpsFrame frame_1{
      time_too_old,         // time (too old)
      data::GpsFix::FIX_3D, // fix
      10,                   // num_satellites
      0.0,                  // latitude
      0.0,                  // longitude
      0.0,                  // horz_accuracy
      0.0,                  // altitude
      0.0,                  // vert_accuracy
      0.0,                  // vertical_speed,
      0.0,                  // horizontal_speed
      0.0,                  // speed_accuracy
      0.0,                  // heading_of_motion
      0.0                   // heading_accuracy

  };

  data::GpsFrame frame_2{
      time_too_new,         // time (too new)
      data::GpsFix::FIX_3D, // fix
      10,                   // num_satellites
      0.0,                  // latitude
      0.0,                  // longitude
      0.0,                  // horz_accuracy
      0.0,                  // altitude
      0.0,                  // vert_accuracy
      0.0,                  // vertical_speed,
      0.0,                  // horizontal_speed
      0.0,                  // speed_accuracy
      0.0,                  // heading_of_motion
      0.0                   // heading_accuracy
  };

  EXPECT_FALSE(data::isGpsFrameValid(frame_1));
  EXPECT_FALSE(data::isGpsFrameValid(frame_2));
}

/**
 * @brief A non-2d or non-3d fix should be invalid.
 * A 2d fix is currently considered valid.
 */
TEST(isGpsFrameValidTest, fixValidity) {
  data::GpsFrame frame{
      BoosterSeat::clck::now(), // time now
      data::GpsFix::ERROR,      // fix
      10,                       // num_satellites
      0.0,                      // latitude
      0.0,                      // longitude
      0.0,                      // horz_accuracy
      0.0,                      // altitude
      0.0,                      // vert_accuracy
      0.0,                      // vertical_speed,
      0.0,                      // horizontal_speed
      0.0,                      // speed_accuracy
      0.0,                      // heading_of_motion
      0.0                       // heading_accuracy
  };

  frame.fix = data::GpsFix::ERROR;
  EXPECT_FALSE(data::isGpsFrameValid(frame));

  frame.fix = data::GpsFix::NO_FIX;
  EXPECT_FALSE(data::isGpsFrameValid(frame));

  frame.fix = data::GpsFix::FIX_2D;
  EXPECT_TRUE(data::isGpsFrameValid(frame));

  frame.fix = data::GpsFix::FIX_3D;
  EXPECT_TRUE(data::isGpsFrameValid(frame));
}

/**
 * @brief Verify that a fix and number of satellites are checked.
 */
TEST(isGpsFrameValidTest, numSatsValidity) {
  data::GpsFrame frame{
      BoosterSeat::clck::now(), // time now
      data::GpsFix::ERROR,      // fix
      0,                        // num_satellites
      0.0,                      // latitude
      0.0,                      // longitude
      0.0,                      // horz_accuracy
      0.0,                      // altitude
      0.0,                      // vert_accuracy
      0.0,                      // vertical_speed,
      0.0,                      // horizontal_speed
      0.0,                      // speed_accuracy
      0.0,                      // heading_of_motion
      0.0                       // heading_accuracy
  };

  // 2D fix requires at least 3 satellites
  frame.fix = data::GpsFix::FIX_2D;
  frame.num_satellites = 2;
  EXPECT_FALSE(data::isGpsFrameValid(frame));
  frame.num_satellites = 3;
  EXPECT_TRUE(data::isGpsFrameValid(frame));

  // 3D fix requires at least 4 satellites
  frame.fix = data::GpsFix::FIX_3D;
  frame.num_satellites = 3;
  EXPECT_FALSE(data::isGpsFrameValid(frame));
  frame.num_satellites = 4;
  EXPECT_TRUE(data::isGpsFrameValid(frame));
}

/**
 * @brief Verify validity of latitude.
 */
TEST(isGpsFrameValidTest, latitudeValidity) {
  data::GpsFrame frame{
      BoosterSeat::clck::now(), // time now
      data::GpsFix::FIX_3D,     // fix
      10,                       // num_satellites
      0.0,                      // latitude
      0.0,                      // longitude
      0.0,                      // horz_accuracy
      0.0,                      // altitude
      0.0,                      // vert_accuracy
      0.0,                      // vertical_speed,
      0.0,                      // horizontal_speed
      0.0,                      // speed_accuracy
      0.0,                      // heading_of_motion
      0.0                       // heading_accuracy
  };

  frame.latitude = -90.1;
  EXPECT_FALSE(data::isGpsFrameValid(frame));
  frame.latitude = -90.0;
  EXPECT_TRUE(data::isGpsFrameValid(frame));
  frame.latitude = 90.0;
  EXPECT_TRUE(data::isGpsFrameValid(frame));
  frame.latitude = 90.1;
  EXPECT_FALSE(data::isGpsFrameValid(frame));
}

/**
 * @brief Verify validity of longitude.
 */
TEST(isGpsFrameValidTest, longitudeValidity) {
  data::GpsFrame frame{
      BoosterSeat::clck::now(), // time now
      data::GpsFix::FIX_3D,     // fix
      10,                       // num_satellites
      0.0,                      // latitude
      0.0,                      // longitude
      0.0,                      // horz_accuracy
      0.0,                      // altitude
      0.0,                      // vert_accuracy
      0.0,                      // vertical_speed,
      0.0,                      // horizontal_speed
      0.0,                      // speed_accuracy
      0.0,                      // heading_of_motion
      0.0                       // heading_accuracy
  };

  frame.longitude = -180.1;
  EXPECT_FALSE(data::isGpsFrameValid(frame));
  frame.longitude = -180.0;
  EXPECT_TRUE(data::isGpsFrameValid(frame));
  frame.longitude = 180.0;
  EXPECT_TRUE(data::isGpsFrameValid(frame));
  frame.longitude = 180.1;
  EXPECT_FALSE(data::isGpsFrameValid(frame));
}

/**
 * @brief Verify validity of horizontal accuracy (meters),
 * must be within [0, 1000]
 */
TEST(isGpsFrameValidTest, horzAccuracyValidity) {
  data::GpsFrame frame{
      BoosterSeat::clck::now(), // time now
      data::GpsFix::FIX_3D,     // fix
      10,                       // num_satellites
      0.0,                      // latitude
      0.0,                      // longitude
      0.0,                      // horz_accuracy
      0.0,                      // altitude
      0.0,                      // vert_accuracy
      0.0,                      // vertical_speed,
      0.0,                      // horizontal_speed
      0.0,                      // speed_accuracy
      0.0,                      // heading_of_motion
      0.0                       // heading_accuracy
  };

  frame.horz_accuracy = -0.1;
  EXPECT_FALSE(data::isGpsFrameValid(frame));
  frame.horz_accuracy = 0.0;
  EXPECT_TRUE(data::isGpsFrameValid(frame));
  frame.horz_accuracy = 1000.0;
  EXPECT_TRUE(data::isGpsFrameValid(frame));
  frame.horz_accuracy = 1000.1;
  EXPECT_FALSE(data::isGpsFrameValid(frame));
}

/**
 * @brief Verify validity of altitude (meters),
 * must be within [-450, 40000]
 */
TEST(isGpsFrameValidTest, altitudeValidity) {
  data::GpsFrame frame{
      BoosterSeat::clck::now(), // time now
      data::GpsFix::FIX_3D,     // fix
      10,                       // num_satellites
      0.0,                      // latitude
      0.0,                      // longitude
      0.0,                      // horz_accuracy
      0.0,                      // altitude
      0.0,                      // vert_accuracy
      0.0,                      // vertical_speed,
      0.0,                      // horizontal_speed
      0.0,                      // speed_accuracy
      0.0,                      // heading_of_motion
      0.0                       // heading_accuracy
  };

  frame.altitude = -450.1;
  EXPECT_FALSE(data::isGpsFrameValid(frame));
  frame.altitude = -450.0;
  EXPECT_TRUE(data::isGpsFrameValid(frame));
  frame.altitude = 40000.0;
  EXPECT_TRUE(data::isGpsFrameValid(frame));
  frame.altitude = 40000.1;
  EXPECT_FALSE(data::isGpsFrameValid(frame));
}

/**
 * @brief Verify validity of heading of motion (0-360 degrees)
 */
TEST(isGpsFrameValidTest, headingOfMotionValidity) {
  data::GpsFrame frame{
      BoosterSeat::clck::now(), // time now
      data::GpsFix::FIX_3D,     // fix
      10,                       // num_satellites
      0.0,                      // latitude
      0.0,                      // longitude
      0.0,                      // horz_accuracy
      0.0,                      // altitude
      0.0,                      // vert_accuracy
      0.0,                      // vertical_speed,
      0.0,                      // horizontal_speed
      0.0,                      // speed_accuracy
      0.0,                      // heading_of_motion
      0.0                       // heading_accuracy
  };

  frame.heading_of_motion = -0.1;
  EXPECT_FALSE(data::isGpsFrameValid(frame));
  frame.heading_of_motion = 0.0;
  EXPECT_TRUE(data::isGpsFrameValid(frame));
  frame.heading_of_motion = 360.0;
  EXPECT_TRUE(data::isGpsFrameValid(frame));
  frame.heading_of_motion = 360.1;
  EXPECT_FALSE(data::isGpsFrameValid(frame));
}

/**
 * @warning Not verifying validity of speeds, speed accuracy, or heading
 * accuracy.
 */