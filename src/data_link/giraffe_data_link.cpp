/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   giraffe_data_link.cpp
 * @brief  The main class for Giraffe Data Link
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-07-01
 * @copyright  2023 (license to be defined)
 */

#include "giraffe_data_link.hpp"
#include "gdl_message.hpp"

namespace giraffe::gdl {

DataLink::DataLink(Config &config) : config_(config) {
}

DataLink::~DataLink() {
  if (status_ != Status::DISABLED) {
    disable();
  }
}

void DataLink::enable() {
  if (status_ != Status::DISABLED) {
    return;
  }
  status_ = Status::DISCONNECTED;
  gdl_thread_stop_flag_ = false;
  gdl_thread_ = std::thread(&DataLink::gdlThread, this);
}

void DataLink::disable() {
  if (status_ == Status::DISABLED) {
    return;
  }

  gdl_thread_stop_flag_ = true;
  gdl_thread_.join();
  status_ = Status::DISABLED;
}

DataLink::Status DataLink::getStatus() const {
  return status_;
}

bool DataLink::sendMessage(const Message &message) {
  if (!isRunning()) {
    return false;
  }

  if (message.getType() == Message::Type::EXCHANGE) {
    return out_exchange_queue_.push(message);
  }

  return out_broadcast_queue_.push(message);
}

void DataLink::gdlThread() {
  while (!gdl_thread_stop_flag_) {
    Message message_buffer;

    // update lower layers
    statistics_lock_.lock();
    transport_layer_.update(statistics_);
    statistics_lock_.unlock();

    // send broadcast messages first if there are any, otherwise send exchange
    if (out_broadcast_queue_.size() > 0 && transport_layer_.isReadyToSend()) {

      if (out_broadcast_queue_.peek(message_buffer) &&
          transport_layer_.send(message_buffer)) {
        out_broadcast_queue_.pop(message_buffer);
      } else {
        /// @todo handle error (should never happen)
        (void)message_buffer;
      }

    } else if (out_exchange_queue_.size() > 0 &&
               transport_layer_.isReadyToSend()) {

      if (out_exchange_queue_.peek(message_buffer) &&
          transport_layer_.send(message_buffer)) {
        out_exchange_queue_.pop(message_buffer);
      } else {
        /// @todo handle error (should never happen)
        (void)message_buffer;
      }
    }

    // receive a message if there is one available
    if (transport_layer_.receive(message_buffer)) {
      if (!in_queue_.push(message_buffer)) {
        /// @todo handle error (no space in queue)
        (void)message_buffer;
      }
    }

    if (transport_layer_.isConnected()) {
      status_ = Status::CONNECTED;
    } else {
      status_ = Status::DISCONNECTED;
    }

    // update uplink/downlink status based on timeouts
    // update the status struct
    // gdl_status_lock_.lock();
    // gdl_status_.exchange_queue_size = queues_.exchange.size();
    // gdl_status_.broadcast_queue_size = queues_.broadcast.size();
    // gdl_status_.received_queue_size = queues_.received.size();
    // transport_layer_.updateStatus(gdl_status_);
    // gdl_status_lock_.unlock();

    BoosterSeat::threadSleep(GDL_THREAD_SLEEP_INTERVAL_MS);
  }
}

} // namespace giraffe::gdl