/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   gdl_server_set_handlers.cpp
 * @brief  Handlers for SET requests (see gdl_server.hpp)
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-01-23
 * @copyright  2024 (license to be defined)
 */

#include "gdl_server.hpp"

namespace giraffe::gdl {

void GdlServer::handleSetNewBroadcast(const json &request_data) {
  if (!request_data.contains("data")) {
    sendResponseError("missing data for new broadcast");
    return;
  }

  std::string broadcast_data = request_data["data"];
  gdl::Message new_broadcast;
  new_broadcast.setBroadcastMessage(broadcast_data, getNewBroadcastId());
  if (sendMessage(new_broadcast)) {
    log_.info("Added new broadcast: " + broadcast_data);
    sendResponseSuccess();
  } else {
    sendResponseError("failed to send broadcast (queue full)");
  }
}

void GdlServer::handleSetConfig(const json &request_data) {
  if (!setConfigFromJson(request_data)) {
    sendResponseError("invalid config, check logs");
    return;
  }
  if (!saveConfig()) {
    sendResponseError("failed to save config, check logs");
    return;
  }

  log_.info("Config updated via SET request");
  sendResponseSuccess();
}

void GdlServer::handleSetNewExchange(const json &request_data) {
  if (!request_data.contains("data")) {
    sendResponseError("missing data for new exchange");
    log_.error("missing data for new exchange");
    return;
  }

  std::string exchange_data = request_data["data"];
  gdl::Message new_exchange;
  new_exchange.setExchangeMessage(exchange_data, getNewBroadcastId());
  if (sendMessage(new_exchange)) {
    log_.info("Added new exchange: " + exchange_data);

    sendResponseSuccess();
  } else {
    log_.error("failed to send exchange (queue full?)");
    sendResponseError("failed to send broadcast (queue full)");
  }
}

void GdlServer::handleSetDisableReceiver() {
  sendResponseError("Not Implemented");
}

void GdlServer::handleSetEnableReceiver() {
  sendResponseError("Not Implemented");
}

void GdlServer::handleSetResetConfig() {
  sendResponseError("Not Implemented");
}

void GdlServer::handleSetRestart() {
  sendResponseError("Not Implemented");
}

} // namespace giraffe::gdl