#include "request_handler.hpp"

#include <iostream> /** @todo remove this */


using namespace req;

RequestRouter::RequestRouter(data::SharedData &shared_data, cfg::Configuration &config)
    : shared_data_(shared_data), config_(config) {
}

RequestRouter::~RequestRouter() {
}

void RequestRouter::handleRequest(sock::TcpSocketServer &client,
                                  std::string &request) {
  // Attempt to parse as a json string
  try {
    protocol::Message msg(request);
    if (msg.dst_ != protocol::Endpoint::GFS) {
      sendErrorPacket(client, "Endpoint it not GFS");
      return;
    }

    if (msg.typ_ == protocol::Type::REQUEST) {
      if (msg.cat_ == protocol::Category::PING) {
        return handlePingRequest(client, msg);
      }
    }

    sendErrorPacket(client, "message type or category not implemented");

  } catch (const std::exception &e) {
    sendErrorPacket(client, e.what());
  }
}

void RequestRouter::sendErrorPacket(sock::TcpSocketServer &client,
                                    const std::string &error) {
  std::string body = "{\"info\":\"" + error + "\"}";

  protocol::Message error_response(
      protocol::Endpoint::GFS, protocol::Endpoint::UNKNOWN,
      protocol::Type::RESPONSE, protocol::Category::ERROR,
      protocol::generateId(), body);

  client.send(error_response.getMessageString());
}

void RequestRouter::handlePingRequest(sock::TcpSocketServer &client,
                                      protocol::Message &msg) {
  protocol::Message ping_response(
      protocol::Endpoint::GFS, msg.src_,
      protocol::Type::RESPONSE, protocol::Category::PING,
      msg.id_, "{}");
  
    client.send(ping_response.getMessageString());
}
