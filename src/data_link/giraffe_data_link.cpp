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

GiraffeDataLink::GiraffeDataLink(GdlConfig config, SessionLayer &session_layer)
    : config_(config),
      queues_(config.exchange_queue_size, config.broadcast_queue_size,
              config.received_queue_size),
      session_layer_(session_layer) {
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
  }
}

bool GiraffeDataLink::sendExchangeMessage(Message message) {
  if (status_ != Status::RUNNING) {
    throw GiraffeException(DiagnosticId::GDL_invalidExchangeCall);
  }
  return queues_.exchange.push(message);
}

bool GiraffeDataLink::sendBroadcastMessage(Message message) {
  if (status_ != Status::RUNNING) {
    throw GiraffeException(DiagnosticId::GDL_invalidBroadcastCall);
  }
  return queues_.broadcast.push(message);
}

} // namespace gdl