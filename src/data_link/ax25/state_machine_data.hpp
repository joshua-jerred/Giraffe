/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   state_machine_data.hpp
 * @brief  Types and data that is shared between the state machines.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-07-01
 * @copyright  2023 (license to be defined)
 */

#ifndef STATE_MACHINE_DATA_HPP_
#define STATE_MACHINE_DATA_HPP_

#include <BoosterSeat/timer.hpp>

namespace ax25 {

/**
 * @brief Timers for the state machines. See 6.7 of the AX.25 2.2 specification.
 */
struct Timers {
  /**
   * @brief Acknowledgement Timer (T1)
   * @details Ensures that the TNC does not wait indefinitely for a response to
   * a frame it sends.
   */
  BoosterSeat::Timer t1{};

  /**
   * @brief Response Delay Timer (T2)
   * @details Optionally implemented by a TNC to specify a maximum amount of
   * time delay to be introduces between the time an I frame is received and the
   * time the resulting response is sent.
   */
  BoosterSeat::Timer t2{};

  /**
   * @brief Inactive Link Timer (T3)
   * @details Maintains the link integrity whenever T1 is not running.
   */
  BoosterSeat::Timer t3{};

  /**
   * @brief Repeater Hand Timer (T100/AXHANG)
   * @details Tracks the amount of time an audio repeater will keep it's
   * transmitter keyed after it stops receiving a signal.
   */
  BoosterSeat::Timer t100{};

  /**
   * @brief Priority Window Timer (T101/PRIACK)
   * @details Prevents stations from transmitting non-priority frames during the
   * first available transmission time slow. The first slow is reserved for
   * priority frames (acks, digipeat)
   */
  BoosterSeat::Timer t101{};

  /**
   * @brief Slot Timer Timer (T102/p-persistence)
   * @details Randomly delays stations before they transmit a frame to help
   * prevent collisions.
   */
  BoosterSeat::Timer t102{};

  /**
   * @brief Transmitter Startup Timer (T103/TXDELAY)
   * @details Allows time for a transmitter to stabilize before transmitting.
   */
  BoosterSeat::Timer t103{};

  /**
   * @brief Repeater Startup Timer (T104/AXDELAY)
   * @details Allows time for a repeater to start up before transmitting.
   */
  BoosterSeat::Timer t104{};

  /**
   * @brief Remote Receiver Sync Timer (T105)
   * @details Introduces additional delay after TXDELAY, if needed, to allow a
   * remote receiver to sync up before transmitting frames.
   */
  BoosterSeat::Timer t105{};

  /**
   * @brief Ten Minute Transmission Timer (T106)
   * @details Ensures that a station is not keyed for more than 10 minutes.
   */
  BoosterSeat::Timer t106{};

  /**
   * @brief Anti-Hogging Timer (T107)
   * @details Prevents a station from monopolizing the channel.
   */
  BoosterSeat::Timer t107{};

  /**
   * @brief Receiver Startup Timer (T108)
   * @details Allows time for a receiver to start up after transmitting to
   * ensure that the frequency is clear before transmitting again.
   */
  BoosterSeat::Timer t108{};

  /**
   * @brief Next Segment Timer (TR210)
   * @details Ensures that the reassembler doesn't wait forever for the next
   * segment.
   */
  BoosterSeat::Timer tr210{};
};

enum class Primitive {
  // Link Multiplexer <--> Physical State Machine

  /**
   * @brief The link multiplexer uses this before each transmission to request
   * access to the radio channel.
   */
  PH_SEIZE_request,

  /**
   * @brief The physical state machine uses this to confirm that the channel has
   * been seized.
   */
  PH_SEIZE_confirm,

  /**
   * @brief The link multiplexer state machine uses this to release the radio
   * channel.
   */
  PH_RELEASE_request,

  /**
   * @brief The physical state machine uses this to indicate that the channel is
   * not busy.
   */
  PH_QUIET_indication,

  /**
   * @brief The Link Multiplexer uses this to request transmission of each
   * digipeat or expedite data frame.
   */
  PH_EXPEDITED_DATA_request,

