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
  res_data = {
      {"gdl_status",
       gdl_.getStatus() == DataLink::Status::DISABLED ? "DISABLED" : "ENABLED"},
      {"telemetry_uplink",
       stats.uplink_connected ? "CONNECTED" : "DISCONNECTED"},
      {"telemetry_downlink",
       stats.downlink_connected ? "CONNECTED" : "DISCONNECTED"},
      {"exchange_queue_size", stats.exchange_queue_size},
      {"broadcast_queue_size", stats.broadcast_queue_size},
      {"received_queue_size", stats.received_queue_size},
      {"total_messages_dropped", stats.total_messages_dropped},
      {"total_packets_received", stats.total_packets_received},
      {"total_packets_sent", stats.total_packets_sent},
      {"last_message_received", stats.total_packets_received > 0
                                    ? stats.last_message_received.toString()
                                    : "N/A"}, // {"rssi", stats.rssi},
      {"position_packets_received", stats.position_packets_received},
      {"volume", stats.volume},
      {"signal_to_noise_ratio", stats.signal_to_noise_ratio},
      {"network_layer_latency_ms", stats.network_layer_latency_ms},
  };
  sendResponseData(res_data);
}

void GdlServer::handleRequestConfig() {
  json res_data = getConfigJson();
  sendResponseData(res_data);
}

void GdlServer::handleRequestReceivedMessages() {
  json res_data = {};
  // res_data = json::array();
  // auto packets = db_.getLatestPositionReports();
  // for (auto &packet : packets) {
  //   res_data.push_back({{"latitude", packet.getLocation().latitude},
  //                       {"longitude", packet.getLocation().longitude},
  //                       {"altitude", packet.getLocation().altitude},
  //                       {"speed", packet.getLocation().speed},
  //                       {"heading", packet.getLocation().heading},
  //                       {"time_code", packet.getLocation().time_code}});
  // }
  sendResponseData(res_data);
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

  sendResponseData(res_data);
}

} // namespace giraffe::gdl