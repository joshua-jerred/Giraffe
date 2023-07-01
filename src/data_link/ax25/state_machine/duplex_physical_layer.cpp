/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   duplex_phsyical_layer.cpp
 * @brief  Duplex Physical Layer State Machine
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-07-01
 * @copyright  2023 (license to be defined)
 */

#include "duplex_physical_layer.hpp"

namespace ax25 {

void DuplexPhysicalLayerStateMachine::state_receiverReady() {
  // SR - acquisition of signal

  // SR - all other primitives
}

void DuplexPhysicalLayerStateMachine::state_receiving() {
  // SR - frame

  // SR - loss of signal
}

void DuplexPhysicalLayerStateMachine::state_transmitterReady() {
  // SR - PH-SEIZE request

  // SR - PH-RELEASE request

  // SR - all other primitives
}

void DuplexPhysicalLayerStateMachine::state_transmitterStart() {
  // SR - T105 expiry

  // SR - all other primitives
}

void DuplexPhysicalLayerStateMachine::state_transmitting() {
  // SR - PH-SEIZE request

  // SR - PH-RELEASE request

  // SR - T107 expiry

  // SR - PH-DATA request

  // SR - PH-EXPEDITED-DATA request

  // SR - all other primitives
}

} // namespace ax25