  /**
   * @brief The link multiplexer state machine uses this to request transmission
   * of each normal frame.
   */
  PH_DATA_request,

  /**
   * @brief The physical state machine uses this primitive to provide incoming
   * frames to the link multiplexer.
   */
  PD_DATA_indication,

  /**
   * @brief The physical state machine uses this to indicate that the channel is
   * busy.
   */
  PH_BUSY_indication,

  // Layer 3 <--> DL (Data-link state machine)

  /**
   * @brief Layer 3 uses this to request the establishment of a AX.25
   * connection.
   */
  DL_CONNECT_request,

  /**
   * @brief The DL state machine uses this to indicate that a AX.25 connection
   * has been requested.
   */
  DL_CONNECT_indication,

  /**
   * @brief The DL State machine uses this to indicate that a AX.25 connection
   * has been made.
   */
  DL_CONNECT_confirm,

  /**
   * @brief The layer 3 entity uses this to request the release of a AX.25
   * connection.
   */
  DL_DISCONNECT_request,

  /**
   * @brief The DL state machine uses this to indicate that a AX.25 connection
   * has been released.
   */
  DL_DISCONNECT_indication,

  /**
   * @brief The DL state machine uses this to indicate that a AX.25 connection
   * has been released and confirmed.
   */
  DL_DISCONNECT_confirm,

  /**
   * @brief The layer 3 entity uses this to request the transmission of data
   * using a connection-oriented protocol.
   */
  DL_DATA_request,

  /**
   * @brief The reassembler uses this primitive to indicate reception of Layer 3
   * data using a connection-oriented protocol.
   */
  DL_DATA_indication,

  /**
   * @brief The layer 3 entity uses this primitive to request the transmission
   * of data using a connection-less protocol. The segmenter may segment the
   * data.
   */
  DL_UNIT_DATA_request,

  /**
   * @brief The reassembler uses this primitive to indicate reception of Layer 3
   * data using a connection-less protocol.
   */
  DL_UNIT_data_indication,

  /**
   * @brief The DL state machine uses this to indicate when frames inconsistent
   * with this protocol definition have been received.
   */
  DL_ERROR_indication,

  /**
   * @brief The layer 3 entity uses this to temporarily suspend the flow of
   * incoming information.
   */
  DL_FLOW_OFF_request,

  /**
   * @brief Layer 3 uses this to resume the flow of incoming information.
   */
  DL_FLOW_ON_request,
  // LM = Link Multiplexer

  // MDL State Machine <--> LM (Link Multiplexer State Machine)

  /**
   * @brief The MDL uses this to pass frames of any type to the LM state
   * machine.
   */
  LM_DATA_request,

  /**
   * @brief The LM state machine uses this to pass frames of any time to the MDL
   * state machine.
   */
  LM_DATA_indication,

  /**
   * @brief The DL state machine uses this to request the link multiplexer state
   * machine to arrange for transmission at the next available opportunity.
   * The DL state machine uses this when an acknowledgement must be made.
   */
  LM_SEIZE_request,

  /**
   * @brief This primitive indicates to the DL state machine that the
   * transmission opportunity has arrived.
   */
  LM_SEIZE_confirm,

  /**
   * @brief The link multiplexer state machine uses this to stop transmission.
   */
  LM_RELEASE_request,

  /**
   * @brief The DL machine uses this to pass expedited data to the link
   * multiplexer.
   */
  LM_EXPEDITED_DATA_request,

  // Layer 3 <--> MDL (Management Data-link State Machine)

  /**
   * @brief Layer 3 uses this to request the Data-link state machine to
   * notify/negotiate.
   */
  MDL_NEGOTIATE_request,

  /**
   * @brief The MDL state machine uses this primitive to notify the layer 3
   * entity that notification/negotiation is complete.
   */
  MDL_NEGOTIATE_confirm,

  /**
   * @brief The MDL state machine uses this to notify the layer 3 entity that
   * notification/negotiation has failed.
   */
  MDL_ERROR_indicate
};

/**
 * @brief Data that is shared between the state machines.
 */
struct StateMachineData {
  ax25::Timers timers_{};
};

} // namespace ax25

#endif /* AX25_PRIMATIVES_HPP_ */