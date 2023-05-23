/**
 * @file module.h
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief Base module
 * @date 2023-05-16
 * @copyright Copyright (c) 2023
 */

#ifndef MODULE_H_
#define MODULE_H_

#include <atomic>
#include <thread>

#include "command.h"
#include "configuration.h"
#include "node.h"
#include "shared_data.hpp"

namespace modules {

struct MetaData {
  MetaData(std::string name, node::Identification id, int sleep_interval = 1000)
      : name_(name), id_(id), sleep_interval_(sleep_interval) {
  }

  const std::string name_;
  const node::Identification id_;
  // data::Source source;
  // command::Destination command_destination;
  int sleep_interval_;  // 1 second, default sleep time
};

class Module {
 public:
  Module(modules::MetaData &, data::SharedData &, cfg::Configuration &);

  Module(const Module &) = delete;             // No copy constructor
  Module &operator=(const Module &) = delete;  // No copy assignment
  virtual ~Module(){};

  void start();
  void stop();
  node::Status getStatus() const;

 protected:
  virtual void startup() {
  }
  virtual void loop() {
  }
  virtual void shutdown() {
  }
  virtual void processCommand(const command::Command &command) {
    (void)command;
  }

  void setStatus(const node::Status status);
  void error(std::string error_code, std::string info = "");
  template <typename T>
  void data(std::string identifier, T value, int precision = 2);

  modules::MetaData metadata_;
  data::SharedData &shared_data_;
  cfg::Configuration &configuration_;

  void sleep();

 private:
  void runner();

  std::atomic<node::Status> status_ = node::Status::UNKNOWN;
  std::atomic<bool> stop_flag_ = true;
  std::thread runner_thread_;

  command::CommandQueue command_queue_;
};

}  // namespace modules
#endif