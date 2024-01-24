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
  StatusLed() = default;
  ~StatusLed() = default;

  void setRed(bool state) {
    red_ = state;
  }
  void setGreen(bool state) {
    green_ = state;
  }
  void setBlue(bool state) {
    blue_ = state;
  }

  void blinkRed() {
    red_ = !red_;
  }
  void blinkGreen() {
    green_ = !green_;
  }
  void blinkBlue() {
    blue_ = !blue_;
  }

  bool getRed() {
    return red_;
  }
  bool getGreen() {
    return green_;
  }
  bool getBlue() {
    return blue_;
  }

private:
  std::atomic<bool> red_ = false;
  std::atomic<bool> green_ = false;
  std::atomic<bool> blue_ = false;
};

} // namespace giraffe

#endif /* GIRAFFE_STATUS_LED_HPP_ */