#include "configuration_module.h"
using namespace modules;

modules::MetaData config_module_metadata(
    "configuration", node::Identification::CONFIGURATION_MODULE);

ConfigurationModule::ConfigurationModule(data::Streams &streams)
    : modules::Module(config_module_metadata, streams) {
}

void ConfigurationModule::startup() {
  data<std::string>("ident", "start");
  std::cout << "config module start" << std::endl;
}

void ConfigurationModule::shutdown() {
  data<std::string>("ident", "start");
  std::cout << "config module shutdown" << std::endl;
}

void ConfigurationModule::loop() {
  data<std::string>("ident", "start");
  std::cout << "config module loop" << std::endl;
}

void ConfigurationModule::processCommand(const command::Command &command) {
  (void)command;
}