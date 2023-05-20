#ifndef DATA_MODULE_H_
#define DATA_MODULE_H_

#include <iostream>

#include "module.h"
#include "streams.h"

namespace modules {

static MetaData metadata("test_data_module", node::Identification::DATA_MODULE);

class DataModule : public Module {
 public:
  DataModule(data::Streams &streams) : modules::Module(metadata, streams) {
  }

 private:
  void startup() override {
    data<std::string>("ident", "start");
    std::cout << "data module start" << std::endl;
  }
  void loop() override {
    data<std::string>("ident", "loop");
    std::cout << "data module" << std::endl;
  }
  void shutdown() override {
    data<std::string>("ident", "stop");
    std::cout << "shutdown" << std::endl;
  }
  void processCommand(const command::Command &command) override {
    (void)command;
  }
};

}  // namespace modules

#endif