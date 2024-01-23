/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   gdl_server_request_handlers.cpp
 * @brief  Handlers for REQ requests (see gdl_server.hpp)
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-01-23
 * @copyright  2024 (license to be defined)
 */

#include "gdl_server.hpp"

namespace giraffe::gdl {

void GdlServer::handleRequestStatus() {
  json res_data;
  gdl::Statistics stats = gdl_.getStatistics();
  // svl_ = server layer
  // apl_ = application layer
  // tpl_ = transport layer
  // nwl_ = network layer
  // phl_ = physical layer
  res_data = {
      {"apl_status",
       gdl_.getStatus() == DataLink::Status::DISABLED ? "DISABLED" : "ENABLED"},
      {"tpl_uplink", stats.uplink_connected ? "CONNECTED" : "DISCONNECTED"},
      {"tpl_downlink", stats.downlink_connected ? "CONNECTED" : "DISCONNECTED"},
      {"apl_exchange_queue_size", stats.exchange_queue_size},
      {"apl_broadcast_queue_size", stats.broadcast_queue_size},
      {"svl_received_queue_size", stats.received_queue_size}, // data from apl
      {"nwl_messages_dropped", stats.total_messages_dropped},
      {"nwl_total_packets_received", stats.total_packets_received},
      {"nwl_total_packets_sent", stats.total_packets_sent},
      {"last_message_received", stats.total_packets_received > 0
                                    ? stats.last_message_received.toString()
                                    : "N/A"}, // {"rssi", stats.rssi},
      {"tpl_position_packets_received", stats.position_packets_received},
      {"nwl_volume", stats.volume},
      {"nwl_signal_to_noise_ratio", stats.signal_to_noise_ratio},
      {"nwl_latency_ms", stats.network_layer_latency_ms},
      {"svl_log_queue_size", log_.getLogQueueSize()},
      {"svl_send_messages_queue", sent_messages_.size()}};

  sendResponseData(res_data, "status");
}

void GdlServer::handleRequestConfig() {
  json res_data = getConfigJson();
  sendResponseData(res_data, "config");
}

void GdlServer::handleRequestReceivedMessages() {
  json res_data = json::array();

  if (!gdl_.messageAvailable()) {
    sendResponseData(res_data, "received_messages");
  }

  while (gdl_.messageAvailable()) {
    gdl::Message msg;
    if (gdl_.receiveMessage(msg)) {
      res_data.push_back(msg.getJson());
    }
  }
  sendResponseData(res_data, "received_messages");
}

void GdlServer::handleRequestSentMessages() {
}

void GdlServer::handleRequestLog() {
  json res_data = json::array();

  LoggerEntry entry;
  while (log_.popEntry(entry)) {
    res_data.push_back(
        {{"level", entry.getLevelString()},
         {"msg", entry.msg},
         {"time", fmt::format("{:%Y-%m-%d %H:%M:%S}", entry.time)}});
  }

  sendResponseData(res_data, "log");
}

} // namespace giraffe::gdl