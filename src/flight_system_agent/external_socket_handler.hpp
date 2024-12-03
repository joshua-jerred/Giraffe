/// =*========GIRAFFE========*=
/// A Unified Flight Command and Control System
/// https://github.com/joshua-jerred/Giraffe
/// https://giraffe.joshuajer.red/
/// =*=======================*=
///
/// @file   socket_request_handler.hpp
///
/// =*=======================*=
/// @author     Joshua Jerred (https://joshuajer.red)
/// @date       2024-11-23
/// @copyright  2024 (license to be defined)

#pragma once

// common
#include "agent_network.hpp"
#include "agent_settings.hpp"
#include "i_logger.hpp"
#include "protocol.hpp"
#include "socket.hpp"

// flight_system_agent
#include "agent_data.hpp"

namespace flight_system_agent {

const std::string K_INTERNAL_COMMS_ADDRESS = "127.0.0.1";

class ExternalCommsHandler {
public:
  ExternalCommsHandler(AgentData &agent_data, AgentSettings &agent_settings,
                       giraffe::ILogger &logger)
      : agent_data_(agent_data), agent_settings_(agent_settings),
        logger_(logger) {
  }

  ~ExternalCommsHandler() = default;

  ExternalCommsHandler(const ExternalCommsHandler &) = delete;
  ExternalCommsHandler &operator=(const ExternalCommsHandler &) = delete;
  ExternalCommsHandler(ExternalCommsHandler &&) = delete;

  bool enable() {
    // Try to bind to the port
    if (!socket_.init(DAEMON_PORT)) {
      logger_.error("Failed to bind to port as new agent.");
      return false;
    }

    logger_.info("Bound to port as new agent.");
    return true;
  }

  void process() {
    sock::TcpSocketServer client; // Create client socket

    // Check for a new connection (non-blocking)
    if (socket_.accept(client)) {
      std::string request;
      if (client.receive(request)) {
        // Helps with hanging on shutdown
        if (agent_data_.isAgentStopRequested()) {
          client.close();
          return;
        }

        std::string response{};
        handleExchange(request, response);
        if (!client.send(response)) {
          logger_.error("Failed to send response");
        }
        client.close();
      }
    }
  }

  /// @brief Bit confusing, but this is called by the CLI. It connects to the
  /// agent.
  /// @param response - The response to send back to the CLI
  /// @param type - The type of message to send (SET or REQ)
  /// @param resource - The resource code to send (e.g. "status")
  /// @param logger - The logger to use
  /// @param exchange_data - The data to send with the request
  /// @return \c true if the transaction was successful, \c false otherwise
  static bool sendExchangeToAgent(protocol::Message &response,
                                  protocol::MessageType type,
                                  const std::string &resource,
                                  giraffe::ILogger &logger,
                                  Json exchange_data = Json::object()) {
    protocol::Message message;
    if (type == protocol::MessageType::REQ) {
      protocol::createRequestMessage(message, protocol::Endpoint::FSA,
                                     protocol::Endpoint::FSA, resource);
    } else if (type == protocol::MessageType::SET) {
      protocol::createSetMessage(message, protocol::Endpoint::FSA,
                                 protocol::Endpoint::FSA, resource,
                                 exchange_data);
    } else {
      logger.error("Unknown message type: " +
                   std::to_string(static_cast<int>(type)));
      return false;
    }

    sock::TcpSocketClient client(logger);
    std::string response_string;
    if (!client.connect(K_INTERNAL_COMMS_ADDRESS, DAEMON_PORT)) {
      logger.error("Failed to connect to agent.");
      return false;
    }

    if (!client.transaction(message.getJsonString(), response_string)) {
      logger.error("Failed to send message to agent.");
      return false;
    }

    if (!protocol::parseMessage(response_string, response)) {
      logger.error("Failed to parse response from agent.");
      return false;
    }

    return true;
  }

private:
  void sendErrorResponse(std::string &response_json,
                         const std::string &error_message,
                         protocol::Endpoint dst = protocol::Endpoint::UNKNOWN,
                         protocol::MessageId id = "") {
    protocol::Message message;
    createResponseMessage(message, protocol::Endpoint::FSA, dst, id,
                          protocol::ResponseCode::ERROR,
                          {{"error", error_message}});
    response_json = message.getJsonString();
  }

  void sendOkResponse(std::string &response_json, protocol::Endpoint dst,
                      protocol::MessageId id) {
    protocol::Message message;
    createResponseMessage(message, protocol::Endpoint::FSA, dst, id,
                          protocol::ResponseCode::GOOD, Json::object());
    response_json = message.getJsonString();
  }

  void handleExchange(const std::string &request, std::string &response) {
    try {

      protocol::Message message;
      if (!protocol::parseMessage(request, message)) {
        logger_.error("Failed to parse message. Request: " + request);
        sendErrorResponse(response, "Failed to parse message.");
        return;
      }

      if (message.typ == protocol::MessageType::REQ) {
        handleRequest(message, response);
      } else if (message.typ == protocol::MessageType::SET) {
        handleSet(message, response);
      } else {
        logger_.error("Unknown message type: " + message.getJsonString());
        sendErrorResponse(response, "Unknown message type.");
        return;
      }
    } catch (const std::exception &e) {
      logger_.error("Exception in handleExchange: " + std::string(e.what()));
    }

    logger_.debug("Sending response: " + response);
  }

  void handleRequest(const protocol::Message &message, std::string &response) {
    protocol::Message response_message;
    bool handled = false;
    const std::string &requested_resource = message.rsc;

    if (requested_resource == "status") {
      Json body{};
      agent_data_.getStatusJson(body);
      createResponseMessage(response_message, protocol::Endpoint::FSA,
                            message.src, message.id,
                            protocol::ResponseCode::GOOD, body);
      handled = true;
    } else if (requested_resource == "settings") {
      Json body{};
      agent_settings_.getSettingsJson(body);
      createResponseMessage(response_message, protocol::Endpoint::FSA,
                            message.src, message.id,
                            protocol::ResponseCode::GOOD, body);
      handled = true;
    }

    if (handled) {
      response = response_message.getJsonString();
    } else {
      logger_.error("Unknown request: " + response_message.getJsonString());
      sendErrorResponse(response, "Unknown request.", message.src, message.id);
    }
  }

  void handleSet(const protocol::Message &message, std::string &response) {
    const std::string &requested_resource = message.rsc;

    if (requested_resource == "stop") {
      agent_data_.setAgentStopFlag(true);
      sendOkResponse(response, message.src, message.id);
      return;
    } else if (requested_resource == "settings") {
      if (message.dat.is_object()) {
        if (!agent_settings_.setFromJson(message.dat)) {
          sendErrorResponse(response, "Failed to some or all settings.",
                            message.src, message.id);
          return;
        }

        sendOkResponse(response, message.src, message.id);
        return;
      }

      sendErrorResponse(response, "Settings must be an object.", message.src,
                        message.id);
      return;
    }

    logger_.error("Unknown set: " + message.getJsonString());
    sendErrorResponse(response, "Unknown set.", message.src, message.id);
  }

  AgentData &agent_data_;
  AgentSettings &agent_settings_;
  giraffe::ILogger &logger_;

  sock::TcpSocketServer socket_{};
};

} // namespace flight_system_agent