/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   gdl_terminal.cpp
 * @brief  Giraffe Data Link Terminal
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-01-13
 * @copyright  2024 (license to be defined)
 */

#include "nlohmann/json.hpp"
#include <unordered_map>

using json = nlohmann::json;

#include "protocol.hpp"
#include <giraffe_data_link.hpp>
#include <socket.hpp>

#include "gdl_server_database.hpp"

namespace giraffe::gdl {

static const std::unordered_map<DataLink::Status, std::string> GDL_STATUS_MAP{
    {DataLink::Status::DISABLED, "DISABLED"},
    {DataLink::Status::DISCONNECTED, "DISCONNECTED"},
    {DataLink::Status::CONNECTED, "CONNECTED"},
};

/**
 * @brief The Giraffe Data Link Terminal
 * @details This contains a simple terminal for interacting with the GDL (or at
 * least it did). However, not it is primarily interfaced with the GGS via a TCP
 * socket.
 */
class GdlServer {
public:
  GdlServer() {
  }
  ~GdlServer() = default;

  int run() {
    std::cout << "GDL Server\n";

    gdl_.enable();
    server_socket_.init(9557);

    std::string received_string;
    protocol::Message received_msg;

    while (!stop_flag_) {
      if (server_socket_.accept(client_socket_)) {
        if (!client_socket_.receive(received_string)) {
          continue;
        }
        if (!protocol::parseMessage(received_string, received_msg)) {
          std::cout << "Failed to parse message: " << received_string << "\n";
          continue;
        }

        if (received_msg.typ == protocol::MessageType::REQ) {
          routeRequest(received_msg.rsc);
        } else if (received_msg.typ == protocol::MessageType::SET) {
          routeSet(received_msg);
        }
      }

      if (gdl_.messageAvailable()) {
        gdl::Message msg;
        if (gdl_.receiveMessage(msg)) {
          db_.insertMessage(msg, false);
        }
      }
    }

    gdl_.disable();

    return 0;
  }

  void routeRequest(const std::string &rsc) {
    protocol::Message msg;
    json res_data;

    if (rsc == "status") {
      gdl::Statistics stats = gdl_.getStatistics();
      res_data = {
          {"gdl_status", gdl_.getStatus() == DataLink::Status::DISABLED
                             ? "DISABLED"
                             : "ENABLED"},
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
    } else if (rsc == "config") {
      res_data = {};
    } else if (rsc == "position_packets") {
      res_data = json::array();
      auto packets = db_.getLatestPositionReports();
      for (auto &packet : packets) {
        res_data.push_back({{"latitude", packet.getLocation().latitude},
                            {"longitude", packet.getLocation().longitude},
                            {"altitude", packet.getLocation().altitude},
                            {"speed", packet.getLocation().speed},
                            {"heading", packet.getLocation().heading},
                            {"time_code", packet.getLocation().time_code}});
      }
    } else if (rsc == "received_messages") {
      res_data = db_.getLatestReceivedMessagesJson();
    } else if (rsc == "sent_messages") {
      res_data = db_.getLatestSentMessagesJson();
    } else if (rsc == "reset") {
      db_.reset();
      protocol::createResponseMessage(msg, protocol::Endpoint::GDL,
                                      protocol::Endpoint::GGS, "1",
                                      protocol::ResponseCode::GOOD, {});
      client_socket_.send(msg.getJsonString());
      return;
    } else {
      protocol::createResponseMessage(
          msg, protocol::Endpoint::GDL, protocol::Endpoint::GGS, "1",
          protocol::ResponseCode::ERROR, {{"err", "unknown resource"}});
      client_socket_.send(msg.getJsonString());
      return;
    }

    protocol::createResponseMessage(msg, protocol::Endpoint::GDL,
                                    protocol::Endpoint::GGS, "1",
                                    protocol::ResponseCode::GOOD, res_data);
    client_socket_.send(msg.getJsonString());
  }

  void routeSet(const protocol::Message &set_msg) {
    protocol::Message msg;

    if (set_msg.rsc == "new_broadcast") {
      if (!set_msg.dat.contains("data")) {
        protocol::createResponseMessage(
            msg, protocol::Endpoint::GDL, protocol::Endpoint::GGS, "1",
            protocol::ResponseCode::ERROR, {{"err", "missing data"}});
        client_socket_.send(msg.getJsonString());
        return;
      }

      std::string broadcast_data = set_msg.dat["data"];
      gdl::Message new_broadcast;
      new_broadcast.setBroadcastMessage(broadcast_data, getNewBroadcastId());
      gdl_.sendMessage(new_broadcast);
      db_.insertMessage(new_broadcast, true);

      protocol::createResponseMessage(msg, protocol::Endpoint::GDL,
                                      protocol::Endpoint::GGS, "1",
                                      protocol::ResponseCode::GOOD, {});
    } else {
      protocol::createResponseMessage(
          msg, protocol::Endpoint::GDL, protocol::Endpoint::GGS, "1",
          protocol::ResponseCode::ERROR, {{"err", "unknown resource"}});
      client_socket_.send(msg.getJsonString());
      return;
    }

    client_socket_.send(msg.getJsonString());
  }

private:
  json aprsPositionalPacketToJson(signal_easel::aprs::PositionPacket &packet) {
    auto info = packet.frame.getInformation();
    std::string raw(info.begin(), info.end());

    json res = {{"src", packet.source_address},
                {"src_ssid", packet.source_ssid},
                {"dst", packet.destination_address},
                {"dst_ssid", packet.destination_ssid},
                {"latitude", packet.latitude},
                {"longitude", packet.longitude},
                {"altitude", packet.altitude},
                {"course", packet.course},
                {"speed", packet.speed},
                {"symbol_table", packet.symbol_table},
                {"symbol", packet.symbol},
                {"comment", packet.comment},
                {"gps_time", packet.time_code},
                {"info", raw},
                {"utc_timestamp", packet.decoded_timestamp.toString()}};
    return res;
  }

  uint32_t getNewBroadcastId() {
    last_broadcast_id_++;
    return last_broadcast_id_;
  }

  bool stop_flag_{false};
  sock::TcpSocketServer server_socket_{};
  sock::TcpSocketServer client_socket_{};

  Config gdl_config_ = Config(true);
  DataLink gdl_{gdl_config_};

  GdlServerDatabase db_{};

  uint32_t last_broadcast_id_{1};
};

} // namespace giraffe::gdl

int main() {
  giraffe::gdl::GdlServer server{};
  return server.run();
}