/**
 * @file units.hpp
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief Basic unit conversions and number formatting.
 * @date 2023-03-12
 * @copyright Copyright (c) 2023
 * @version 0.4
 */

#ifndef UNITS_HPP_
#define UNITS_HPP_

#include <iomanip>
#include <sstream>
#include <string>

namespace units {

inline double MetersToFeet(double meters) { return meters * 3.28084; }

inline double MetersPerSecondToKnots(double meters_per_second) {
  return meters_per_second * 1.94384;
}
} // namespace units

namespace numbers {

inline std::string round(double value, int precision = 2) {
  std::stringstream stream;
  stream << std::fixed << std::setprecision(precision) << value;
  return stream.str();
}

}

#endif