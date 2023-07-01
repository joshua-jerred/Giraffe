/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   data_link_state_machine.hpp
 * @brief  AX.25 Data Link Layer State Machine
 * @details See C4.2 of the AX.25 Specification.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-07-01
 * @copyright  2023 (license to be defined)
 */

#include "primatives.hpp"

namespace ax25 {
enum class DataLinkStates {
  DISCONNECTED,
  AWAITING_CONNECTION,
  AWAITING_RELEASE,
  CONNECTED,
  TIMER_RECOVERY
};

enum class DataLinkErrorCodes {
  A, // F=1 received but P=1 not outstanding
  B, // Unexpected DM with F=1 in states 3, 4, or 5
  C, // Unexpected UA in states 3, 4, or 5
  D, // UA received without F=1 when SABM or DISC was sent P=1
  E, // DM received in states 3, 4, or 5
  F, // Data link reset; i.e., SABM received in state 3, 4, or 5
  I, // N2 timeouts; unacknowledged data
  J, // N(r) sequence error
  L, // Control field error
  M, // Information field was received in a U or S frame
  N, // Length of frame incorrect for frame type
  O, // I frame exceeded maximum allowed length
  P, // N(s) out of the window
  Q, // UI response received, or UI command with P=1 received
  R, // UI frame exceeded maximum allowed length
  S, // I response received
  T, // N2 timeouts; no response to enquiry
  U, // N2 timeouts; extended peer busy condition
  V  // No DL machines available to establish connection
};

enum class DataLinkFlags {
  LAYER_3_INITIATED,  // SABM was sent by request of Layer 3; i.e., DL-CONNECT
                      // Request primitive.
  PEER_RECEIVER_BUSY, // Remote station is busy and cannot receive I frames.
  OWN_RECEIVER_BUSY,  // Layer 3 is busy and cannot receive I frames.
  REJECT_EXCEPTION,   // a REJ frame has been sent to the remote station
  SELECTIVE_REJECT_EXCEPTION, // A SREJ frame has been sent to the remote
                              // station.
  ACKNOWLEDGE_PENDING, // I frames have been successfully received but not yet
                       // acknowledged to the remote station.
  SRT,                 // Smoothed round trip time
  T1V, // Next value for T1; default initial value is initial value of SRT
  N1, // Maximum number of octets in the information field of a frame, excluding
      // inserted 0-bits
  N2, // Maximum number of retries permitted.
};

enum class DataLinkTimers {
  T1, // Outstanding I frame or P-bit
  T3  // Idle supervision (keep alive)
};

class DataLinkStateMachine {
public:
  DataLinkStateMachine() = default;
  ~DataLinkStateMachine() = default;

  DataLinkStates getCurrentState() const {
    return state_;
  }
  void receive(Primitive primitive);

private:
  void set_state(DataLinkStates state) {
    state_ = state;
  }

  void generateSignal(Primitive primitive, PrimitiveAction action);
  void indicate(DataLinkErrorCodes error_code);

  void state_disconnected();
  void state_awaitingConnection();
  void state_awaitingRelease();
  void state_connected();
  void state_timerRecovery();
  void state_awaitingConnectionV2_2();

  void subroutine_NrErrorRecovery();
  void subroutine_establishDataLink();
  void subroutine_clearExceptionConditions();
  void subroutine_transmitEnquiry();
  void subroutine_enquiryResponse();
  void subroutine_invokeRetransmission();
  void subroutine_checkIFrameAcked();
  void subroutine_checkNeedForResponse();
  void subroutine_selectT1Value();
  void subroutine_establishExtendedDataLink();
  void subroutine_setVersion2_0();
  void subroutine_setVersion2_2();
  void subroutine_uiCheck();

  DataLinkStates state_ = DataLinkStates::DISCONNECTED;

  /**
   * @brief Queue of information to be transmitted in I frames.
   */
  // IFrameQueue i_frame_queue_;
};
} // namespace ax25