/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   link_multiplexer.cpp
 * @brief  Link Multiplexer State Machine
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-07-01
 * @copyright  2023 (license to be defined)
 */

#include "link_multiplexer.hpp"

namespace ax25 {

void LinkMultiplexerStateMachine::state_idle() {
  // SR - LM-EXPEDITED-DATA request

  // any other LM primitives

  // SR - LM-DATA request

  // SR - LM-SEIZE request

  // SR - LM-RELEASE request

  // SR - queue empty

  // SR - PH-DATA indication

  // SR - PH-BUSY indication

  // SR - PH-QUIET indication
}

void LinkMultiplexerStateMachine::state_seizePending() {
  // SR - LM-EXPEDITED-DATA request

  // SR - any other LM primitives

  // SR - LM-DATA request

  // SR - LM-SEIZE request

  // SR - LM-RELEASE request

  // SR - queue empty

  // SR - PH-SEIZE confirm

  // SR - PH-DATA indication

  // SR - PH-BUSY indication

  // SR - PH-QUIET indication
}

void LinkMultiplexerStateMachine::state_seized() {
  // SR - LM-EXPEDITED-DATA request

  // SR - any other LM primitives

  // SR - LM-DATA request

  // SR - LM-SEIZE request

  // SR - LM-RELEASE request

  // SR - queue empty

  // SR - PH-SEIZE confirm

  // SR - PH-DATA indication

  // SR - PH-BUSY indication

  // SR - PH-QUIET indication
}

} // namespace ax25