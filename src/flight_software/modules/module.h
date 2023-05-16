/**
 * @file module.h
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief Base module
 * @date 2023-05-16
 * @copyright Copyright (c) 2023
 */

#ifndef MODULE_H_
#define MODULE_H_

#include "streams.h"

namespace module {

struct MetaData {
  std::string name;
  data::Source source;
  int sleep_interval = 1000;  // 1 second, default sleep time
};

struct CommandQueue {
  std::mutex lock_ = std::mutex();
  //std::queue<GFSCommand> queue_ = std::queue<GFSCommand>();
};

class Module {
 public:
  Module(const module::MetaData metadata, data::Streams &streams);

  Module(const Module &) = delete;             // No copy constructor
  Module &operator=(const Module &) = delete;  // No copy assignment
  virtual ~Module();

  void start();
  void stop();

 protected:
  virtual void startup();
  virtual void loop();
  virtual void processCommand();

  void error(std::string error_code, std::string info = "");
  template <typename T>
  void data(std::string identifier, T value);

  module::MetaData metadata_;
  data::Streams &streams_;

  virtual void doCommand(GFSCommand command) {
    error("MCNI", command.id);  // Module Command Not Implemented
  };

 private:
  CommandQueue command_queue_;
};

}  // namespace module
#endif