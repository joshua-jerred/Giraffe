#include <iostream>

#include <gdl_network_layer_aprs.hpp>
#include <giraffe_data_link.hpp>

int main() {
  std::cout << "Giraffe Data Link" << std::endl;
  std::string message;

  signal_easel::aprs::Packet base_packet{};

  std::cout << "Client number 1 or 2? ";
  std::cin >> message;
  if (message == "1") {
    base_packet.source_address = "GDL1";
    base_packet.destination_address = "GDL2";
  } else if (message == "2") {
    base_packet.source_address = "GDL2";
    base_packet.destination_address = "GDL1";
  } else {
    return 1;
  }

  gdl::GdlConfig config;
  config.user_id = message;

  gdl::PhysicalLayer physical_layer;
  gdl::AprsNetworkLayer network_layer{physical_layer, base_packet};
  gdl::TransportLayer transport_layer{network_layer, config};

  gdl::GiraffeDataLink gdl{config, transport_layer};
  gdl.start();

  while (true) {
    // std::cout << "Enter a message to send: " << std::endl;
    // usleep(1000);

    std::cin >> message;

    if (message == "stats") {
      gdl.getGdlStatus().print();
      continue;
    }

    gdl.exchangeMessage(message);
  }

  return 0;
}