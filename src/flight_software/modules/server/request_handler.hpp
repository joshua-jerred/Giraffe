#ifndef REQUEST_HANDLER_HPP_
#define REQUEST_HANDLER_HPP_

#include "configuration.h"
#include "protocol.h"
#include "socket.h"
#include "streams.h"

namespace req {
class RequestRouter {
 public:
  RequestRouter(data::Streams &streams, cfg::Configuration &config);
  ~RequestRouter();

  void handleRequest(sock::TcpSocketServer &client, std::string &request);

 private:
  void sendErrorPacket(sock::TcpSocketServer &client,
                       const std::string &error = "");

  void handlePingRequest(sock::TcpSocketServer &client, protocol::Message &msg);

  data::Streams &streams_;
  cfg::Configuration &config_;
};
}  // namespace req

#endif