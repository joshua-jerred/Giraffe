/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   data_link.cpp
 * @brief  AX.25 Data Link State Machine
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-07-01
 * @copyright  2023 (license to be defined)
 */

#include "data_link.hpp"

namespace ax25 {
void DataLinkStateMachine::state_disconnected() {
  // SR = Signal Reception
  // SG = Signal Generation

  // SR - control field error
  indicate(DataLinkErrorCodes::L);
  // Stay disconnected

  // SR - info not permited in frame
  indicate(DataLinkErrorCodes::M);
  // Stay disconnected

  // incorrect U or S frame length
  indicate(DataLinkErrorCodes::N);
  // Stay disconnected

  // SR - UA
  indicate(DataLinkErrorCodes::C);
  indicate(DataLinkErrorCodes::D);
  // Stay disconnected

  // SR - DM
  // Stay disconnected

  // SR - UI
  subroutine_uiCheck();
  // P = 1?
  // -- No -- Stay disconnected
  // -- Yes -- SG DM F=1, Stay disconnected

  // SR - DL-Disconnect request
  generateSignal(Primitive::DL_DISCONNECT, PrimitiveAction::CONFIRM);
  // Stay disconnected

  // SR - DISC
  // F <-- P
  // SG - DM
  // Stay disconnected

  // SR - DL-Unit-Data request
  // SG - UI command
  // Stay disconnected

  // All other SABM commands
  // F <-- P
  // SG - DM
  // Stay disconnected

  // DL-Release - All other primatives
  // Stay disconnected

  // SR - DL-CONNECT request
  // SAT <-- initial default
  // TIV <-- 2*SAT
  // Subroutine call (establish data link)
  // set layer 3 initiated
  // Transition to 'awaiting connection' state

  // SR - SABM
  // F <-- P
  // Able to establish?
  // -- No -- SG DM, Stay disconnected
  // -- Yes -- Set Version 2.0 (GO TO __UA__ BELOW)

  // SR - SABME
  // F <-- P
  // Able to establish?
  // -- No -- SG DM, Stay disconnected
  // -- Yes -- Set Version 2.2 (GO TO __UA__ BELOW)

  // __UA__
  // Subroutine clear exception conditions
  // V(s) <-- 0
  // V(a) <-- 0
  // V(r) <-- 0
  // SG DL-Connect indication
  // SRT <-- initial default
  // TIV <-- 2*SRT
  // start T3
  // Transition to 'connected' state
}

void DataLinkStateMachine::state_awaitingConnection() {
  // SR - control field error
  indicate(DataLinkErrorCodes::L);
  // Stay in 'awaiting connection' state

  // SR - info not permited in frame
  indicate(DataLinkErrorCodes::M);
  // Stay in 'awaiting connection' state

  // incorrect U or S frame length
  indicate(DataLinkErrorCodes::N);
  // Stay in 'awaiting connection' state

  // SR - DL-CONNECT request
  // discard queue
  // set layer 3 initiated
  // Stay in 'awaiting connection' state

  // SR - DL-DISCONNECT request
  // requeue

  // SR - SABM
  // F <-- P
  // SG UA
  // Stay in 'awaiting connection' state

  // SR - DISC
  // F <-- P
  // SG DM
  // Stay in 'awaiting connection' state

  // SR - DL-DATA request
  // layer 3 initiated?
  // -- NO -- push frame on queue
  // -- YES -- continue
  // Stay in 'awaiting connection' state

  // SR - frame pop off queue
  // layer 3 initiated?
  // -- NO -- push frame on queue
  // -- YES -- continue
  // Stay in 'awaiting connection' state

  // SR - UI
  subroutine_uiCheck();
  // P = 1?
  // -- No -- Stay in 'awaiting connection' state
  // -- Yes -- DM F=1, Stay in 'awaiting connection' state

  // SR - DL-UNIT-DATA request
  // SG UI Command
  // Stay in 'awaiting connection' state

  // all other primatives
  // stay in 'awaiting connection' state

  // all other primatives
  // stay in 'awaiting connection' state

  // SR - DM
  // F=1?
  // -- NO -- Stay in 'awaiting connection' state
  // -- YES --
  // discard queue
  // DL-disconnect indication
  // stop T1
  // transition to 'disconnected' state

  // SR - UA
  // F=1?
  // -- NO -- DL-Error indication D, stay in 'awaiting connection' state
  // -- YES --
  // layer 3 initiated?
  // -- NO -- (look this one up)
  // connected

  // timer T1 expiry
  // disconnected

  // SABME
  // awaiting connect 2.2
}

void DataLinkStateMachine::state_awaitingRelease() {
  // SR - control field error
  indicate(DataLinkErrorCodes::L);
  // Stay in 'awaiting release' state

  // SR - info not permited in frame
  indicate(DataLinkErrorCodes::M);
  // Stay in 'awaiting release' state

  // incorrect U or S frame length
  indicate(DataLinkErrorCodes::N);
  // Stay in 'awaiting release' state

  // SR - DL-DISCONNECT request
  // DM (expedited)
  // stop T1
  // transition to 'disconnected' state

  // SABM
  // ...
  // stay in 'awaiting release' state

  // DISC
  // ...
  // stay in 'awaiting release' state

  // SR - DL-UNIT-DATA
  // ..
  // stay in 'awaiting release' state

  // I, RR, RNR, REJ, SREJ commands
  // ..
  // stay in 'awaiting release' state

  // SR - UI
  subroutine_uiCheck();
  // ..
  // stay in 'awaiting release' state

  // all other primatives
  // ..
  // stay in 'awaiting release' state

  // all other primatives
  // ..
  // stay in 'awaiting release' state

  // UA
  // ..
  // ..

  // DM
  // ..

  // timer T1 expiry
  //
}

void DataLinkStateMachine::state_connected() {
  // SR - control field error
  indicate(DataLinkErrorCodes::L);
  // ...

  // SR - info not permited in frame
  indicate(DataLinkErrorCodes::M);
  // ...

  // incorrect U or S frame length
  indicate(DataLinkErrorCodes::N);
  // ...

  // SR - DL-CONNECT request
  // ...

  // DL-DISCONNECT request
  // ..

  // DL-DATA request
  // ..

  // I frame pop off queue
  // ..

  // timer T1 expiry
  // ..

  // timer T3 expiry
  // ..

  // SR - SABM
  // ..

  // SR - SABME
  // ..

  // SR - DISC
  // ..

  // SR - UA
  // ..

  // SR - DM
  // ..

  // SR - FRMR
  // ..

  // DL-FLOW-OFF request
  // ..

  // DL-FLOW-ON request
  // ..

  // DL-UNIT-DATA request
  // ..

  // SR - UI
  // ..

  // SR - RR
  // ..

  // SR - RNR
  // ..

  // SR - LM-SEIZE confirm
  // ..

  // SR - SREJ
  // ..

  // SR - REJ
  // ..

  // I
  // ..
  ///////////////// weird transition
}

void DataLinkStateMachine::state_timerRecovery() {
  // SR - control field error
  indicate(DataLinkErrorCodes::L);
  // ...

  // SR - info not permited in frame
  indicate(DataLinkErrorCodes::M);
  // ...

  // incorrect U or S frame length
  indicate(DataLinkErrorCodes::N);
  // ...

  // SR - DL-CONNECT request
  // ...

  // DL-DISCONNECT request
  // ..

  // DL-DATA request
  // ..

  // I frame pop off queue
  // ..

  // timer T1 expiry
  // ..

  // SR - SABM
  // ..

  // SR - SABE
  // ..

  // SR - RR
  // ..

  // SR - RNR
  // ..

  // SR - DISC
  // ..

  // SR - UA
  // ..

  // SR - LM-SEIZE confirm
  // ..

  // SR - UI
  // ..

  // SR - DL-UNIT-DATA request
  // ..

  // REJ
  // ..

  // SR - DL-FLOW-OFF request
  // ..

  // SR - DL-FLOW-ON request
  // ..

  // FRMR (Note 1)
  // ..

  // SR - SREJ
  // ..

  // I
  // ......
}

void DataLinkStateMachine::state_awaitingConnectionV2_2() {
  // SR - control field error
  // ..

  // UI

  // DL-UNIT-DATA request

  // all other primatives

  // all other primatives

  // info not permited in frame

  // incorrect U or S frame length

  // SR - DL-CONNECT request

  // SR - DL-DISCONNECT request

  // SABM

  // DISC

  // DL-DATA request

  // I frame pop off queue

  // 1 awaiting connection ENTRY

  // DM

  // UA

  // timer T1 expiry

  // SABME

  // FRMR
}

void DataLinkStateMachine::subroutine_uiCheck() {
}

} // namespace ax25