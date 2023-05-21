#include "data_module.h"

static modules::MetaData metadata("data_module", node::Identification::DATA_MODULE);

modules::DataModule::DataModule(data::Streams &streams,
                                cfg::Configuration &config)
    : modules::Module(metadata, streams, config) {
}

modules::DataModule::~DataModule() {

}

void modules::DataModule::startup() {
  data<std::string>("ident", "start");
  std::cout << "data module start" << std::endl;
}

void modules::DataModule::loop() {
  data<std::string>("ident", "loop");
}

void modules::DataModule::shutdown() {
  data<std::string>("ident", "stop");
  std::cout << "shutdown" << std::endl;
}

void modules::DataModule::processCommand(const command::Command &command) {
  (void)command;
}
