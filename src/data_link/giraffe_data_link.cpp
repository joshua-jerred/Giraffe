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

#include "giraffe_assert.hpp"

#include "gdl_message.hpp"
#include "giraffe_data_link.hpp"

namespace giraffe::gdl {

DataLink::DataLink(Config &config,
                   std::shared_ptr<PhysicalLayer> p_physical_layer)
    : config_(config), p_physical_layer_(p_physical_layer),
      transport_layer_(config_, network_layer_) {
  status_ = Status::DISABLED;
}

DataLink::~DataLink() {
  if (status_ != Status::DISABLED) {
    disable();
  }
}

bool DataLink::setPhysicalLayer(
    std::shared_ptr<PhysicalLayer> p_physical_layer) {
  if (isRunning()) {
    return false;
  }
  p_physical_layer_ = p_physical_layer;
  return true;
}

bool DataLink::enable() {
  if (isRunning()) {
    return true; // already running
  }

  if (p_physical_layer_ == nullptr) {
    return false;
  }

  setStatus(Status::STARTING);
  gdl_thread_stop_flag_ = false;
  gdl_thread_ = std::thread(&DataLink::gdlThread, this);
  return isRunning();
}

bool DataLink::disable() {
  if (!isEnabled()) {
    return true; // already disabled
  }

  setStatus(Status::STOPPING);
  gdl_thread_stop_flag_ = true;
  gdl_thread_.join();
  return !isRunning();
}

DataLink::Status DataLink::getStatus() const {
  return status_;
}

bool DataLink::sendMessage(const Message &message) {
  if (!isRunning()) {
    return false;
  }

  // Only text based messages require the logic of exchange messages
  if (message.getType() == Message::Type::EXCHANGE) {
    return out_exchange_queue_.push(message);
  }

  return out_broadcast_queue_.push(message);
}

bool DataLink::sendText(const std::string &text, uint32_t message_id) {
  if (!isRunning()) {
    return false;
  }

  Message message;
  message.setExchangeMessage(text, message_id);
  return out_exchange_queue_.push(message);
}

bool DataLink::receiveMessage(Message &message) {
  if (!isRunning()) {
    return false;
  }

  return in_queue_.pop(message);
}

void DataLink::gdlThread() {
  Message message_buffer;
  while (!gdl_thread_stop_flag_) {
    setStatus(Status::RUNNING);

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
        std::cout << "ERROR: No space in received queue\n";
      }
    }

    // if (transport_layer_.isConnected()) {
    //   status_ = Status::CONNECTED;
    // } else {
    //   status_ = Status::DISCONNECTED;
    // }

    // update the status struct
    statistics_lock_.lock();
    statistics_.exchange_queue_size = out_exchange_queue_.size();
    statistics_.broadcast_queue_size = out_broadcast_queue_.size();
    statistics_.received_queue_size = in_queue_.size();
    statistics_lock_.unlock();

    bst::sleep(GDL_THREAD_SLEEP_INTERVAL_MS);
  }

  setStatus(Status::DISABLED);
}

} // namespace giraffe::gdl