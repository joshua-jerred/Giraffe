/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   gdl_transport_layer.hpp
 * @brief  The GDL TransportLayer class/interface
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-07-01
 * @copyright  2023 (license to be defined)
 */

#ifndef GDL_TRANSPORT_LAYER_HPP_
#define GDL_TRANSPORT_LAYER_HPP_

#include <BoosterSeat/timer.hpp>

#include "gdl_network_layer.hpp"

namespace gdl {
class TransportLayer {
public:
  enum class State {
    IDLE,
    SENDING,
    WAITING_FOR_ACK,
  };

  TransportLayer(NetworkLayer &network_layer, GdlConfig config)
      : network_layer_(network_layer), config_(config),
        timer_(config_.retry_interval_ms) {
  }
  ~TransportLayer() = default;

  bool isReady() {
    return state_ == State::IDLE;
  }

  bool send(Message &message) {
    if (state_ != State::IDLE) {
      return false;
    }
    current_message_ = message;
    state_ = State::SENDING;
    return true;
  }

  bool receive(Message &message) {
    return network_layer_.rxMessage(message);
  }

  void update() {
    network_layer_.update();

    if (state_ == State::IDLE) {
      return;
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
        }
      }
    }
  }

private:
  bool sendPacket() {
    if (!network_layer_.txMessage(current_message_)) {
      return false;
    }
    return true;
  }

  void startTimer() {
    timer_.reset();
  }

  bool isTimerExpired() {
    return timer_.isDone();
  }

  bool receivedAck() {
    return false;
  }

  NetworkLayer &network_layer_;

  GdlConfig config_;

  BoosterSeat::Timer timer_;

  Message current_message_{};

  State state_ = State::IDLE;
};
}; // namespace gdl

#endif /* GDL_SESSION_LAYER_INTERFACE_HPP_ */