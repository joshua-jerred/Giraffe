/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   gdl_layers.hpp
 * @brief  The network layers for Giraffe Data Link
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-01-18
 * @copyright  2024 (license to be defined)
 */

#ifndef GDL_LAYERS_HPP_
#define GDL_LAYERS_HPP_

#include <SignalEasel/aprs.hpp>

#include "BoosterSeat/timer.hpp"
#include "gdl_config_and_stats.hpp"
#include "gdl_message.hpp"
#include "gdl_packet.hpp"

namespace giraffe::gdl {

class PhysicalLayer;
class NetworkLayer;

/**
 * @brief The Transport Layer - Layer 3
 * @details The transport layer is responsible for sending messages from the
 * application layer (DataLink) to the network layer (NetworkLayer) and
 * receiving messages from the network layer and passing them to the
 * application layer.
 * The transport layer will look at the messages that is must send and will
 * route them to the correct portion of the network layer.
 *
 * Along with all of this, the transport layer is responsible for maintaining
 * the connection between the local and remote nodes.
 */
class TransportLayer {
public:
  enum class State { IDLE, EXCHANGE_SEND, EXCHANGE_WAITING_FOR_ACK, BROADCAST };

  TransportLayer(Config &config, NetworkLayer &network_layer);
  ~TransportLayer();

  /**
   * @brief Check if the transport layer is ready to send a message (true) or if
   * it is busy with the previous message (false).
   */
  bool isReadyToSend();

  /**
   * @brief Send a message to the remote node.
   * @details This will set the state of the transport layer to something other
   * than IDLE. If the message type is 'exchange' then this will return false
   * and the message will not be sent.
   * @param message - The message to send.
   */
  bool send(Message &message);

  bool receive(Message &message);

  bool isConnected() const {
    return connected_;
  }

  void update(Statistics &statistics);

private:
  void idleState();
  void controllerIdleState();
  void exchangeSendState();
  void exchangeWaitingForAckState();
  void broadcastState();

  void sendAck(uint32_t message_id);
  void sendPing();
  void sendPingResponse();

  Config &config_;
  NetworkLayer &network_layer_;

  bool connected_ = false;

  Packet current_tx_packet_{};

  bool message_received_ = false;
  Message received_message_{};

  State state_ = State::IDLE;

  /**
   * @brief Reset whenever a message is received.
   * @details Used by the controller node and the remote node.
   */
  bst::Timer received_timer_{GDL_EXCHANGE_CONNECTION_TIMEOUT_MS};

  /**
   * @brief Used by the remote node only to detect if we are getting responses
   * to packets that we send.
   */
  bst::Timer uplink_timer_{GDL_EXCHANGE_CONNECTION_TIMEOUT_MS};

  /**
   * @brief The interval to send pings at if we don't receive any messages. Used
   * by the controller node only.
   * @see GDL_EXCHANGE_PING_INTERVAL_MS
   */
  bst::Timer exchange_ping_interval_timer_{GDL_EXCHANGE_PING_INTERVAL_MS};

  /**
   * @brief A timer used to retry sending an exchange message if we don't
   * receive an ack.
   */
  bst::Timer exchange_ack_timer_{GDL_RDT_RETRY_INTERVAL_MS};

  /**
   * @brief The last message id that was acked by the local node.
   * @details This is used to determine if the message should be pushed to the
   * DataLink layer or not. If the message id of the next received message does
   * not match this, it will be pushed up. If the message id of the received
   * message matches this, it will not be pushed up but it will still be acked.
   */
  uint32_t last_acked_message_id_ = 0;

  uint32_t sent_exchange_message_id_ = 0;
};

/**
 * @brief
 */
class NetworkLayer {
public:
  NetworkLayer(Config &config, PhysicalLayer &physical_layer);
  ~NetworkLayer();

  bool txPacket(Packet &packet);

  bool rxPacket(Packet &packet);

  void update(Statistics &stats);

private:
  bool txAprsPositionPacket(const Packet &packet);

  Config &config_;

  PhysicalLayer &physical_layer_;
  signal_easel::aprs::Modulator modulator_{};
  signal_easel::aprs::Receiver receiver_{};
};

class PhysicalLayer {
public:
  enum class State {
    ERROR,
    DISABLED,
    STARTING,
    IDLE,
    TRANSMITTING,
    RECEIVING,
    BUSY
  };

  PhysicalLayer(Config &config);

  ~PhysicalLayer();

  void enable();

  void disable();

  void update();

  PhysicalLayer::State getState() const {
    return state_;
  }

protected:
  Config &config_;

  State state_ = State::DISABLED;
};

} // namespace giraffe::gdl

#endif /* GDL_LAYERS_HPP_ */