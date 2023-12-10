#include <iostream>

#include <gdl_network_layer_aprs.hpp>
#include <giraffe_data_link.hpp>

int main() {
  std::cout << "Giraffe Data Link" << std::endl;

  signal_easel::aprs::Packet base_packet{};
  base_packet.source_address = "SRCADR";
  base_packet.destination_address = "DSTADR";

  gdl::GdlConfig config;

  gdl::PhysicalLayer physical_layer;
  gdl::AprsNetworkLayer network_layer{physical_layer, base_packet};
  gdl::TransportLayer transport_layer{network_layer, config};

  gdl::GiraffeDataLink gdl{config, transport_layer};
  gdl.start();

  while (true) {
    std::cout << "Enter a message to send: " << std::endl;
    usleep(1000);
    std::string message;
    std::cin >> message;
    gdl.exchangeMessage(message);
  }

  return 0;
}