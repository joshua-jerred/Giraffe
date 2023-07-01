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
enum class SimplexPhysicalLayerStates {
  READY = 0,
  RECEIVING = 1,
  TRANSMITTER_SUPPRESSION = 2,
  TRANSMITTER_START = 3,
  TRANSMITTING = 4,
  DIGIPEATING = 5,
  RECEIVER_START = 6
};

enum class SimplexPhysicalLayerErrorCodes {
  // none used
};

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

enum class SimplexPhysicalLayerTimers {
  T100, // Repeater Hand (AXHANG)
  T101, // Priority Window (PRIACK)
  T102, // Slot Time (p-persistence)
  T103, // Transmitter Startup (TXDELAY)
  T104, // Repeater Startup (AXDELAY)
  T105, // Remote Receiver Sync
  T106, // Ten Minute Transmission Limit
  T107, // Anti-Hogging Limit
  T108  // Receiver Startup
};

class SimplexPhysicalLayerStateMachine : public BaseAX25StateMachine {
public:
  SimplexPhysicalLayerStateMachine(StateMachineData &data)
      : BaseAX25StateMachine(data) {
  }
  ~SimplexPhysicalLayerStateMachine() = default;

  SimplexPhysicalLayerStates getCurrentState() const {
    return state_;
  }
  void receive(Primitive primitive);

private:
  void set_state(SimplexPhysicalLayerStates state) {
    state_ = state;
  }

  void indicate(SimplexPhysicalLayerErrorCodes error_code);

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

  SimplexPhysicalLayerStates state_ = SimplexPhysicalLayerStates::READY;

  /**
   * @brief Queue of information to be transmitted in I frames.
   */
  // PriorityQueue;
  // NormalQueue;
};
} // namespace ax25

#endif /* SIMPLEX_PHYSICAL_LAYER_HPP_ */
