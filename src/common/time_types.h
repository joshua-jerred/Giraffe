/**
 * @file clock.h
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief Standardization of clocks and time.
 * @date 2023-05-15
 * @copyright Copyright (c) 2023
 */

#ifndef TIME_H_
#define TIME_H_

#include <chrono>

namespace giraffe_time {
  typedef std::chrono::high_resolution_clock Clock;
  typedef std::chrono::time_point<giraffe_time::Clock> TimePoint;
  
  typedef std::chrono::duration<int, std::ratio<1>> Seconds;
  typedef std::chrono::duration<int, std::ratio<1, 1000>> Milliseconds;

  inline void start(giraffe_time::TimePoint &time_point) {
    time_point = giraffe_time::Clock::now();
  }

  inline giraffe_time::TimePoint now() {
    return giraffe_time::Clock::now();
  }

  inline int secondsElapsed(giraffe_time::TimePoint &time_point) {
    return std::chrono::duration_cast<giraffe_time::Seconds>(giraffe_time::now() - time_point).count();
  }

  inline int millisecondsElapsed(giraffe_time::TimePoint &time_point) {
    return std::chrono::duration_cast<giraffe_time::Milliseconds>(giraffe_time::now() - time_point).count();
  }

}

#endif