/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   base_ax25_state_machine.hpp
 * @brief  Base AX.25 State Machine Class
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-07-01
 * @copyright  2023 (license to be defined)
 */

#ifndef BASE_AX25_STATE_MACHINE_HPP_
#define BASE_AX25_STATE_MACHINE_HPP_

#include "state_machine_data.hpp"

namespace ax25 {

class BaseAX25StateMachine {
public:
  BaseAX25StateMachine(StateMachineData &data) : data_(data) {
  }

protected:
  void generateSignal(Primitive primitive);
  void indicateError(StateMachineErrors error_code, bool active);
  // void indicate(ErrorCode error_code);

  StateMachineData &data_;
};

} // namespace ax25

#endif /* BASE_AX25_STATE_MACHINE_HPP_ */