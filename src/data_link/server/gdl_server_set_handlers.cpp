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
  if (gdl_.sendMessage(new_broadcast)) {
    sendResponseSuccess();
  } else {
    sendResponseError("failed to send broadcast (queue full)");
  }
}

} // namespace giraffe::gdl