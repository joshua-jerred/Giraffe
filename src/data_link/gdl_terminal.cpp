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

#include <unordered_map>

#include "protocol.hpp"
#include <gdl_network_layer_aprs.hpp>
#include <giraffe_data_link.hpp>
#include <socket.hpp>

#include "nlohmann/json.hpp"
using json = nlohmann::json;

static const std::unordered_map<gdl::GiraffeDataLink::ConnectionStatus,
                                std::string>
    GDL_CONNECTION_STATE_MAP{
        {gdl::GiraffeDataLink::ConnectionStatus::CONNECTED, "CONNECTED"},
        {gdl::GiraffeDataLink::ConnectionStatus::DISCONNECTED, "DISCONNECTED"}};

static const std::unordered_map<gdl::GiraffeDataLink::Status, std::string>
    GDL_STATUS_MAP{{gdl::GiraffeDataLink::Status::ERROR, "ERROR"},
                   {gdl::GiraffeDataLink::Status::STOPPED, "STOPPED"},
                   {gdl::GiraffeDataLink::Status::STARTING, "STARTING"},
                   {gdl::GiraffeDataLink::Status::RUNNING, "RUNNING"},
                   {gdl::GiraffeDataLink::Status::STOPPING, "STOPPING"}};

/**
 * @brief The Giraffe Data Link Terminal
 * @details This contains a simple terminal for interacting with the GDL (or at
 * least it did). However, not it is primarily interfaced with the GGS via a TCP
 * socket.
 */
class GdlTerminal {
public:
  GdlTerminal() {
    gdl_.start();
    server_socket_.init(9557);
  }
  ~GdlTerminal() = default;

  int run() {
    std::string received_string;
    protocol::Message received_msg;

    while (stop_flag_ == false) {
      if (server_socket_.accept(client_socket_)) {
        if (!client_socket_.receive(received_string)) {
          continue;
        }
        if (!protocol::parseMessage(received_string, received_msg)) {
          continue;
        }

        if (received_msg.typ == protocol::MessageType::REQ) {
          routeRequest(received_msg.rsc);
        }
      }
    }
    return 0;
  }

  void routeRequest(const std::string &rsc) {
    protocol::Message msg;
    json res_data;

    if (rsc == "status") {
      auto stats = gdl_.getGdlStatus();
      res_data = {
          {"gdl_status", GDL_STATUS_MAP.at(gdl_.getStatus())},
          {"telemetry_uplink",
           GDL_CONNECTION_STATE_MAP.at(gdl_.getUplinkStatus())},
          {"telemetry_downlink",
           GDL_CONNECTION_STATE_MAP.at(gdl_.getDownlinkStatus())},
          {"exchange_queue_size", stats.exchange_queue_size},
          {"broadcast_queue_size", stats.broadcast_queue_size},
          {"received_queue_size", stats.received_queue_size},
          {"volume", stats.volume},
          {"signal_to_noise_ratio", stats.signal_to_noise_ratio},
          {"network_layer_latency_ms", stats.network_layer_latency_ms},
      };
    } else if (rsc == "config") {
      res_data = {};
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

private:
  bool stop_flag_{false};

  signal_easel::aprs::Packet base_aprs_packet_{};
  gdl::GdlConfig gdl_config_{};

  sock::TcpSocketServer server_socket_{};
  sock::TcpSocketServer client_socket_{};

  // layers
  gdl::PhysicalLayer physical_layer_{};
  gdl::AprsNetworkLayer network_layer_{physical_layer_, base_aprs_packet_};
  gdl::TransportLayer transport_layer_{network_layer_, gdl_config_};
  gdl::GiraffeDataLink gdl_{gdl_config_, transport_layer_};
};

int main() {
  GdlTerminal terminal{};
  return terminal.run();
}