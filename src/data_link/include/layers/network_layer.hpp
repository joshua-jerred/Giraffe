/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   network_layer.hpp
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-08-14
 * @copyright  2024 (license to be defined)
 */

#pragma once

#include <SignalEasel/aprs.hpp>

#include "gdl_packet.hpp"
#include "layers/physical_layer.hpp"

namespace giraffe::gdl {

class NetworkLayer {
public:
  NetworkLayer(Config &config, PhysicalLayer &physical_layer);
  ~NetworkLayer();

  bool txPacket(Packet &packet);

  bool rxPacket(Packet &packet);

  void update(Statistics &stats);

private:
  bool txAprsPositionPacket(const Packet &packet);

  uint32_t total_packets_sent_ = 0;
  uint32_t total_packets_received_ = 0;

  Config &config_;

  PhysicalLayer &physical_layer_;
  signal_easel::aprs::Modulator modulator_{};
  signal_easel::aprs::Receiver receiver_{};
};

} // namespace giraffe::gdl