/**
 * @file utility-timer.h
 * @author Joshua Jerred (github.com/joshua-jerred)
 * @brief The declaration of the timer class.
 * 
 * @version 0.0.9
 * @date 2022-10-11
 * @copyright Copyright (c) 2022
 */

#ifndef UTILITY_TIMER_H_
#define UTILITY_TIMER_H_

#include <chrono>

/**
 * @brief A simple timer class that can be used to time events.
 * @todo Isn't this what a stopwatch is called? I'm not timing anythging.
 */
class Timer
{
public:
    Timer();
    void pause();
    void resume();
    void reset();
    int elapsed() const;

private:
    typedef std::chrono::high_resolution_clock _clock;
    typedef std::chrono::duration<int, std::ratio<1> > _second;
    std::chrono::time_point<_clock> _begin;
};

#endif // UTILITY_TIMER_H_
