/**
 * @file utility-timer.cpp
 * @author Joshua Jerred (github.com/joshua-jerred)
 * @brief The implementation of the timer class.
 * 
 * @version 0.0.9
 * @date 2022-10-11
 * @copyright Copyright (c) 2022
 */

#include "utility-timer.h"

/**
 * @brief Constructs a new timer object and starts the stopwatch.
 */
Timer::Timer():_begin(_clock::now()) {
}

/** @todo Implement this. */
void Timer::pause() {

}

/** @todo Implement this. */
void Timer::resume() {

}

/**
 * @brief Resets the timer and continues to count.
 */
void Timer::reset() {
    _begin = _clock::now();
}

/**
 * @brief Returns the number of seconds since the timer was started/created/
 * last reset.
 * @return int The number of seconds since the timer has started.
 */
int Timer::elapsed() const { 
    return std::chrono::duration_cast<_second> (_clock::now() - _begin).count();
}