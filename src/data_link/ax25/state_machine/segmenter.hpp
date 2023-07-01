/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   segmenter.hpp
 * @brief  Segmenter State Machine
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-07-01
 * @copyright  2023 (license to be defined)
 */

#ifndef SEGMENTER_HPP_
#define SEGMENTER_HPP_

#include "base_ax25_state_machine.hpp"

namespace ax25 {
enum class SegmenterStates { READY = 0 };

enum class SegmenterErrorCodes {
  /**
   * @brief Data too large to segment
   */
  Y
};

enum class SegmenterFlags {

};

enum class SegmenterTimers {

};

class SegmenterStateMachine : public BaseAX25StateMachine {
public:
  SegmenterStateMachine(StateMachineData &data) : BaseAX25StateMachine(data) {
  }
  ~SegmenterStateMachine() = default;

  SegmenterStates getCurrentState() const {
    return state_;
  }
  void receive(Primitive primitive);

private:
  void set_state(SegmenterStates state) {
    state_ = state;
  }

  void indicate(SegmenterErrorCodes error_code);

  void state_ready();

  void subroutine_initiateN1Notification();

  SegmenterStates state_ = SegmenterStates::READY;
};
} // namespace ax25

#endif /* SEGMENTER_HPP_ */