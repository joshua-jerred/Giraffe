#include "data_module.h"

static modules::MetaData metadata("data_module", node::Identification::DATA_MODULE);

modules::DataModule::DataModule(data::Streams &streams,
                                cfg::Configuration &config)
    : modules::Module(metadata, streams, config) {
}

modules::DataModule::~DataModule() {

}

void modules::DataModule::startup() {

}

void modules::DataModule::loop() {
  data<std::string>("ident", "loop");
}

void modules::DataModule::shutdown() {

}

void modules::DataModule::processCommand(const command::Command &command) {
  (void)command;
}
