/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   link_multiplexer.hpp
 * @brief  State Machine for the Link Multiplexer
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-07-01
 * @copyright  2023 (license to be defined)
 */

#ifndef LINK_MULTIPLEXER_HPP_
#define LINK_MULTIPLEXER_HPP_

#include "primatives.hpp"

namespace ax25 {
enum class LinkMultiplexerStates {
  IDLE = 0,
  SEIZE_PENDING = 1,
  SEIZED = 2,
};

enum class LinkMultiplexerErrorCodes {
  // none used
};

enum class LinkMultiplexerFlags {
  /**
   * @brief Identifies the Data-link State Machine currently using the
   * transmitter.
   */
  CURRENT_DL,

  /**
   * @brief Identifies the Data-link State Machines that have already used the
   * transmitter.  This list is cleared when all Data-link State Machines with
   * frames to send have been served.
   *
   */
  SERVED_LIST
};

enum class LinkMultiplexerTimers {
  // none used
};

class LinkMultiplexerStateMachine {
public:
  LinkMultiplexerStateMachine() = default;
  ~LinkMultiplexerStateMachine() = default;

  LinkMultiplexerStates getCurrentState() const {
    return state_;
  }
  void receive(Primitive primitive);

private:
  void set_state(LinkMultiplexerStates state) {
    state_ = state;
  }

  void generateSignal(Primitive primitive, PrimitiveAction action);
  void indicate(LinkMultiplexerErrorCodes error_code);

  void state_idle();
  void state_seizePending();
  void state_seized();

  void subroutine_frameReceived();
  void subroutine_finishCurrentTransmission();
  void subroutine_queueEvent();

  LinkMultiplexerStates state_ = LinkMultiplexerStates::IDLE;

  // AwaitingQueue;
  // CurrentQueue;
  // ServedQueue;
};
} // namespace ax25

#endif /* LINK_MULTIPLEXER_HPP_ */