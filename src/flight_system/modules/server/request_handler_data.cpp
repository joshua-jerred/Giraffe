/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   request_handler_data.cpp
 * @brief  Request handler for data requests.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-02-20
 * @copyright  2024 (license to be defined)
 */

#include "request_handler.hpp"

void RequestRouter::handleDataRequest(sock::TcpSocketServer &client,
                                      protocol::Message &msg) {

  // format of msg.rec is data/<section>, so split on '/'
  std::string requested_data = msg.rsc.substr(msg.rsc.find('/') + 1);

  Json res_body;

  if (requested_data == "system_info") {
    res_body = shared_data_.blocks.system_info.get().toJson();
  } else if (requested_data == "data_log_stats") {
    res_body = shared_data_.blocks.data_log_stats.get().toJson();
  } else if (requested_data == "modules_statuses") {
    res_body = shared_data_.blocks.modules_statuses.get().toJson();
  } else if (requested_data == "stream_stats") {
    res_body = shared_data_.blocks.stream_stats.get().toJson();
  } else if (requested_data == "server_module_stats") {
    res_body = shared_data_.blocks.server_module_stats.get().toJson();
  } else if (requested_data == "extension_module_stats") {
    res_body = shared_data_.blocks.extension_module_stats.get().toJson();
  } else if (requested_data == "environmental") {
    res_body["temperature"] = toJson(shared_data_.frames.env_temp);
    res_body["pressure"] = toJson(shared_data_.frames.env_pres);
    res_body["humidity"] = toJson(shared_data_.frames.env_hum);
  } else if (requested_data == "location_data") {
    res_body = nlohmann::json(shared_data_.blocks.location_data.get());
  } else if (requested_data == "calculated_data") {
    res_body = shared_data_.blocks.calculated_data.get().toJson();
  } else if (requested_data == "telemetry_module_stats") {
    res_body = shared_data_.blocks.telemetry_module_stats.get().toJson();
  } else if (requested_data == "file_system_data") {
    res_body = shared_data_.blocks.file_system_data.get().toJson();
  } else {
    sendErrorPacket(client, "data section not found");
    return;
  }

  protocol::Message response_message;
  protocol::createResponseMessage(response_message, protocol::Endpoint::GFS,
                                  msg.src, msg.id, protocol::ResponseCode::GOOD,
                                  res_body);

  sendMessage(response_message, client);
}