#include <iostream>

#include "protocol.hpp"
#include <gdl_network_layer_aprs.hpp>
#include <giraffe_data_link.hpp>
#include <socket.hpp>

class GdlTerminal {
public:
  GdlTerminal() {
    gdl_.start();
    server_socket_.init(9557);
  }
  ~GdlTerminal() = default;

  int run() {
    while (stop_flag_ == false) {
      if (server_socket_.accept(client_socket_)) {
        std::string request;
        client_socket_.receive(request);
        std::cout << "Received: " << request << std::endl;

        protocol::Message msg;
        protocol::createResponseMessage(
            msg, protocol::Endpoint::GDL, protocol::Endpoint::GGS, "1",
            protocol::ResponseCode::GOOD, {{"data", "this is data"}});
        client_socket_.send(msg.getJsonString());
      }
    }
    return 0;
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