/**
 * @file module_server.h
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief
 * @date 2023-05-16
 * @copyright Copyright (c) 2023
 */

#ifndef SERVER_MODULE_H_
#define SERVER_MODULE_H_

#include "module.hpp"
#include "request_handler.hpp"
#include "socket.hpp"

namespace modules {

class ServerModule : public Module {
public:
  ServerModule(data::SharedData &, cfg::Configuration &);
  ~ServerModule() override = default;

private:
  void startup() override;
  void loop() override;
  void shutdown() override;
  void processCommand(const command::Command &);

  sock::TcpSocketServer server_socket_ = {};
  req::RequestRouter request_router_;
};

} // namespace modules

#endif