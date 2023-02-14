/**
 * @file module.cpp
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief Implementation of the module base class.
 * @date 2023-02-14
 * @copyright Copyright (c) 2023
 * @version 0.4
 */

#include "modules.h"

modules::Module::Module(DataStream &stream, std::string error_prefix,
                        std::string module_name)
    : data_stream_(stream),
      module_status_(ModuleStatus::STOPPED),
      error_source_(error_prefix),
      module_name_(module_name) {}

void modules::Module::updateStatus(ModuleStatus new_status) {
  data_stream_.updateModuleStatus(module_name_, new_status);
}

void modules::Module::error(std::string error_code, std::string info) {
  data_stream_.addError("M_" + error_source_, error_code, info, 0);
};

void modules::Module::error(std::string error_code) {
  data_stream_.addError(error_source_, error_code, "", 0);
};

void modules::Module::data(std::string data_name, std::string data_value) {
  data_stream_.addData(error_source_, data_name, data_value);
};

void modules::Module::data(std::string data_name, int data_value) {
  data_stream_.addData(error_source_, data_name, std::to_string(data_value));
};

void modules::Module::module_sleep(int sleep_time) {
  if (sleep_time == -1) {
    sleep_time = kDefaultSleepTime_;
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
}

bool modules::Module::addCommand(GFSCommand command) {
  const int kMaxCommandQueueSize = 20;
  command_queue_lock_.lock();
  if (command_queue_.size() > kMaxCommandQueueSize) {
    command_queue_lock_.unlock();
    error("MCQF");  // Module Command Queue Full
    return false;
  }
  command_queue_.push(command);
  command_queue_lock_.unlock();
  return true;
}

void modules::Module::parseCommands() {
  command_queue_lock_.lock();
  if (command_queue_.empty()) {
    command_queue_lock_.unlock();
    return;
  }

  while (!command_queue_.empty()) {
    GFSCommand command = command_queue_.front();
    command_queue_.pop();
    doCommand(command);
  }
  command_queue_lock_.unlock();
}