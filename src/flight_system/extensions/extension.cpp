/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   extension.cpp
 * @brief  The new implementation of the extension system.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-06-12
 * @copyright  2023 (license to be defined)
 */

#include "extension.hpp"
#include <BoosterSeat/sleep.hpp>
#include <BoosterSeat/timer.hpp>

constexpr node::Identification kExtensionId = node::Identification::EXTENSION;

namespace ext {

Extension::Extension(ext::ExtensionInterfaces &extension_interfaces,
                     cfg::ExtensionMetadata metadata)
    : metadata_(metadata), interfaces_(extension_interfaces) {
}

Extension::~Extension() {
  // If the extension is inactive, return early.
  if (static_cast<uint8_t>(status_.load()) & node::kNodeInactiveStatuses) {
    return;
  }

  constexpr int kExtensionDestroyTimeoutMs = 1500;
  constexpr int kExtensionDestroyCheckIntervalMs = 10;

  BoosterSeat::Timer timer(kExtensionDestroyTimeoutMs);
  stop(); // Signal the extension to stop.

  // Wait for the extension to stop, or for the timeout to expire.
  while (!timer.isDone() &&
         static_cast<uint8_t>(status_.load()) & node::kNodeActiveStatuses) {
    BoosterSeat::threadSleep(10);
  }
}

void Extension::start() {
  if (status_ == node::Status::RUNNING || status_ == node::Status::SLEEPING ||
      status_ == node::Status::STARTING) {
    error(data::LogId::EXTENSION_startCall);
    return;
  }
  stop_flag_ = false;
  runner_thread_ = std::thread(&Extension::runner, this);
}

void Extension::stop() {
  setStatus(node::Status::STOPPING);
  stop_flag_ = true;
  if (runner_thread_.joinable()) {
    runner_thread_.join();
  }
  setStatus(node::Status::STOPPED);
}

node::Status Extension::getStatus() const {
  return status_;
}

void Extension::setStatus(const node::Status status) {
  status_ = status;
  data::DataPacket packet;
  packet.identifier = data::DataId::EXTENSION_statusUpdate;
  packet.node_status = status;
  packet.secondary_identifier =
      metadata_.name; // extensions are identified by name
  interfaces_.streams.data.addPacket(packet);
}

void Extension::error(data::LogId log_id, const std::string &extra_info) {
  data::LogPacket packet;
  packet.source = kExtensionId;
  packet.id = log_id;
  packet.secondary_identifier = metadata_.name;
  packet.info = extra_info;
  interfaces_.streams.log.addPacket(packet);
}

void Extension::error(data::LogId log_id, int info) {
  data::LogPacket packet;
  packet.source = kExtensionId;
  packet.id = log_id;
  packet.secondary_identifier = metadata_.name;
  packet.info = std::to_string(info);
  interfaces_.streams.log.addPacket(packet);
}

void Extension::info(std::string info) {
  data::LogPacket packet;
  packet.source = kExtensionId;
  packet.id = data::LogId::GENERIC_info;
  packet.secondary_identifier = metadata_.name;
  packet.info = info;
  interfaces_.streams.log.addPacket(packet);
}

template <typename T>
void Extension::data(data::DataId identifier, T value, int precision) {
  data::DataPacket packet;
  packet.source = kExtensionId;
  packet.identifier = identifier;
  packet.secondary_identifier = metadata_.name;

  if constexpr (std::is_same<T, std::string>::value) {
    packet.value = value;
  } else if (std::is_same<T, float>::value || std::is_same<T, double>::value) {
    std::stringstream stream;
    stream << std::fixed << std::setprecision(precision) << value;
    packet.value = stream.str();
  } else {
    packet.value = std::to_string(value);
  }

  interfaces_.streams.data.addPacket(packet);
}

template void Extension::data<int>(data::DataId, int, int);
template void Extension::data<float>(data::DataId, float, int);
template void Extension::data<double>(data::DataId, double, int);
template void Extension::data<std::string>(data::DataId, std::string, int);

void Extension::runner() {
  setStatus(node::Status::STARTING);
}

void Extension::sleep() {
  constexpr int kMinimumSleepTimeMs = 50;
  constexpr int kMaximumSleepTimeMs = 600000;

  constexpr int kMinimumCheckInterval = 10;
  constexpr int kMaximumCheckInterval = 1000;

  // How long to sleep for
  int sleep_ms = std::clamp(metadata_.update_interval, kMinimumSleepTimeMs,
                            kMaximumSleepTimeMs);

  // How often to check if we should wake up/make actions
  int check_interval =
      std::clamp(sleep_ms / 10, kMinimumSleepTimeMs, kMaximumCheckInterval);

  // Sleep for the specified amount of time
  BoosterSeat::Timer timer(sleep_ms);
  while (!timer.isDone()) {
    BoosterSeat::threadSleep(check_interval);
    if (stop_flag_) {
      return;
    }
  }
}

} // namespace ext