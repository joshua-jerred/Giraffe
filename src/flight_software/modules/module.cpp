/**
 * @file module.cpp
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief Implementation of the module base class.
 * @date 2023-02-14
 * @copyright Copyright (c) 2023
 * @version 0.4
 *
 * @todo Documentation
 */

#include "module.h"

#include <iomanip>
#include <sstream>
#include <thread>
#include <type_traits>

modules::Module::Module(modules::MetaData metadata, data::Streams &streams,
                        cfg::Configuration &configuration)
    : metadata_(metadata),
      streams_(streams),
      configuration_(configuration),
      runner_thread_(),
      command_queue_(metadata.id_) {
}

void modules::Module::start() {
  if (status_ == modules::Status::RUNNING ||
      status_ == modules::Status::SLEEPING) {
    return;
  }

  stop_flag_ = false;
  runner_thread_ = std::thread(&modules::Module::runner, this);
}

void modules::Module::stop() {
  stop_flag_ = true;
  if (runner_thread_.joinable()) {
    runner_thread_.join();
  }
}

modules::Status modules::Module::getStatus() const {
  return status_;
}

/**
 * @todo report status to status stream
 */
void modules::Module::setStatus(const modules::Status status) {
  status_ = status;
}

/**
 * @todo
 */
void modules::Module::error(std::string error_code, std::string info) {
  (void)error_code;
  (void)info;
}

template <typename T>
void modules::Module::data(std::string identifier, T value, int precision) {
  if constexpr (std::is_same<T, std::string>::value) {
    streams_.data_stream.addData(metadata_.id_, identifier, value);
  } else if (std::is_same<T, float>::value || std::is_same<T, double>::value) {
    std::stringstream stream;
    stream << std::fixed << std::setprecision(precision) << value;
    std::string rounded = stream.str();
    streams_.data_stream.addData(metadata_.id_, identifier, rounded);
  } else {
    streams_.data_stream.addData(metadata_.id_, identifier,
                                 std::to_string(value));
  }
}

template void modules::Module::data<int>(std::string, int, int);
template void modules::Module::data<float>(std::string, float, int);
template void modules::Module::data<double>(std::string, double, int);
template void modules::Module::data<std::string>(std::string, std::string, int);

void modules::Module::runner() {
  setStatus(modules::Status::STARTING);
  startup();
  while (!stop_flag_) {
    loop();
    setStatus(modules::Status::SLEEPING);
    sleep();
    setStatus(modules::Status::RUNNING);
  }
  setStatus(modules::Status::STOPPING);
  shutdown();
  setStatus(modules::Status::STOPPED);
}

void modules::Module::sleep() {
  constexpr int kMinimumSleepTimeMs = 100;
  constexpr int kMaximumSleepTimeMs = 5000;

  int sleep_ms = metadata_.sleep_interval;
  if (metadata_.sleep_interval < kMinimumSleepTimeMs) {
    sleep_ms = kMinimumSleepTimeMs;
  } else if (metadata_.sleep_interval > kMaximumSleepTimeMs) {
    sleep_ms = kMaximumSleepTimeMs;
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(sleep_ms));
}
