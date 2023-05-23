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

modules::Module::Module(modules::MetaData &metadata,
                        data::SharedData &shared_data,
                        cfg::Configuration &configuration)
    : metadata_(metadata),
      shared_data_(shared_data),
      configuration_(configuration),
      runner_thread_(),
      command_queue_(metadata.id_) {
}

void modules::Module::start() {
  if (status_ == node::Status::RUNNING ||
      status_ == node::Status::SLEEPING) {
        error("MD_ALST"); // Module Already Started
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

node::Status modules::Module::getStatus() const {
  return status_;
}

/**
 * @todo report status to status stream
 */
void modules::Module::setStatus(const node::Status status) {
  status_ = status;
}

void modules::Module::error(std::string error_code, std::string info) {
  shared_data_.streams.error.addError(metadata_.id_, error_code, info);
}

template <typename T>
void modules::Module::data(std::string identifier, T value, int precision) {
  if constexpr (std::is_same<T, std::string>::value) {
    shared_data_.streams.data.addData(metadata_.id_, identifier, value);
  } else if (std::is_same<T, float>::value || std::is_same<T, double>::value) {
    std::stringstream stream;
    stream << std::fixed << std::setprecision(precision) << value;
    std::string rounded = stream.str();
    shared_data_.streams.data.addData(metadata_.id_, identifier, rounded);
  } else {
    shared_data_.streams.data.addData(metadata_.id_, identifier,
                                 std::to_string(value));
  }
}

template void modules::Module::data<int>(std::string, int, int);
template void modules::Module::data<float>(std::string, float, int);
template void modules::Module::data<double>(std::string, double, int);
template void modules::Module::data<std::string>(std::string, std::string, int);

void modules::Module::runner() {
  setStatus(node::Status::STARTING);
  startup();
  while (!stop_flag_) {
    loop();
    setStatus(node::Status::SLEEPING);
    sleep();
    setStatus(node::Status::RUNNING);
  }
  setStatus(node::Status::STOPPING);
  shutdown();
  setStatus(node::Status::STOPPED);
}

void modules::Module::sleep() {
  constexpr int kMinimumSleepTimeMs = 100;
  constexpr int kMaximumSleepTimeMs = 5000;

  int sleep_ms = metadata_.sleep_interval_;
  if (metadata_.sleep_interval_ < kMinimumSleepTimeMs) {
    sleep_ms = kMinimumSleepTimeMs;
  } else if (metadata_.sleep_interval_ > kMaximumSleepTimeMs) {
    sleep_ms = kMaximumSleepTimeMs;
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(sleep_ms));
}
