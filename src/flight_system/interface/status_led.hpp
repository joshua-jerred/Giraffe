/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   status_led.hpp
 * @brief  Status LED control
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-01-24
 * @copyright  2024 (license to be defined)
 */

#ifndef GIRAFFE_STATUS_LED_HPP_
#define GIRAFFE_STATUS_LED_HPP_

#include <atomic>

namespace giraffe {

class StatusLed {
public:
  enum class State { OFF, ON, BLINK };

  StatusLed() = default;
  ~StatusLed() = default;

  void setRed(State state) {
    r_state_ = state;
  }
  void setGreen(State state) {
    g_state_ = state;
  }
  void setBlue(State state) {
    b_state_ = state;
  }

  State getRedState() const {
    return r_state_;
  }

  State getGreenState() const {
    return g_state_;
  }

  State getBlueState() const {
    return b_state_;
  }

private:
  std::atomic<State> r_state_ = State::OFF;
  std::atomic<State> g_state_ = State::OFF;
  std::atomic<State> b_state_ = State::OFF;
};

} // namespace giraffe

#endif /* GIRAFFE_STATUS_LED_HPP_ */