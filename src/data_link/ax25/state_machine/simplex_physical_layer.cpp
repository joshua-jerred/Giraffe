/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   simplex_physical_layer.cpp
 * @brief  AX.25 Simplex Physical Layer State Machine
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-07-01
 * @copyright  2023 (license to be defined)
 */

#include "simplex_physical_layer.hpp"

namespace ax25 {

void SimplexPhysicalLayerStateMachine::state_ready() {
  if (isSignalPresent(Primitive::PH_EXPEDITED_DATA_request)) {
    // add frame to priority queue
    // set digipeating
    // start transmitter
    set_state(States::TRANSMITTER_START);
    return;
  }

  // SR - all other primitives
  // add to normal queue
  // keep state the same

  if (isSignalPresent(Primitive::PH_SEIZE_request)) {
    // clear digipeating
    // start transmitter
    set_state(States::TRANSMITTER_START);
    return;
  }

  if (isSignalPresent(Primitive::PH_RELEASE_request)) {
    // keep state the same
    return;
  }

  if (isSignalPresent(Primitive::PD_DATA_request)) {
    // discard erroneous primitive
    return;
  }

  // SR - T102 expiry
  if (data_.timers.t102.expired()) {
    // clear repeater up
    // keep state the same
    return;
  }

  // SR - Acquisition of signal

  // SR - All other primitives
}

void SimplexPhysicalLayerStateMachine::state_receiving() {
  // SR - PH-EXPEDITED-DATA request

  // SR - all other primitives

  // SR - Frame

  // SR - Loss of signal
}

void SimplexPhysicalLayerStateMachine::state_transmitterSuppression() {
  // SR - PH-EXPEDITED-DATA request

  // SR - all other primitives

  // SR - T100 expiry

  // SR - acquisition of signal

  // SR - all other primitives

  // SR - T102 expiry

  // SR - T101 expiry
}

void SimplexPhysicalLayerStateMachine::state_transmitterStart() {
  // SR - PH-EXPEDITED-DATA request

  // SR - all other primitives

  // SR - T100 expiry

  // SR - T104 expiry

  // SR - T103 expiry

  // SR - T105 expiry
}

void SimplexPhysicalLayerStateMachine::state_transmitting() {
  // SR - PH-EXPEDITED-DATA request

  // SR - all other primitives

  // SR - PH-SEIZE-REQUEST

  // SR - frame

  // T107 expiry

  // PH-RELEASE-REQUEST

  // T106 expiry
}

void SimplexPhysicalLayerStateMachine::state_digipeating() {
  // SR - PH-EXPEDITED-DATA request

  // SR - all other primitives

  // SR - priority frame

  // SR - priority queue empty

  // SR - T106 expiry
}

void SimplexPhysicalLayerStateMachine::state_receiverStart() {
  // SR - PH-EXPEDITED-DATA request

  // SR - all other primitives

  // SR - T108 expiry
}

} // namespace ax25