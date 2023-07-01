/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   management_data_link.cpp
 * @brief  Management Data Link State Machine
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-07-01
 * @copyright  2023 (license to be defined)
 */

#include "management_data_link.hpp"

namespace ax25 {
void ManagementDataLinkStateMachine::state_ready() {
  // SR - MDL-NEGOTIATE request

  // SR - XID command

  // SR - XID response
}

void ManagementDataLinkStateMachine::state_negotiating() {
  // SR - MDL-NEGOTIATE request

  // SR - XID command

  // SR - TM201 expiry

  // SR - XID response

  // SR - FRMR response
}
} // namespace ax25