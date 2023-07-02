/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   reassembler.hpp
 * @brief  Reassembler State Machine
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-07-01
 * @copyright  2023 (license to be defined)
 */

#ifndef REASSEMBLER_HPP_
#define REASSEMBLER_HPP_

#include "base_ax25_state_machine.hpp"

namespace ax25 {

enum class ReassemblerStates {
  NULL_STATE = 0,
  REASSEMBLING_DATA = 1,
  REASSEMBLING_UNIT_DATA = 2
};

enum class ReassemblerFlags {
  /**
   * @brief number of segments remaining to be reassembled
   */
  N
};

class ReassemblerStateMachine : public BaseAX25StateMachine {
public:
  ReassemblerStateMachine(StateMachineData &data) : BaseAX25StateMachine(data) {
  }
  ~ReassemblerStateMachine() = default;

  ReassemblerStates getCurrentState() const {
    return state_;
  }
  void receive(Primitive primitive);

private:
  void set_state(ReassemblerStates state) {
    state_ = state;
  }

  void state_null();
  void state_reassemblingData();
  void state_reassemblingUnitData();

  void subroutine_initiateN1Notification();

  ReassemblerStates state_ = ReassemblerStates::NULL_STATE;
};
} // namespace ax25

#endif /* REASSEMBLER_HPP_ */