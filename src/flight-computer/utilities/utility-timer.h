#ifndef _TIMER_H_
#define _TIMER_H_

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

#endif