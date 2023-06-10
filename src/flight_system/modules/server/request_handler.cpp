#include "request_handler.hpp"
#include "to_json.hpp"
#include <iostream> /** @todo remove this */

using namespace req;

/**
 * @brief Construct a new RequestRouter::RequestRouter object.
 * @param shared_data
 * @param config
 */
RequestRouter::RequestRouter(data::SharedData &shared_data,
                             cfg::Configuration &config)
    : shared_data_(shared_data), config_(config) {
}

/**
 * @brief This function is responsible for parsing a request and routing it to
 * the right place.
 *
 * @details The function handles incoming requests from a TCP socket server by
 * parsing them as JSON strings and checking their type and category before
 * sending the appropriate response or an error packet if they are not
 * implemented or malformed.
 *
 * @param client The `client` parameter is a reference to a
 * `sock::TcpSocketServer` object, which represents the client that sent the
 * request to the server.
 * @param request The `request` parameter is a reference to a `std::string`
 * object that contains the json message received from the client.
 *
 */
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
      } else if (msg.cat_ == protocol::Category::SETTING) {
        return handleSettingRequest(client, msg);
      } else if (msg.cat_ == protocol::Category::DATA) {
        return handleDataRequest(client, msg);
      }
    }

    sendErrorPacket(client, "message type or category not implemented");

  } catch (const std::exception &e) {
    sendErrorPacket(client, e.what());
  }
}

/**
 * @brief This function is the generic error response function.
 *
 * @param client The `client` is a the client socket.
 * @param error A `std::string` containing the error message.
 */
void RequestRouter::sendErrorPacket(sock::TcpSocketServer &client,
                                    const std::string &error) {
  std::string body = "{\"info\":\"" + error + "\"}";

  protocol::Message error_response(
      protocol::Endpoint::GFS, protocol::Endpoint::UNKNOWN,
      protocol::Type::RESPONSE, protocol::Category::ERROR,
      protocol::generateId(), body);

  client.send(error_response.getMessageString());
}

/**
 * @brief This function responds to a ping request.
 * @param client
 * @param msg
 */
void RequestRouter::handlePingRequest(sock::TcpSocketServer &client,
                                      protocol::Message &msg) {
  protocol::Message ping_response(protocol::Endpoint::GFS, msg.src_,
                                  protocol::Type::RESPONSE,
                                  protocol::Category::PING, msg.id_, "{}");

  client.send(ping_response.getMessageString());
}

/**
 * @brief This function responds to a setting request.
 * @details It will respond with the configuration portion requested
 * if it exists. Otherwise it will respond with an error.
 *
 * @param client
 * @param msg
 */
auto RequestRouter::handleSettingRequest(sock::TcpSocketServer &client,
                                         protocol::Message &msg) -> void {
  std::string res_body;

  json all_config;
  config_.getAllJson(all_config);
  std::cout << "body: " << msg.body_ << std::endl;

  if (all_config.contains(msg.body_)) {
    res_body = all_config[msg.body_].dump();
  } else {
    sendErrorPacket(client, "setting section not found");
    return;
  }

  protocol::Message setting_response(
      protocol::Endpoint::GFS, msg.src_, protocol::Type::RESPONSE,
      protocol::Category::SETTING, msg.id_, res_body);

  client.send(setting_response.getMessageString());
}

/**
 * @brief This function responds to data requests.
 * 
 * @param client 
 * @param msg 
 */
auto RequestRouter::handleDataRequest(sock::TcpSocketServer &client,
                                      protocol::Message &msg) -> void {
  
  std::string requested_data = "";
  try {
    requested_data = msg.body_["section"].get<std::string>();
  } catch (const std::exception &e) {
    sendErrorPacket(client, "data section malformed " + std::string(e.what()));
    return;
  }

  json res_body;
  if (requested_data == "system_info") {
    res_body = to_json(shared_data_.blocks.system_info.get());
  } else {
    sendErrorPacket(client, "data section not found");
    return;
  }

  protocol::Message data_response(protocol::Endpoint::GFS, msg.src_,
                                  protocol::Type::RESPONSE,
                                  protocol::Category::DATA, msg.id_, res_body);

  client.send(data_response.getMessageString());
}