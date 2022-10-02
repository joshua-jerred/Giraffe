/**
 * @todo Implement pause and resume
 */

#include <chrono>

#include "utility-timer.h"

Timer::Timer():_begin(_clock::now()) {
}

/**
 * @bug Not yet implemented
 * 
 */
void Timer::pause() {

}

/**
 * @bug Not yet implemented
 */
void Timer::resume() {

}

void Timer::reset() {
    _begin = _clock::now();
}

int Timer::elapsed() const { 
    return std::chrono::duration_cast<_second> (_clock::now() - _begin).count();
}