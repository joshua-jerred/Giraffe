/**
 * @file module_server.cpp
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief 
 * @date 2023-05-16
 * @copyright Copyright (c) 2023
 */

#include "server_module.h"

static modules::MetaData metadata("server_module", node::Identification::SERVER_MODULE);

modules::ServerModule::ServerModule(data::Streams &streams,
                                cfg::Configuration &config)
    : modules::Module(metadata, streams, config) {
}

modules::ServerModule::~ServerModule() {

}

void modules::ServerModule::startup() {
  data<std::string>("ident", "start");
  std::cout << "server module start" << std::endl;
}

void modules::ServerModule::loop() {
  data<std::string>("ident", "loop");
}

void modules::ServerModule::shutdown() {
  data<std::string>("ident", "stop");
  std::cout << "shutdown" << std::endl;
}

void modules::ServerModule::processCommand(const command::Command &command) {
  (void)command;
}
