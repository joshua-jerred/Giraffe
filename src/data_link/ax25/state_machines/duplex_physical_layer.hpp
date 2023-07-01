/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   duplex_physical_layer.hpp
 * @brief
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-07-01
 * @copyright  2023 (license to be defined)
 */

#ifndef DUPLEX_PHYSICAL_LAYER_HPP_
#define DUPLEX_PHYSICAL_LAYER_HPP_

#include "primatives.hpp"

namespace ax25 {
enum class DuplexPhysicalLayerStates {
  RECEIVER_READY = 0,
  RECEIVING = 1,
  TRANSMITTER_READY = 2,
  TRANSMITTER_START = 3,
  TRANSMITTING = 4
};

enum class DuplexPhysicalLayerErrorCodes {
  // none used
};

enum class DuplexPhysicalLayerFlags { Interrupted };

enum class DuplexPhysicalLayerTimers {
  T105, // remove receiver sync
  T106, // 10-minute transmission limit
  T107  // anti-hogging limit
};

class DuplexPhysicalLayerStateMachine {
public:
  DuplexPhysicalLayerStateMachine() = default;
  ~DuplexPhysicalLayerStateMachine() = default;

  DuplexPhysicalLayerStates getCurrentState() const {
    return state_;
  }
  void receive(Primitive primitive);

private:
  void set_state(DuplexPhysicalLayerStates state) {
    state_ = state;
  }

  void generateSignal(Primitive primitive, PrimitiveAction action);
  void indicate(DuplexPhysicalLayerErrorCodes error_code);

  void state_receiverReady();
  void state_receiving();
  void state_transmitterReady();
  void state_transmitterStart();
  void state_transmitting();

  void subroutine_startTransmitter();
  void subroutine_acquisition();

  // To the radio
  bool turnTransmitterOn();
  bool turnTransmitterOff();
  void frame();

  // From the radio
  bool acquisitionOfSignal();
  bool lossOfSignal();
  void frame();

  DuplexPhysicalLayerStates state_ = DuplexPhysicalLayerStates::RECEIVER_READY;

  // NormalQueue;
};
} // namespace ax25

#endif /* DUPLEX_PHYSICAL_LAYER_HPP_ */