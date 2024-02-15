/**
 * @file timer.h
 * @author Joshua Jerred (https://joshuajer.red/)
 * @brief The declaration of the timer class.
 *
 * @version 0.0.9
 * @date 2022-10-11
 * @copyright Copyright (c) 2022
 */

#ifndef UTILITY_TIMER_H_
#define UTILITY_TIMER_H_

#include <chrono>

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
