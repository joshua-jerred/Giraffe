/**
 * @file module_server.cpp
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief
 * @date 2023-05-16
 * @copyright Copyright (c) 2023
 */

#include "server_module.h"

static modules::MetaData metadata("server_module",
                                  node::Identification::SERVER_MODULE, 200);

modules::ServerModule::ServerModule(data::SharedData &shared_data,
                                    cfg::Configuration &config)
    : modules::Module(metadata, shared_data, config),
      request_router_(shared_data_, config) {}

modules::ServerModule::~ServerModule() {}

void modules::ServerModule::startup() {
  int port_number = configuration_.server_module.getTcpSocketPort();
  if (server_socket_.init(port_number)) {
    info("initialized and running on port " + std::to_string(port_number));
  }
}

void modules::ServerModule::loop() {
  sock::TcpSocketServer client; // Create client socket
  /*
  The following is non-blocking. It will first "accept" the connection
  request from the client. If a client does not exist it will fail gracefully
  and exit the function.

  If the client does exist it will attempt to receive data from the client.
  Upon reception of a packet it will pass off the remaining work to the
  "requestRouter()"
  */
  if (server_socket_.accept(client)) {
    std::string request;
    if (client.receive(request)) {
      request_router_.handleRequest(client, request);
    }
  }
}

void modules::ServerModule::shutdown() {}

void modules::ServerModule::processCommand(const command::Command &command) {
  (void)command;
}
