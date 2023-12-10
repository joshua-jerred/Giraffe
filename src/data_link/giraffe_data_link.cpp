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

#include <BoosterSeat/sleep.hpp>

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

GiraffeDataLink::ConnectionStatus GiraffeDataLink::getConnectionStatus() const {
  return connection_status_;
}

bool GiraffeDataLink::getReceivedMessage(Message &message) {
  return queues_.received.pop(message);
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

void GiraffeDataLink::gdlThread() {
  constexpr int kSleepIntervalMs = 50;

  status_ = Status::RUNNING;
  while (status_ == Status::RUNNING) {
    BoosterSeat::threadSleep(kSleepIntervalMs);
    if (queues_.exchange.size() > 0 && transport_layer_.isReady()) {
      Message msg;
      bool res = queues_.exchange.pop(msg);
      if (res) {
        res = transport_layer_.send(msg);
      }
    }

    transport_layer_.update();

    // update the status struct
    gdl_status_lock_.lock();
    gdl_status_.exchange_queue_size = queues_.exchange.size();
    gdl_status_.broadcast_queue_size = queues_.broadcast.size();
    gdl_status_.received_queue_size = queues_.received.size();
    gdl_status_lock_.unlock();
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
  return queues_.exchange.push(msg);
}

bool GiraffeDataLink::broadcastMessage(std::string message) {
  if (status_ != Status::RUNNING) {
    throw GiraffeException(DiagnosticId::GDL_invalidBroadcastCall);
  }
  Message msg;
  msg.data = message;
  msg.type = Message::Type::BROADCAST;
  msg.id = 0;
  return queues_.broadcast.push(msg);
}

uint16_t GiraffeDataLink::getNextMessageId() {
  return queues_.message_id_++;
}

} // namespace gdl