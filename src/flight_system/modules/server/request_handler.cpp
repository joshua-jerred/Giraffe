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

    if (BoosterSeat::containsPrefix(msg.rsc, "setting/")) {
      handleSettingRequest(client, msg);
      return;
    }

    if (BoosterSeat::containsPrefix(msg.rsc, "data/")) {
      handleDataRequest(client, msg);
      return;
    }
  } else if (msg.typ == protocol::MessageType::SET) {
    if (BoosterSeat::containsPrefix(msg.rsc, "cmd/")) {
      handleCommandRequest(client, msg);
      return;
    } else if (BoosterSeat::containsPrefix(msg.rsc, "setting/")) {
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

void RequestRouter::handleSettingRequest(sock::TcpSocketServer &client,
                                         protocol::Message &msg) {
  Json res_body;

  std::string req = msg.rsc.substr(msg.rsc.find('/') + 1);

  // it ain't pretty, but it works and it's faster than getting all the json at
  // once
  if (req == "general") {
    res_body = config_.general.getJson();
  } else if (req == "data_module_data") {
    res_body = config_.data_module_data.getJson();
  } else if (req == "data_module_influxdb") {
    res_body = config_.data_module_influxdb.getJson();
  } else if (req == "data_module_log") {
    res_body = config_.data_module_log.getJson();
  } else if (req == "console_module") {
    res_body = config_.console_module.getJson();
  } else if (req == "server_module") {
    res_body = config_.server_module.getJson();
  } else if (req == "system_module") {
    res_body = config_.system_module.getJson();
  } else if (req == "telemetry") {
    res_body = config_.telemetry.getJson();
  } else if (req == "telemetry_aprs") {
    res_body = config_.telemetry_aprs.getJson();
  } else if (req == "telemetry_sstv") {
    res_body = config_.telemetry_sstv.getJson();
  } else if (req == "telemetry_data_packets") {
    res_body = config_.telemetry_data_packets.getJson();
  } else if (req == "extensions") {
    res_body = config_.extensions.getJson();
  } else if (req == "extension_module") {
    res_body = config_.extension_module.getJson();
  } else if (req == "hardware_interface") {
    res_body = config_.hardware_interface.getJson();
  } else {
    sendErrorPacket(client, "setting section not found");
    return;
  }

  protocol::Message response_message;
  protocol::createResponseMessage(response_message, protocol::Endpoint::GFS,
                                  msg.src, msg.id, protocol::ResponseCode::GOOD,
                                  res_body);

  sendMessage(response_message, client);
}

void RequestRouter::handleSettingSet(sock::TcpSocketServer &client,
                                     protocol::Message &msg) {

  if (!msg.getBodyJson().contains("dat")) {
    sendErrorPacket(client, "no data provided");
    return;
  }

  std::string req = msg.rsc.substr(msg.rsc.find('/') + 1);
  Json new_cfg_data = msg.getBodyJson()["dat"];

  Json res_body = {{"success", true}};
  if (req == "general") {
    config_.general.setFromJson(new_cfg_data);
  } else {
    sendErrorPacket(client, "setting section not found");
    return;
  }

  protocol::Message response_message;
  protocol::createResponseMessage(response_message, protocol::Endpoint::GFS,
                                  msg.src, msg.id, protocol::ResponseCode::GOOD,
                                  res_body);
  sendMessage(response_message, client);
}

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