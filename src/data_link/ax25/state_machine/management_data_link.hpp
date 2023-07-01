/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   management_data_link.hpp
 * @brief  Management Data Link State Machine
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-07-01
 * @copyright  2023 (license to be defined)
 */

#ifndef MANAGEMENT_DATA_LINK_HPP_
#define MANAGEMENT_DATA_LINK_HPP_

#include "base_ax25_state_machine.hpp"

namespace ax25 {
enum class ManagementDataLinkStates {
  READY = 0,
  NEGOTIATING = 1,
};

enum class ManagementDataLinkFlags {
  /**
   * @brief Retry count
   */
  RC,

  /**
   * @brief Maximum number of retries of the XID command
   */
  NM201
};

enum class ManagementDataLinkTimers {
  /**
   * @brief Retry timer for management functions.
   */
  TM201
};

class ManagementDataLinkStateMachine : public BaseAX25StateMachine {
public:
  ManagementDataLinkStateMachine(StateMachineData &data)
      : BaseAX25StateMachine(data) {
  }
  ~ManagementDataLinkStateMachine() = default;

  ManagementDataLinkStates getCurrentState() const {
    return state_;
  }
  void receive(Primitive primitive);

private:
  void set_state(ManagementDataLinkStates state) {
    state_ = state;
  }

  void state_ready();
  void state_negotiating();

  void subroutine_initiateN1Notification();
  void subroutine_N1NotificationResponse();
  void subroutine_CompleteN1Notification();
  void subroutine_initiateWindowNotification();
  void subroutine_WindowNotificationResponse();
  void subroutine_CompleteWindowNotification();
  void subroutine_initiateT1Negotiation();
  void subroutine_T1NegotiationResponse();
  void subroutine_CompleteT1Negotiation();
  void subroutine_initiateRetryNegotiation();
  void subroutine_RetryNegotiationResponse();
  void subroutine_CompleteRetryNegotiation();
  void subroutine_initiateOptionalFunctionsNegotiation();
  void subroutine_OptionalFunctionsNegotiationResponse();
  void subroutine_CompleteOptionalFunctionsNegotiation();
  void subroutine_initiateClassesOfProceduresNegotiation();
  void subroutine_ClassesOfProceduresNegotiationResponse();
  void subroutine_CompleteClassesOfProceduresNegotiation();

  ManagementDataLinkStates state_ = ManagementDataLinkStates::READY;

  // AwaitingQueue;
  // CurrentQueue;
  // ServedQueue;
};
} // namespace ax25

#endif /* MANAGEMENT_DATA_LINK_HPP_ */