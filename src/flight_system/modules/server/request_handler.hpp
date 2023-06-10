#ifndef REQUEST_HANDLER_HPP_
#define REQUEST_HANDLER_HPP_

#include "configuration.hpp"
#include "protocol.h"
#include "shared_data.hpp"
#include "socket.hpp"

namespace req {
class RequestRouter {
public:
  RequestRouter(data::SharedData &, cfg::Configuration &);
  ~RequestRouter() = default;

  void handleRequest(sock::TcpSocketServer &client, std::string &request);

private:
  void sendErrorPacket(sock::TcpSocketServer &client,
                       const std::string &error = "");

  void handlePingRequest(sock::TcpSocketServer &client, protocol::Message &msg);

  void handleSettingRequest(sock::TcpSocketServer &client,
                            protocol::Message &msg);

  void handleDataRequest(sock::TcpSocketServer &client, protocol::Message &msg);

  data::SharedData &shared_data_;
  cfg::Configuration &config_;
};
} // namespace req

#endif