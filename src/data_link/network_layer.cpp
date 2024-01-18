/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   network_layer.cpp
 * @brief  GDL Network layer implementation
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-01-18
 * @copyright  2024 (license to be defined)
 */

#include "giraffe_data_link.hpp"

namespace giraffe {

GiraffeDataLink::NetworkLayer::NetworkLayer(PhysicalLayer &physical_layer)
    : physical_layer_(physical_layer) {
  physical_layer_.enable();
  modulator_.setBasePacket(base_packet_);
}

GiraffeDataLink::PhysicalLayer::~PhysicalLayer() {
}

bool GiraffeDataLink::NetworkLayer::txMessage(Message &message) {
  signal_easel::aprs::MessagePacket packet{};
  packet.addressee = base_packet_.destination_address;
  packet.message = message.data;
  packet.message_id = message.id;

  modulator_.encodeMessagePacket(packet);
  modulator_.writeToPulseAudio();
  modulator_.clearBuffer();
  return true;
}

bool GiraffeDataLink::NetworkLayer::rxMessage(Message &message) {
  signal_easel::aprs::MessagePacket message_packet{};
  signal_easel::ax25::Frame frame{};
  if (receiver_.getAprsMessage(message_packet, frame)) {
    std::cout << "APRS Message: " << message_packet.message << std::endl;
    if (frame.getSourceAddress().getAddressString() ==
        base_packet_.source_address) { // message is from us, ignore it.
      return false;
    }

    message.data = message_packet.message;
    message.id = message_packet.message_id;
    return true;
  } else if (receiver_.getOtherAprsPacket(frame)) {
    std::cout << "APRS Other Packet" << std::endl;
    return false;
  }
  return false;
}

bool GiraffeDataLink::NetworkLayer::txAprsPositionPacket(
    signal_easel::aprs::PositionPacket &packet) {
  try {
    modulator_.encodePositionPacket(packet);
    modulator_.writeToPulseAudio();
    modulator_.clearBuffer();
  } catch (signal_easel::Exception &e) {
    /// @todo report back up
    return false;
  }
  return true;
}

bool GiraffeDataLink::NetworkLayer::rxAprsPositionPacket(
    signal_easel::aprs::PositionPacket &packet,
    signal_easel::ax25::Frame &frame) {
  if (receiver_.getAprsPosition(packet, frame)) {
    return true;
  }
  return false;
}

void GiraffeDataLink::NetworkLayer::updateNetworkLayer() {
  receiver_.process();
}

void GiraffeDataLink::NetworkLayer::updateStatus(GdlStatus &status) {
  status.network_layer_latency_ms = receiver_.getLatency();
  status.volume = receiver_.getVolume();
  status.signal_to_noise_ratio = receiver_.getSNR();
  status.aprs_receiver_stats = receiver_.getStats();
}

void GiraffeDataLink::NetworkLayer::update() {
  receiver_.process();
  physical_layer_.update();
}

} // namespace giraffe