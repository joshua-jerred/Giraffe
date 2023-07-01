/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   reassembler.cpp
 * @brief
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-07-01
 * @copyright  2023 (license to be defined)
 */

#include "reassembler.hpp"

namespace ax25 {

void ReassemblerStateMachine::state_null() {
  // RS - All other DL primitives

  // RS - DL-DATA indication

  // RS - DL-UNIT-DATA indication
}

void ReassemblerStateMachine::state_reassemblingData() {
  // RS - TR210 expiry

  // RS - DL-DATA indication

  // RS - all other DL primitives
}

void ReassemblerStateMachine::state_reassemblingUnitData() {
  // RS - TR210 expiry

  // RS - DL-UNIT-DATA indication

  // RS - all other DL primitives
}

} // namespace ax25