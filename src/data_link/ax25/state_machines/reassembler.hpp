/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   reassembler.hpp
 * @brief
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-07-01
 * @copyright  2023 (license to be defined)
 */

#ifndef REASSEMBLER_HPP_
#define REASSEMBLER_HPP_

#include "primatives.hpp"

namespace ax25 {
enum class ReassemblerStates {
  NULL = 0,
  REASSEMBLING_DATA = 1,
  REASSEMBLING_UNIT_DATA = 2
};

enum class ReassemblerErrorCodes {
  /**
   * @brief Reassembly error
   */
  Z
};

enum class ReassemblerFlags {
  /**
   * @brief number of segments remaining to be reassembled
   */
  N
};

enum class ReassemblerTimers {
  /**
   * @brief time limit for receipt of next segment
   */
  TR210
};

class ReassemblerStateMachine {
public:
  ReassemblerStateMachine() = default;
  ~ReassemblerStateMachine() = default;

  ReassemblerStates getCurrentState() const {
    return state_;
  }
  void receive(Primitive primitive);

private:
  void set_state(ReassemblerStates state) {
    state_ = state;
  }

  void generateSignal(Primitive primitive, PrimitiveAction action);
  void indicate(ReassemblerStates error_code);

  void state_null();
  void state_reassemblingData();
  void state_reassemblingUnitData();

  void subroutine_initiateN1Notification();

  ReassemblerStates state_ = ReassemblerStates::NULL;
};
} // namespace ax25

#endif /* REASSEMBLER_HPP_ */