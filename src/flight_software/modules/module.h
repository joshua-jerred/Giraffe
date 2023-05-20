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
#include "node.h"
#include "streams.h"

namespace modules {

struct MetaData {
  MetaData(std::string name, node::Identification id)
      : name_(name), id_(id) {
  }

  std::string name_;
  node::Identification id_;
  // data::Source source;
  // command::Destination command_destination;
  int sleep_interval = 1000;  // 1 second, default sleep time
};

enum Status { STOPPED, STARTING, RUNNING, SLEEPING, STOPPING, ERROR };

class Module {
 public:
  Module(modules::MetaData metadata, data::Streams &streams);

  Module(const Module &) = delete;             // No copy constructor
  Module &operator=(const Module &) = delete;  // No copy assignment
  virtual ~Module(){};

  void start();
  void stop();
  modules::Status getStatus() const;

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

  void setStatus(const modules::Status status);
  void error(std::string error_code, std::string info = "");
  template <typename T>
  void data(std::string identifier, T value, int precision = 2);

  modules::MetaData metadata_;
  data::Streams &streams_;

 private:
  void runner();
  void sleep();

  std::atomic<modules::Status> status_ = modules::Status::STOPPED;
  std::atomic<bool> stop_flag_ = true;
  std::thread runner_thread_;

  command::CommandQueue command_queue_;
};

}  // namespace modules
#endif