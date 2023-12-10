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

  bool txMessage(Message &message) override {
    // std::cout << "APRS: " << message.data << std::endl;
    signal_easel::aprs::MessagePacket packet{};
    packet.addressee = base_packet_.destination_address;
    packet.message = message.data;
    packet.message_id = message.id;

    modulator_.encodeMessagePacket(packet);
    modulator_.writeToPulseAudio();
    modulator_.clearBuffer();
    // std::vector<uint8_t> bytes(message.data.begin(), message.data.end());
    // return physical_layer_.transmitBytes(bytes);
    return true;
  }

  bool rxMessage(Message &message) override {
    signal_easel::aprs::MessagePacket message_packet{};
    signal_easel::ax25::Frame frame{};
    if (receiver_.getAprsMessage(message_packet, frame)) {
      if (frame.getSourceAddress().getAddressString() ==
          base_packet_.source_address) { // message is from us, ignore it.
        return false;
      }

      message.data = message_packet.message;
      message.id = message_packet.message_id;
      return true;
    }
    return false;
  }

  void updateNetworkLayer() override {
    receiver_.process();
    // Message message{};
    // if (rxMessage(message)) {

    // }
  }

  void updateStatus(GdlStatus &status) override {
    status.network_layer_latency_ms = receiver_.getLatency();
  }

private:
  signal_easel::aprs::Packet base_packet_;
  signal_easel::aprs::Modulator modulator_{};
  signal_easel::aprs::Receiver receiver_{};
};

} // namespace gdl

#endif /* GDL_NETWORK_LAYER_APRS_HPP_ */