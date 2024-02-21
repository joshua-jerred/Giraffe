/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   request_handler.cpp
 * @brief  The request handler class implementation.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-11-02
 * @copyright  2023 (license to be defined)
 */

#include <BoosterSeat/string_utils.hpp>

#include "command_ids.hpp"
#include "request_handler.hpp"

RequestRouter::RequestRouter(data::SharedData &shared_data,
                             cfg::Configuration &config,
                             data::blocks::ServerModuleStats &stats)
    : shared_data_(shared_data), config_(config), stats_(stats),
      logger_(shared_data_.streams.log, node::Identification::SERVER_MODULE,
              "RequestRouter") {
}

void RequestRouter::handleMessage(sock::TcpSocketServer &client,
                                  std::string &request) {
  stats_.num_messages_received++;
  bps_down_in_a_second_ += static_cast<double>(request.size());
  protocol::Message msg;
  // Attempt to parse as a json string
  if (!protocol::parseMessage(request, msg)) {
    sendErrorPacket(client, "malformed json");
    logger_.debug("malformed json, received: " + request);
    return;
  }

  if (msg.dst != protocol::Endpoint::GFS) {
    sendErrorPacket(client, "Endpoint it not GFS");
    return;
  }

  if (msg.typ == protocol::MessageType::REQ) {
    if (msg.rsc == "ping") {
      handlePingRequest(client, msg);
      return;
    }

    if (bst::containsPrefix(msg.rsc, "setting/")) {
      handleSettingRequest(client, msg);
      return;
    }

    if (bst::containsPrefix(msg.rsc, "data/")) {
      handleDataRequest(client, msg);
      return;
    }

    if (bst::containsPrefix(msg.rsc, "img/")) {
      handleImageRequest(client, msg.rsc);
      return;
    }
  } else if (msg.typ == protocol::MessageType::SET) {
    if (bst::containsPrefix(msg.rsc, "cmd/")) {
      handleCommandRequest(client, msg);
      return;
    } else if (bst::containsPrefix(msg.rsc, "setting/")) {
      handleSettingSet(client, msg);
      return;
    }
    sendErrorPacket(client, "message type not implemented");
    return;
  } else {
    sendErrorPacket(client, "message type not implemented");
    return;
  }

  sendErrorPacket(client, "message type or category not implemented");
}

void RequestRouter::sendErrorPacket(sock::TcpSocketServer &client,
                                    const std::string &error) {
  stats_.num_invalid_received++;
  Json body = {{"error", error}};

  protocol::Message response_message;
  protocol::createResponseMessage(response_message, protocol::Endpoint::GFS,
                                  protocol::Endpoint::GGS, "",
                                  protocol::ResponseCode::ERROR, body);

  sendMessage(response_message, client);
}

void RequestRouter::handlePingRequest(sock::TcpSocketServer &client,
                                      protocol::Message &msg) {
  protocol::Message response_message;
  protocol::createResponseMessage(response_message, protocol::Endpoint::GFS,
                                  msg.src, msg.id, protocol::ResponseCode::GOOD,
                                  {});
  sendMessage(response_message, client);
}

void RequestRouter::handleCommandRequest(sock::TcpSocketServer &client,
                                         protocol::Message &msg) {
  static constexpr int kPrefixLength = 4; // "cmd/"
  static constexpr int kMaxCommandLength = 35 + kPrefixLength;

  if (msg.rsc.length() <= kPrefixLength) {
    sendErrorPacket(client, "no command provided");
    return;
  } else if (msg.rsc.length() > kMaxCommandLength) {
    sendErrorPacket(client, "command string too long");
    return;
  }

  cmd::Command command{};
  if (!cmd::parseCommandString(msg.rsc, command)) {
    sendErrorPacket(client, "malformed command string");
    shared_data_.streams.log.error(
        node::Identification::SERVER_MODULE,
        DiagnosticId::SERVER_MODULE_invalidCommandString, msg.rsc);
    return;
  }

  shared_data_.streams.command.addCommand(node::Identification::SERVER_MODULE,
                                          command);
}

void RequestRouter::sendMessage(protocol::Message &response_json,
                                sock::TcpSocketServer &client) {
  std::string response = response_json.getJsonString();
  client.send(response);
  bps_down_in_a_second_ += static_cast<double>(response.size());
}

void RequestRouter::sendSuccessResponse(sock::TcpSocketServer &client) {
  protocol::Message response_message;
  protocol::createResponseMessage(response_message, protocol::Endpoint::GFS,
                                  protocol::Endpoint::GGS, "",
                                  protocol::ResponseCode::GOOD, {});
  sendMessage(response_message, client);
}