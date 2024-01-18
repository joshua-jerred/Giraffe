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

#include "giraffe_data_link.hpp"

namespace giraffe {

GiraffeDataLink::TransportLayer::TransportLayer(NetworkLayer &network_layer,
                                                GdlConfig config)
    : network_layer_(network_layer), config_(config),
      timer_(config_.retry_interval_ms) {
}
GiraffeDataLink::TransportLayer::~TransportLayer() {
}

bool GiraffeDataLink::TransportLayer::isReady() {
  return state_ == State::IDLE;
}

bool GiraffeDataLink::TransportLayer::send(Message &message) {
  if (state_ != State::IDLE) {
    return false;
  }
  current_message_ = message;
  state_ = State::SENDING;
  return true;
}

bool GiraffeDataLink::TransportLayer::send(
    signal_easel::aprs::PositionPacket &packet) {
  auto gdl_aprs_nw_layer =
      dynamic_cast<gdl::AprsNetworkLayer *>(&network_layer_);
  if (gdl_aprs_nw_layer == nullptr) {
    /// @todo throw something
    return false;
  }
  state_ = State::SENDING;

  bool res = gdl_aprs_nw_layer->txAprsPositionPacket(packet);

  state_ = State::IDLE;
  return res;
}

bool GiraffeDataLink::TransportLayer::receive(Message &message) {
  return network_layer_.rxMessage(message);
}

void GiraffeDataLink::TransportLayer::update(
    MessageQueue &received_messages,
    std::queue<signal_easel::aprs::PositionPacket> &aprs_gps_rx_queue) {
  network_layer_.update();

  if (state_ == State::IDLE) {
    Message message{};
    if (network_layer_.rxMessage(message)) {
      // std::cout << "Got a new message: " << message.data
      // << " id: " << message.id << std::endl;
      if (message.isValid() && !message.isAck()) {
        sendAck(message.id);
        if (message.id == last_acked_id_) {
          // std::cout << "Got a duplicate message." << std::endl;
          return;
        }
        last_acked_id_ = message.id;
        received_messages.push(message);
      } else if (message.isAck()) {
        // std::cout << "Got an ACK, seemingly out of place." << std::endl;
      }
    }

    auto gdl_aprs_nw_layer =
        dynamic_cast<gdl::AprsNetworkLayer *>(&network_layer_);
    if (gdl_aprs_nw_layer == nullptr) {
      return;
    }
    signal_easel::aprs::PositionPacket packet{};
    signal_easel::ax25::Frame frame{};
    if (gdl_aprs_nw_layer->rxAprsPositionPacket(packet, frame)) {
      packet.frame = frame; // mush them together. Lazy fix.
      aprs_gps_rx_queue.push(packet);
    }

  } else if (state_ == State::SENDING) {
    if (!sendPacket()) {
      return;
    }
    startTimer();
    state_ = State::WAITING_FOR_ACK;
  } else if (state_ == State::WAITING_FOR_ACK) {
    if (receivedAck()) {
      state_ = State::IDLE;
    } else if (isTimerExpired()) {
      if (current_message_.retries < config_.max_retries) {
        current_message_.retries++;
        state_ = State::SENDING;
      } else {
        state_ = State::IDLE;
        std::cout << "Message failed to send." << std::endl;
      }
    }
  }
}

void GiraffeDataLink::TransportLayer::updateStatus(gdl::GdlStatus &status) {
  network_layer_.updateStatus(status);
}

bool GiraffeDataLink::TransportLayer::sendPacket() {
  if (!network_layer_.txMessage(current_message_)) {
    return false;
  }
  return true;
}

void GiraffeDataLink::TransportLayer::sendAck(std::string id) {
  // std::cout << "Sending ACK for id: " << id << std::endl;
  Message message{};
  message.data = "ACK";
  message.id = id;
  message.type = Message::Type::EXCHANGE;
  // BoosterSeat::threadSleep(5);
  network_layer_.txMessage(message);
}

void GiraffeDataLink::TransportLayer::startTimer() {
  timer_.reset();
}

bool GiraffeDataLink::TransportLayer::isTimerExpired() {
  return timer_.isDone();
}

bool GiraffeDataLink::TransportLayer::receivedAck() {
  Message message{};
  if (network_layer_.rxMessage(message)) {
    if (message.data == "ACK") {
      // std::cout << "Got an ACK!" << std::endl;
      return true;
    }
  }
  return false;
}

} // namespace giraffe