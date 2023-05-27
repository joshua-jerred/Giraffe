/**
 * @file clock.h
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief Standardization of clocks and time.
 * @date 2023-05-15
 * @copyright Copyright (c) 2023
 */

#ifndef TIME_TYPES_HPP_
#define TIME_TYPES_HPP_

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>

namespace giraffe_time {
typedef std::chrono::high_resolution_clock Clock;
typedef std::chrono::time_point<giraffe_time::Clock> TimePoint;

typedef std::chrono::duration<int, std::ratio<1>> Seconds;
typedef std::chrono::duration<int, std::ratio<1, 1000>> Milliseconds;

inline void start(giraffe_time::TimePoint &time_point) {
  time_point = giraffe_time::Clock::now();
}

inline giraffe_time::TimePoint now() { return giraffe_time::Clock::now(); }

inline int secondsElapsed(const giraffe_time::TimePoint &time_point) {
  return std::chrono::duration_cast<giraffe_time::Seconds>(giraffe_time::now() -
                                                           time_point)
      .count();
}

inline int millisecondsElapsed(const giraffe_time::TimePoint &time_point) {
  return std::chrono::duration_cast<giraffe_time::Milliseconds>(
             giraffe_time::now() - time_point)
      .count();
}

} // namespace giraffe_time
#endif