/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   ax25_state_machine.hpp
 * @brief  Defines the AX.25 state machine
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-07-01
 * @copyright  2023 (license to be defined)
 */

#ifndef AX25_STATE_MACHINE_HPP_
#define AX25_STATE_MACHINE_HPP_

#include "state_machine/data_link.hpp"
#include "state_machine/duplex_physical_layer.hpp"
#include "state_machine/link_multiplexer.hpp"
#include "state_machine/management_data_link.hpp"
#include "state_machine/reassembler.hpp"
#include "state_machine/segmenter.hpp"
#include "state_machine/simplex_physical_layer.hpp"
#include "state_machine_data.hpp"

namespace gdl {

class Ax25StateMachine {
public:
  enum class ErrorCodes {

  };

  Ax25StateMachine()
      : sm_data_(), data_link_sm_(sm_data_),
        duplex_physical_layer_sm_(sm_data_), link_multiplexer_sm_(sm_data_),
        reassembler_sm_(sm_data_), segmenter_sm_(sm_data_),
        simplex_physical_layer_sm_(sm_data_) {
  }

  ~Ax25StateMachine() = default;

  void start();
  void stop();

private:
  ax25::StateMachineData sm_data_;

  ax25::DataLinkStateMachine data_link_sm_;
  ax25::DuplexPhysicalLayerStateMachine duplex_physical_layer_sm_;
  ax25::LinkMultiplexerStateMachine link_multiplexer_sm_;
  ax25::ReassemblerStateMachine reassembler_sm_;
  ax25::SegmenterStateMachine segmenter_sm_;
  ax25::SimplexPhysicalLayerStateMachine simplex_physical_layer_sm_;
};

} // namespace gdl

#endif /* AX25_STATE_MACHINE_HPP_ */