/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   data_link.hpp
 * @brief  AX.25 Data Link Layer State Machine
 * @details See C4.2 of the AX.25 Specification.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-07-01
 * @copyright  2023 (license to be defined)
 */

#include "base_ax25_state_machine.hpp"

namespace ax25 {
enum class DataLinkStates {
  DISCONNECTED,
  AWAITING_CONNECTION,
  AWAITING_RELEASE,
  CONNECTED,
  TIMER_RECOVERY
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

class DataLinkStateMachine : public BaseAX25StateMachine {
public:
  DataLinkStateMachine(StateMachineData &data) : BaseAX25StateMachine(data) {
  }
  ~DataLinkStateMachine() = default;

  DataLinkStates getCurrentState() const {
    return state_;
  }
  void receive(Primitive primitive);

private:
  void set_state(DataLinkStates state) {
    state_ = state;
  }

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