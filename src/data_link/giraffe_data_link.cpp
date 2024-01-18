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

#include "gdl_message.hpp"
#include "giraffe_diagnostics.hpp"

#include "giraffe_data_link.hpp"

namespace giraffe::gdl {

DataLink::DataLink(Config &config) : config_(config), {
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

// bool DataLink::sendMessage(std::string message) {
//   if (status_ == Status::DISABLED) {
//     return false;
//   }
//   Message msg;
//   msg.data = message;
//   msg.type = Message::Type::EXCHANGE;
//   msg.id = getNextMessageId();
//   message_id_ += 1;
//   return exchange_queue_.push(msg);
// }

// bool DataLink::getReceivedMessage(Message &message) {
//   bool res = queues_.received.pop(message);

//   constexpr int MAX_PACKET_AGE_SECONDS = 60;
//   if ((message.time_decoded.secondsFromNow() * -1) < MAX_PACKET_AGE_SECONDS)
//   {
//     downlink_timeout_.reset();
//   }

//   return res;
// }

bool DataLink::sendMessage(Message message) {
  if (!isRunning() ||) {
    return false;
  }
  Message msg;
  msg.data = message;
  msg.type = Message::Type::BROADCAST;
  msg.id = "";
  return broadcast_queue_.push(msg);
}

// bool DataLink::getReceivedAprsGpsPacket(
//     signal_easel::aprs::PositionPacket &packet) {
//   if (queues_.aprs_gps_rx_queue.size() == 0) {
//     return false;
//   }
//   packet = queues_.aprs_gps_rx_queue.front();
//   queues_.aprs_gps_rx_queue.pop();

//   constexpr int MAX_PACKET_AGE_SECONDS = 60;
//   if ((packet.decoded_timestamp.secondsFromNow() * -1) <
//       MAX_PACKET_AGE_SECONDS) {
//     downlink_timeout_.reset();
//   }

//   return true;
// }

void DataLink::gdlThread() {
  while (!gdl_thread_stop_flag_) {
    BoosterSeat::threadSleep(GDL_THREAD_SLEEP_INTERVAL_MS);

    transport_layer_.update(queues_.received, queues_.aprs_gps_rx_queue);

    // update uplink/downlink status based on timeouts
    // update the status struct
    // gdl_status_lock_.lock();
    // gdl_status_.exchange_queue_size = queues_.exchange.size();
    // gdl_status_.broadcast_queue_size = queues_.broadcast.size();
    // gdl_status_.received_queue_size = queues_.received.size();
    // transport_layer_.updateStatus(gdl_status_);
    // gdl_status_lock_.unlock();
  }
}

// bool DataLink::broadcastAprsLocation(
//     signal_easel::aprs::PositionPacket positional_data) {
//   if (status_ != Status::RUNNING) {
//     return false;
//   }

//   constexpr unsigned int MAX_APRS_TX_QUEUE_SIZE = 10;
//   if (queues_.broadcast.size() > MAX_APRS_TX_QUEUE_SIZE) {
//     return false;
//   }

//   queues_.aprs_gps_tx_queue.push(positional_data);
//   return true;
// }

// std::string DataLink::getNextMessageId() {
//   std::string id = std::to_string((int)message_id_);
//   return id;
// }

} // namespace giraffe::gdl