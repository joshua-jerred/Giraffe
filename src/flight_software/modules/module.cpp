/**
 * @file module.cpp
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief Implementation of the module base class.
 * @date 2023-02-14
 * @copyright Copyright (c) 2023
 * @version 0.5
 */

#include "module.hpp"

#include <iomanip>
#include <sstream>
#include <thread>
#include <type_traits>

modules::Module::Module(modules::MetaData &metadata,
                        data::SharedData &shared_data,
                        cfg::Configuration &configuration)
    : metadata_(metadata), shared_data_(shared_data),
      configuration_(configuration), runner_thread_(),
      command_queue_(metadata.id_) {}

void modules::Module::start() {
  if (status_ == node::Status::RUNNING || status_ == node::Status::SLEEPING) {
    error(data::LogId::MODULE_moduleAlreadyStarted);
    return;
  }

  stop_flag_ = false;
  runner_thread_ = std::thread(&modules::Module::runner, this);
}

void modules::Module::stop() {
  setStatus(node::Status::STOPPING);
  stop_flag_ = true;
  if (runner_thread_.joinable()) {
    runner_thread_.join();
  }
  setStatus(node::Status::STOPPED);
}

node::Status modules::Module::getStatus() const { return status_; }

void modules::Module::setStatus(const node::Status status) {
  status_ = status;
  shared_data_.streams.data.reportStatus(metadata_.id_, status);
}

void modules::Module::error(data::LogId log_id, std::string info) {
  shared_data_.streams.log.error(metadata_.id_, log_id, info);
}

void modules::Module::error(data::LogId log_id, int info) {
  shared_data_.streams.log.error(metadata_.id_, log_id, std::to_string(info));
}

void modules::Module::info(std::string info) {
  shared_data_.streams.log.info(metadata_.id_, info);
}

template <typename T>
void modules::Module::data(data::DataId identifier, T value, int precision) {
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

template void modules::Module::data<int>(data::DataId, int, int);
template void modules::Module::data<float>(data::DataId, float, int);
template void modules::Module::data<double>(data::DataId, double, int);
template void modules::Module::data<std::string>(data::DataId, std::string,
                                                 int);

void modules::Module::runner() {
  setStatus(node::Status::STARTING);
  startup();
  while (!stop_flag_) {
    loop();
    if (metadata_.sleep_interval_ > 2000) {
      setStatus(node::Status::SLEEPING); // Really no point if it's less than 2s
    }
    sleep();
    setStatus(node::Status::RUNNING);
  }
  setStatus(node::Status::STOPPING);
  shutdown();
  setStatus(node::Status::STOPPED);
}

void modules::Module::sleep() {
  constexpr int kMinimumSleepTimeMs = 50;
  constexpr int kMaximumSleepTimeMs = 5000;

  int sleep_ms = metadata_.sleep_interval_;
  if (metadata_.sleep_interval_ < kMinimumSleepTimeMs) {
    sleep_ms = kMinimumSleepTimeMs;
  } else if (metadata_.sleep_interval_ > kMaximumSleepTimeMs) {
    sleep_ms = kMaximumSleepTimeMs;
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(sleep_ms));
}
