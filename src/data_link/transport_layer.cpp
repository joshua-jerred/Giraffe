/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   transport_layer.cpp
 * @brief  GDL Transport Layer Implementation
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-01-18
 * @copyright  2024 (license to be defined)
 */

#include "gdl_constants.hpp"
#include "gdl_message.hpp"

#include "layers/transport_layer.hpp"

namespace giraffe::gdl {

TransportLayer::TransportLayer(Config &config, NetworkLayer &network_layer)
    : config_(config), network_layer_(network_layer) {
}
TransportLayer::~TransportLayer() {
}

bool TransportLayer::isReadyToSend() {
  if (!new_tx_timer_.isDone()) {
    return false;
  }
  return state_ == State::IDLE;
}

bool TransportLayer::send(Message &message) {
  if (!isReadyToSend()) {
    return false;
  }

  // if it's an exchange message, we need to make sure we're connected.
  // but only in proactive keep alive mode
  if (message.getType() == Message::Type::EXCHANGE) {
    if (!isConnected() && config_.getProactiveKeepAlive()) {
      return false;
    }
    current_tx_packet_ = message;
    current_tx_packet_.setPacketType(Packet::PacketType::EXCHANGE);
    state_ = State::EXCHANGE_SEND;
    return true;
  }

  // if it's a location message, we don't need to be connected
  if (message.getType() == Message::Type::LOCATION) {
    current_tx_packet_ = message;
    current_tx_packet_.setPacketType(Packet::PacketType::LOCATION);
    state_ = State::BROADCAST;
    return true;
  }

  // if it's an image message, we don't need to be connected
  if (message.getType() == Message::Type::IMAGE) {
    current_tx_packet_ = message;
    current_tx_packet_.setPacketType(Packet::PacketType::SSTV);
    state_ = State::BROADCAST;
    return true;
  }

  // if it's a telemetry message, we don't need to be connected
  if (message.getType() == Message::Type::TELEMETRY) {
    current_tx_packet_ = message;
    current_tx_packet_.setPacketType(Packet::PacketType::APRS_TELEMETRY);
    state_ = State::BROADCAST;
    return true;
  }

  // if it's a broadcast message, we don't need to be connected
  if (message.getType() == Message::Type::BROADCAST) {
    current_tx_packet_ = message;
    current_tx_packet_.setPacketType(Packet::PacketType::BROADCAST);
    state_ = State::BROADCAST;
    return true;
  }

  /// @todo handle other messages and report an error
  return false;
}

bool TransportLayer::receive(Message &message) {
  if (!message_received_) {
    return false;
  }

  message = received_message_;
  message_received_ = false; // reset the receive state
  return true;
}

void TransportLayer::update(Statistics &statistics) {
  network_layer_.update(statistics);

  // state machine
  switch (state_) {
  case State::IDLE:
    idleState();
    break;
  case State::EXCHANGE_SEND:
    exchangeSendState();
    break;
  case State::EXCHANGE_WAITING_FOR_ACK:
    exchangeWaitingForAckState();
    break;
  case State::BROADCAST:
    broadcastState();
    break;
  }

  // update statistics
  statistics.uplink_connected = uplink_connected_;
  statistics.downlink_connected = downlink_connected_;
  statistics.total_messages_dropped = total_messages_dropped_;
  statistics.last_message_received = last_message_received_;
  statistics.position_packets_received = position_packets_received_;
  statistics.telemetry_packets_received = telemetry_packets_received_;
}

void TransportLayer::idleState() {
  Packet packet_buffer;

  // special case for the controller node
  if (config_.isController()) {
    controllerIdleState();
    if (uplink_timer_.isDone() && config_.getProactiveKeepAlive()) {
      connected_ = false;
      uplink_connected_ = false;
    }
  }

  if (received_timer_.isDone()) {
    connected_ = false;
    downlink_connected_ = false;
  }

  // check for incoming packets
  if (network_layer_.rxPacket(packet_buffer)) {
    received_timer_.reset(); // reset the connection timeout timer
    downlink_connected_ = true;

    if (message_received_) {
      /// @todo remove this. We should never get here. We can log this.
      std::cout << "WARNING: dropping packet!\n";
    }

    auto typ = packet_buffer.getPacketType();
    auto message_id = packet_buffer.getIdentifier();
    switch (typ) {

    case Packet::PacketType::EXCHANGE:
      if (message_id != last_acked_message_id_) {
        // if we haven't already acked this message, we probably
        // have not already received it. Push up to the DataLink
        /// @note this may be worth expanding to a queue in the future
        last_acked_message_id_ = message_id;
        received_message_.setExchangeMessage(packet_buffer.getData(),
                                             packet_buffer.getIdentifier());
        message_received_ = true;
        last_message_received_.setToNow();
      } else {
        std::cout << "WARNING: received duplicate exchange message\n";
      }
      sendAck(message_id);
      break;

    case Packet::PacketType::APRS_TELEMETRY:
      telemetry_packets_received_++;
      received_message_ = static_cast<Message>(packet_buffer);
      received_message_.setType(Message::Type::TELEMETRY);
      message_received_ = true;
      last_message_received_.setToNow();
      break;
    case Packet::PacketType::LOCATION:
      position_packets_received_++;
      received_message_ = static_cast<Message>(packet_buffer);
      received_message_.setType(Message::Type::LOCATION);
      message_received_ = true;
      last_message_received_.setToNow();
      break;
    case Packet::PacketType::BROADCAST:
      received_message_ = static_cast<Message>(packet_buffer);
      message_received_ = true;
      last_message_received_.setToNow();
      break;
    case Packet::PacketType::PING:
      sendPingResponse();
      connected_ = true;
      received_timer_.reset(); // we got a ping, reset the timer.
      /// @todo handle connection timeout reset
      break;
    case Packet::PacketType::PING_RESPONSE:
      connected_ = true;
      uplink_connected_ = true;
      uplink_timer_.reset(); // we got a ping response, reset the timer.
      exchange_ping_interval_timer_.reset(); // reset the ping interval timer
      /// @todo handle connection timeout reset
      break;
    default: // ack or unknown
      /// @todo log this
      break;
    }
  }
}

void TransportLayer::controllerIdleState() {
  // if we haven't received an exchange response in a while, send a ping.
  if (exchange_ping_interval_timer_.isDone() &&
      config_.getProactiveKeepAlive()) {
    sendPing();
    exchange_ping_interval_timer_.reset();
  }
}

void TransportLayer::exchangeSendState() {
  // no need to receive in this state as we will immediately exit it
  sent_exchange_message_id_ = current_tx_packet_.getIdentifier();

  if (current_tx_packet_.getSendAttempts() >= GDL_RDT_MAX_RETRIES) {
    /// @todo handle error
    std::cout << "ERROR: max retries exceeded\n";
    total_messages_dropped_++;
    state_ = State::IDLE;
    return;
  }

  if (!network_layer_.txPacket(current_tx_packet_)) {
    /// @todo handle error
    std::cout << "ERROR: failed to send exchange message\n";
    state_ = State::IDLE;
    return;
  }

  // start the ack timer
  current_tx_packet_.incrementSendAttempts();
  exchange_ack_timer_.reset();

  state_ = State::EXCHANGE_WAITING_FOR_ACK;
}

void TransportLayer::exchangeWaitingForAckState() {
  Packet packet_buffer;

  // check for incoming packets
  if (network_layer_.rxPacket(packet_buffer)) {
    received_timer_.reset(); // reset the connection timeout timer

    if (message_received_) {
      /// @todo remove this. We should never get here. We can log this.
      std::cout << "WARNING: dropping packet in wait for ack state!\n";
    }

    auto typ = packet_buffer.getPacketType();
    auto message_id = packet_buffer.getIdentifier();
    switch (typ) {
    case Packet::PacketType::ACK:
      if (message_id == sent_exchange_message_id_) {
        // we got an ack for the message we sent
        state_ = State::IDLE;
        std::cout << "INFO: received ack for message " << message_id << "\n";
        uplink_connected_ = true;
        uplink_timer_.reset(); // we got an ack, reset the timer.
        new_tx_timer_.reset(); // delay before sending a new message
        exchange_ping_interval_timer_.reset(); // no need to send a ping
        return;
      }
      std::cout << "WARNING: received ack for message " << message_id
                << " but we sent " << sent_exchange_message_id_ << "\n";
      break;

    default:
      std::cout
          << "WARNING: received unexpected packet in wait for ack state!\n";
      /// @todo log this
      break;
    }
  }

  if (exchange_ack_timer_.isDone()) {
    state_ = State::EXCHANGE_SEND;
    return;
  }
}

void TransportLayer::broadcastState() {
  // no need to receive in this state as we will immediately exit it
  if (!network_layer_.txPacket(current_tx_packet_)) {
    /// @todo handle error
    std::cout << "ERROR: failed to send broadcast message\n";
  }
  new_tx_timer_.reset(); // delay before sending a new message
  state_ = State::IDLE;
}

void TransportLayer::sendAck(uint32_t message_id) {
  Packet packet;
  packet.setPacketType(Packet::PacketType::ACK);
  packet.setIdentifier(message_id);
  if (!network_layer_.txPacket(packet)) {
    /// @todo handle error
    std::cout << "ERROR: failed to send ack\n";
  }
}

void TransportLayer::sendPing() {
  Packet packet;
  packet.setPacketType(Packet::PacketType::PING);
  packet.setIdentifier(0);
  if (!network_layer_.txPacket(packet)) {
    /// @todo handle error
    std::cout << "ERROR: failed to send ping\n";
  }
}

void TransportLayer::sendPingResponse() {
  Packet packet;
  packet.setPacketType(Packet::PacketType::PING_RESPONSE);
  packet.setIdentifier(0);
  if (!network_layer_.txPacket(packet)) {
    /// @todo handle error
    std::cout << "ERROR: failed to send ping response\n";
  }
}

} // namespace giraffe::gdl