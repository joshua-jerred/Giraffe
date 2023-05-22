/**
 * @file module_server.cpp
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief
 * @date 2023-05-16
 * @copyright Copyright (c) 2023
 */

#include "server_module.h"

#include <iostream> /** @todo remove this */

static modules::MetaData metadata("server_module",
                                  node::Identification::SERVER_MODULE, 200);

modules::ServerModule::ServerModule(data::Streams &streams,
                                    cfg::Configuration &config)
    : modules::Module(metadata, streams, config),
      request_router_(streams, config) {
}

modules::ServerModule::~ServerModule() {
}

void modules::ServerModule::startup() {
  data<std::string>("ident", "start");

  int port_number = configuration_.getServerModule().tcp_socket_port;
  if (server_socket_.init(port_number)) {
    data<std::string>("tcp_sck_init", "initialized and running on port " +
                                          std::to_string(port_number));
  }
}

void modules::ServerModule::loop() {
  sock::TcpSocketServer client;  // Create client socket
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

void modules::ServerModule::shutdown() {
  data<std::string>("ident", "stop");
  std::cout << "shutdown" << std::endl;
}

void modules::ServerModule::processCommand(const command::Command &command) {
  (void)command;
}
