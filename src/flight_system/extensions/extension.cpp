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

namespace extension {

Extension::Extension(ExtensionResources &extension_interfaces,
                     cfg::ExtensionMetadata metadata)
    : metadata_(metadata), interfaces_(extension_interfaces) {
}

Extension::~Extension() {
  stop(); // Attempt to stop the extension if it is running.
}

void Extension::start() {
  if (static_cast<uint16_t>(status_.load()) & node::kNodeActiveStatuses) {
    error(DiagnosticId::EXTENSION_startCall);
    return;
  }

  // Reset flags
  stop_flag_ = false;
  fault_flag_ = false;
  fault_code_ = DiagnosticId::EXT_FAULT_none;

  runner_thread_ = std::thread(&Extension::runner, this);
}

void Extension::stop() {
  /// @brief The timeout for stopping an extension.
  constexpr int kExtensionStopTimeoutMs = 1500;
  /// @brief The interval of how often to check if the extension has stopped.
  constexpr int kExtensionStopCheckIntervalMs = 10;

  // If the extension is already inactive, join the thread if it's joinable and
  // return.
  if (static_cast<uint16_t>(status_.load()) & node::kNodeInactiveStatuses) {
    if (runner_thread_.joinable()) {
      runner_thread_.join();
    }
    return;
  }

  stop_flag_ = true; // signal the stop
  BoosterSeat::Timer timer(kExtensionStopTimeoutMs);

  while (!timer.isDone()) {
    BoosterSeat::threadSleep(kExtensionStopCheckIntervalMs);
    if (static_cast<uint16_t>(status_.load()) & node::kNodeInactiveStatuses) {
      if (runner_thread_.joinable()) {
        runner_thread_.join();
      }
      return; // If the extension has stopped, return.
    }
  }
  error(DiagnosticId::EXTENSION_stopTimeout);
}

void Extension::reset() {
  stop();
  status_ = node::Status::STOPPED;
}

void Extension::error(DiagnosticId log_id, const std::string &extra_info) {
  data::LogPacket packet;
  packet.source = kExtensionId;
  packet.id = log_id;
  packet.secondary_identifier = metadata_.name;
  packet.info = extra_info;
  interfaces_.streams.log.addPacket(packet);
}

void Extension::error(DiagnosticId log_id, int info) {
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
  packet.id = DiagnosticId::GENERIC_info;
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

/**
 * @brief templates for Extension::data
 * @verbatim
 */

template void Extension::data<int>(data::DataId, int, int);
template void Extension::data<float>(data::DataId, float, int);
template void Extension::data<double>(data::DataId, double, int);
template void Extension::data<std::string>(data::DataId, std::string, int);

/**
 * @endverbatim
 */

void Extension::data(data::GpsFrame frame) {
  interfaces_.streams.gps.addFrame(node::Identification::EXTENSION,
                                   metadata_.name, frame);
}

void Extension::data(data::ImuFrame frame) {
  interfaces_.streams.imu.addFrame(node::Identification::EXTENSION,
                                   metadata_.name, frame);
}

void Extension::extSleep(uint32_t ms) {
  const uint32_t kMaxSleepTimeMs = 1000;
  if (ms > kMaxSleepTimeMs) {
    ms = kMaxSleepTimeMs;
  }
  BoosterSeat::threadSleep(ms);
}

void Extension::sleep() {
  constexpr int kMinimumSleepTimeMs = 50;
  constexpr int kMaximumSleepTimeMs = 600000;

  constexpr int kMinimumCheckInterval = 10;
  constexpr int kMaximumCheckInterval = 500;

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
    if (stopRequested()) {
      return;
    }
  }
}

void Extension::runner() {
  // -- STARTUP -- //
  status_ = node::Status::STARTING;
  startup();
  if (fault_flag_) {
    status_ = node::Status::ERROR;
    return;
  }

  // -- RUN -- //
  while (!stop_flag_) {
    status_ = node::Status::RUNNING;
    loop();
    if (fault_flag_) {
      status_ = node::Status::ERROR;
      return;
    }
    sleep();
  }
  if (fault_flag_) {
    status_ = node::Status::ERROR;
    return;
  }

  // -- SHUTDOWN -- //
  status_ = node::Status::STOPPING;
  shutdown();
  if (fault_flag_) {
    status_ = node::Status::ERROR;
    return;
  }
  status_ = node::Status::STOPPED;
}

bool Extension::stopRequested() const {
  return stop_flag_;
}

void Extension::raiseFault(DiagnosticId fault_code, std::string extra_info) {
  fault_flag_ = true;
  fault_code_ = fault_code;
  error(fault_code, extra_info);
  stop_flag_ = true;
}

} // namespace extension