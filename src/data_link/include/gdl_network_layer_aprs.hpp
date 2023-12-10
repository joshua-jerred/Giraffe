/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   gdl_network_layer_aprs.hpp
 * @brief  The network layer with APRS messaging for the Giraffe Data Link
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-12-10
 * @copyright  2023 (license to be defined)
 */

#ifndef GDL_NETWORK_LAYER_APRS_HPP_
#define GDL_NETWORK_LAYER_APRS_HPP_

#include <SignalEasel/aprs.hpp>

#include "gdl_network_layer.hpp"

namespace gdl {

class AprsNetworkLayer : public NetworkLayer {
public:
  AprsNetworkLayer(PhysicalLayer &physical_layer,
                   signal_easel::aprs::Packet base_packet)
      : NetworkLayer(physical_layer), base_packet_(base_packet) {
    modulator_.setBasePacket(base_packet_);
  }

  virtual ~AprsNetworkLayer() {
  }

  virtual bool txMessage(Message &message) {
    std::cout << "APRS: " << message.data << std::endl;
    signal_easel::aprs::MessagePacket packet{};
    packet.addressee = "APRS";
    packet.message = message.data;
    packet.message_id = "1";

    modulator_.encodeMessagePacket(packet);
    modulator_.writeToPulseAudio();
    modulator_.clearBuffer();
    // std::vector<uint8_t> bytes(message.data.begin(), message.data.end());
    // return physical_layer_.transmitBytes(bytes);
    return true;
  }

  virtual bool rxMessage(Message &message) {
    (void)message;
    return true;
  }

private:
  signal_easel::aprs::Packet base_packet_;
  signal_easel::aprs::Modulator modulator_{};
  signal_easel::aprs::Receiver receiver_{};
};

} // namespace gdl

#endif /* GDL_NETWORK_LAYER_APRS_HPP_ */