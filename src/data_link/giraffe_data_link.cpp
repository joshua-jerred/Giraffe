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

#include "giraffe_diagnostics.hpp"

#include "giraffe_data_link.hpp"

namespace gdl {

GiraffeDataLink::GiraffeDataLink(GdlConfig config,
                                 TransportLayer transport_layer)
    : transport_layer_(transport_layer), config_(config),
      queues_(config.exchange_queue_size, config.broadcast_queue_size,
              config.received_queue_size) {
}

GiraffeDataLink::~GiraffeDataLink() {
  if (status_ == Status::RUNNING) {
    stop();
  }
}

void GiraffeDataLink::start() {
  if (status_ != Status::STOPPED) {
    throw GiraffeException(DiagnosticId::GDL_invalidStartCall);
  }
  status_ = Status::STARTING;
  gdl_thread_ = std::thread(&GiraffeDataLink::gdlThread, this);
}

void GiraffeDataLink::stop() {
  if (status_ != Status::RUNNING) {
    throw GiraffeException(DiagnosticId::GDL_invalidStopCall);
  }
  status_ = Status::STOPPING;
  gdl_thread_.join();
}

GiraffeDataLink::Status GiraffeDataLink::getStatus() const {
  return status_;
}

bool GiraffeDataLink::getReceivedMessage(Message &message) {
  bool res = queues_.received.pop(message);

  constexpr int MAX_PACKET_AGE_SECONDS = 60;
  if ((message.time_decoded.secondsFromNow() * -1) < MAX_PACKET_AGE_SECONDS) {
    downlink_timeout_.reset();
  }

  return res;
}

bool GiraffeDataLink::getReceivedAprsGpsPacket(
    signal_easel::aprs::PositionPacket &packet) {
  if (queues_.aprs_gps_rx_queue.size() == 0) {
    return false;
  }
  packet = queues_.aprs_gps_rx_queue.front();
  queues_.aprs_gps_rx_queue.pop();

  constexpr int MAX_PACKET_AGE_SECONDS = 60;
  if ((packet.decoded_timestamp.secondsFromNow() * -1) <
      MAX_PACKET_AGE_SECONDS) {
    downlink_timeout_.reset();
  }

  return true;
}

int GiraffeDataLink::getExchangeQueueSize() const {
  return queues_.exchange.size();
}
int GiraffeDataLink::getBroadcastQueueSize() const {
  return queues_.broadcast.size();
}
int GiraffeDataLink::getReceiveQueueSize() const {
  return queues_.received.size();
}
int GiraffeDataLink::getAprsGpsTxQueueSize() const {
  return queues_.aprs_gps_tx_queue.size();
}
int GiraffeDataLink::getAprsGpsRxQueueSize() const {
  return queues_.aprs_gps_rx_queue.size();
}

void GiraffeDataLink::gdlThread() {
  constexpr int kSleepIntervalMs = 5;

  status_ = Status::RUNNING;
  while (status_ == Status::RUNNING) {
    // BoosterSeat::threadSleep(kSleepIntervalMs);
    if (queues_.exchange.size() > 0 && transport_layer_.isReady()) {
      Message msg;
      bool res = queues_.exchange.pop(msg);
      if (res) {
        res = transport_layer_.send(msg);
      }
    } else if (queues_.broadcast.size() > 0 && transport_layer_.isReady()) {
      Message msg;
      bool res = queues_.broadcast.pop(msg);
      if (res) {
        res = transport_layer_.send(msg);
      }
    } else if (queues_.aprs_gps_tx_queue.size() > 0 &&
               transport_layer_.isReady()) {
      signal_easel::aprs::PositionPacket packet =
          queues_.aprs_gps_tx_queue.front();
      queues_.aprs_gps_tx_queue.pop();

      transport_layer_.send(packet);
    }

    transport_layer_.update(queues_.received, queues_.aprs_gps_rx_queue);

    // update uplink/downlink status based on timeouts
    uplink_status_ = uplink_timeout_.isDone() ? ConnectionStatus::DISCONNECTED
                                              : ConnectionStatus::CONNECTED;
    downlink_status_ = downlink_timeout_.isDone()
                           ? ConnectionStatus::DISCONNECTED
                           : ConnectionStatus::CONNECTED;

    // update the status struct
    gdl_status_lock_.lock();
    gdl_status_.exchange_queue_size = queues_.exchange.size();
    gdl_status_.broadcast_queue_size = queues_.broadcast.size();
    gdl_status_.received_queue_size = queues_.received.size();
    transport_layer_.updateStatus(gdl_status_);
    gdl_status_lock_.unlock();

    if (config_.print_new_messages) {
      Message msg;
      while (queues_.received.pop(msg)) {
        std::cout << "Received: " << msg.data << std::endl;
      }
    }
  }
}

bool GiraffeDataLink::exchangeMessage(std::string message) {
  if (status_ != Status::RUNNING) {
    throw GiraffeException(DiagnosticId::GDL_invalidExchangeCall);
  }
  Message msg;
  msg.data = message;
  msg.type = Message::Type::EXCHANGE;
  msg.id = getNextMessageId();
  message_id_ += 1;
  return queues_.exchange.push(msg);
}

bool GiraffeDataLink::broadcastMessage(std::string message) {
  if (status_ != Status::RUNNING) {
    /// @todo this seems like a bad idea.
    throw GiraffeException(DiagnosticId::GDL_invalidBroadcastCall);
  }
  Message msg;
  msg.data = message;
  msg.type = Message::Type::BROADCAST;
  msg.id = "";
  return queues_.broadcast.push(msg);
}

bool GiraffeDataLink::broadcastAprsLocation(
    signal_easel::aprs::PositionPacket positional_data) {
  if (status_ != Status::RUNNING) {
    return false;
  }

  constexpr unsigned int MAX_APRS_TX_QUEUE_SIZE = 10;
  if (queues_.broadcast.size() > MAX_APRS_TX_QUEUE_SIZE) {
    return false;
  }

  queues_.aprs_gps_tx_queue.push(positional_data);
  return true;
}

std::string GiraffeDataLink::getNextMessageId() {
  std::string id = std::to_string((int)message_id_);
  return id;
}

} // namespace gdl