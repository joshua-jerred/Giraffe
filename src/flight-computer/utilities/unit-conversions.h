/**
 * @file unit-conversions.h
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief Basic unit conversions.
 * @date 2023-03-12
 * @copyright Copyright (c) 2023
 * @version 0.4
 */

#ifndef UNIT_CONVERSIONS_H_
#define UNIT_CONVERSIONS_H_

namespace unit_conversions {

inline double MetersToFeet(double meters) {
  return meters * 3.28084;
}

inline double MetersPerSecondToKnots(double meters_per_second) {
  return meters_per_second * 1.94384;
}

} // namespace unit_conversions

#endif // UNIT_CONVERSIONS_H_