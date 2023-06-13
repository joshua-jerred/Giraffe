/**
 * @file module_server.cpp
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief
 * @date 2023-05-16
 * @copyright Copyright (c) 2023
 */

#include "server_module.h"

static modules::MetaData metadata("server_module",
                                  node::Identification::SERVER_MODULE, 50);

modules::ServerModule::ServerModule(data::SharedData &shared_data,
                                    cfg::Configuration &config)
    : modules::Module(metadata, shared_data, config),
      request_router_(shared_data_, config, stats_) {
}

/**
 * @brief Startup the server module.
 * @details Reads the socket port from the configuration and initializes the
 * socket with it. Override of Module::startup()
 */
void modules::ServerModule::startup() {
  int port_number = configuration_.server_module.getTcpSocketPort();
  if (server_socket_.init(port_number)) {
    info("initialized and running on port " + std::to_string(port_number));
  }
  rolling_average_timer_.reset();
  connected_timeout_.reset();
}

/**
 * @brief Loop module function for the server module.
 * @details Connects to clients and handles the requests. Override of
 * Module::loop()
 */
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
      if (stopRequested()) { // Helps with hanging on shutdown
        client.close();
        return;
      }
      request_router_.handleMessage(client, request);
      connected_timeout_.reset();
      stats_.is_connected = true;
      client.close();
    }
  }

  if (connected_timeout_.isDone()) {
    stats_.is_connected = false;
  }

  // Update the rolling average every second and update the stats
  if (rolling_average_timer_.isDone()) {
    rolling_average_timer_.reset();
    request_router_.oneSecondTick();
    stats_.bytes_per_second_down = request_router_.getBytesPerSecondDown();
    stats_.bytes_per_second_up = request_router_.getBytesPerSecondUp();

    shared_data_.blocks.server_module_stats.set(stats_);
  }
}

/**
 * @brief Shutdown the server module.
 * @details Closes the socket. Override of Module::shutdown()
 */
void modules::ServerModule::shutdown() {
}

void modules::ServerModule::processCommand(const command::Command &command) {
  (void)command;
}
