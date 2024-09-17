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

class StatusLedState {
public:
  enum class State { OFF, ON, BLINK };

  StatusLedState() = default;
  ~StatusLedState() = default;

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

class StatusLedChannel {
public:
  enum class Channel { RED, GREEN, BLUE };

  StatusLedChannel(StatusLedState &status_led_state, Channel channel)
      : status_led_state_(status_led_state), channel_(channel) {
  }

  void blink() {
    state_ = StatusLedState::State::BLINK;
    set();
  }

  void on() {
    state_ = StatusLedState::State::ON;
    set();
  }

  void off() {
    state_ = StatusLedState::State::OFF;
    set();
  }

  StatusLedState::State getState() const {
    return state_;
  }

private:
  void set() {
    switch (channel_) {
    case Channel::RED:
      status_led_state_.setRed(state_);
      break;
    case Channel::GREEN:
      status_led_state_.setGreen(state_);
      break;
    case Channel::BLUE:
      status_led_state_.setBlue(state_);
      break;
    }
  }

  StatusLedState &status_led_state_;
  Channel channel_;
  StatusLedState::State state_ = StatusLedState::State::OFF;
};

} // namespace giraffe

#endif /* GIRAFFE_STATUS_LED_HPP_ */