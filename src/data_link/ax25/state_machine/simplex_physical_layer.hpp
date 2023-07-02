/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   simplex_physical_layer.hpp
 * @brief  AX.25 Simplex Physical Layer State Machine
 * @details See C2a of the AX.25 Specification.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-07-01
 * @copyright  2023 (license to be defined)
 */

#ifndef SIMPLEX_PHYSICAL_LAYER_HPP_
#define SIMPLEX_PHYSICAL_LAYER_HPP_

#include "base_ax25_state_machine.hpp"

namespace ax25 {

enum class SimplexPhysicalLayerFlags {
  /**
   * @brief Set when this transmission is for digipeating frames. Cleared when
   * this transmission is for normal frames.
   */
  DIGIPEATING,

  /**
   * @brief Set when repeater is expected to still be transmitting. Cleared when
   * repeater carrier is expected to have dropped.
   */
  REPEATER_UP,

  /**
   * @brief Set when anti-hogging or 10-minute transmitter limits have
   * interrupted the transmission of normal frames.
   */
  INTERRUPTED,

  /**
   * @brief p-persistence value, in the range 0-1
   */
  P
};

class SimplexPhysicalLayerStateMachine : public BaseAX25StateMachine {
public:
  enum class State {
    READY = 0,
    RECEIVING = 1,
    TRANSMITTER_SUPPRESSION = 2,
    TRANSMITTER_START = 3,
    TRANSMITTING = 4,
    DIGIPEATING = 5,
    RECEIVER_START = 6
  };

  SimplexPhysicalLayerStateMachine(StateMachineData &data)
      : BaseAX25StateMachine(data) {
  }
  ~SimplexPhysicalLayerStateMachine() = default;

  State getCurrentState() const {
    return state_;
  }
  void receive(Primitive primitive);

private:
  void set_state(State state) {
    state_ = state;
  }

  void state_ready();
  void state_receiving();
  void state_transmitterSuppression();
  void state_transmitterStart();
  void state_transmitting();
  void state_digipeating();
  void state_receiverStart();

  void subroutine_startTransmitter();
  void subroutine_acquisition();

  // To the radio
  bool turnTransmitterOn();
  bool turnTransmitterOff();
  void sendFrameToRadio();

  // From the radio
  bool acquisitionOfSignal();
  bool lossOfSignal();
  void getFrameFromRadio();

  State state_ = State::READY;

  /**
   * @brief Queue of information to be transmitted in I frames.
   */
  // PriorityQueue;
  // NormalQueue;
};
} // namespace ax25

#endif /* SIMPLEX_PHYSICAL_LAYER_HPP_ */
