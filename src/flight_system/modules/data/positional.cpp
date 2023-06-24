/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   positional.cpp
 * @brief  Positional data for the flight system (gps/imu data) handling.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-06-23
 * @copyright  2023 (license to be defined)
 */

#include "positional.hpp"

bool data::isGpsFrameValid(const data::GpsFrame &frame) {
  (void)frame;
  return true;
}