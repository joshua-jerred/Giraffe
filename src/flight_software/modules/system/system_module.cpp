#include "system_module.hpp"

static modules::MetaData metadata("system_module", node::Identification::DATA_MODULE);

modules::SystemModule::SystemModule(data::Streams &streams,
                                cfg::Configuration &config)
    : modules::Module(metadata, streams, config) {
}

modules::SystemModule::~SystemModule() {

}

void modules::SystemModule::startup() {
  data<std::string>("ident", "start");
  std::cout << "system module start" << std::endl;
}

void modules::SystemModule::loop() {
  data<std::string>("ident", "loop");
}

void modules::SystemModule::shutdown() {
  data<std::string>("ident", "stop");
  std::cout << "shutdown" << std::endl;
}

void modules::SystemModule::processCommand(const command::Command &command) {
  (void)command;
}
