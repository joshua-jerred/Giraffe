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

#include "SignalEasel/aprs.hpp"
#include "SignalEasel/exception.hpp"

#include "gdl_config_and_stats.hpp"
#include "gdl_layers.hpp"
#include "gdl_packet.hpp"

namespace giraffe::gdl {

inline constexpr char EXP_PREFIX_ACK = 'a';
inline constexpr char EXP_PREFIX_BROADCAST = 'b';
inline constexpr char EXP_PREFIX_PING = 'p';
inline constexpr char EXP_PREFIX_PING_RESPONSE = 'r';
inline constexpr char EXP_PREFIX_EXCHANGE = 'e';

NetworkLayer::NetworkLayer(Config &config, PhysicalLayer &physical_layer)
    : config_(config), physical_layer_(physical_layer) {
  physical_layer_.enable();
}

NetworkLayer::~NetworkLayer() {
}

bool NetworkLayer::txPacket(Packet &packet) {
  total_packets_sent_++;
  if (packet.getPacketType() == Packet::PacketType::LOCATION) {
    return txAprsPositionPacket(packet);
  }

  signal_easel::aprs::Experimental exp_aprs_packet{};
  exp_aprs_packet.source_address = config_.getCallSign();
  exp_aprs_packet.source_ssid = config_.getSSID();
  exp_aprs_packet.destination_address = config_.getRemoteCallSign();
  exp_aprs_packet.destination_ssid = config_.getRemoteSSID();

  switch (packet.getPacketType()) {
  case Packet::PacketType::ACK:
    exp_aprs_packet.packet_type_char = EXP_PREFIX_ACK;
    break;
  case Packet::PacketType::BROADCAST:
    exp_aprs_packet.packet_type_char = EXP_PREFIX_BROADCAST;
    break;
  case Packet::PacketType::PING:
    exp_aprs_packet.packet_type_char = EXP_PREFIX_PING;
    break;
  case Packet::PacketType::EXCHANGE:
    exp_aprs_packet.packet_type_char = EXP_PREFIX_EXCHANGE;
    break;
  case Packet::PacketType::PING_RESPONSE:
    exp_aprs_packet.packet_type_char = EXP_PREFIX_PING_RESPONSE;
    break;
  default:
    return false;
  }

  // add the identifier bytes
  auto ident = packet.getIdentifierString();
  exp_aprs_packet.data = std::vector<uint8_t>(ident.begin(), ident.end());

  // add the data bytes if applicable
  switch (packet.getPacketType()) {
  case Packet::PacketType::BROADCAST:
  case Packet::PacketType::EXCHANGE:
    for (auto &character : packet.getData()) {
      exp_aprs_packet.data.push_back(character);
    }
    break;
  default:
    break;
  }

  modulator_.encode(exp_aprs_packet);
  modulator_.writeToPulseAudio();
  modulator_.clearBuffer();
  return true;
}

bool NetworkLayer::rxPacket(Packet &packet) {
  signal_easel::aprs::Experimental exp_packet{};
  signal_easel::ax25::Frame frame{};

  if (receiver_.getAprsExperimental(exp_packet, frame)) {
    total_packets_received_++;

    std::string received_source_call_sign =
        frame.getSourceAddress().getAddressString();
    uint8_t received_source_ssid = frame.getSourceAddress().getSsid();

    if (received_source_call_sign == config_.getCallSign() &&
        received_source_ssid == config_.getSSID()) {
      // message is from us, ignore it.
      // std::cout << "ignoring packet from us" << std::endl;
      return false;
    }

    switch (exp_packet.packet_type_char) {
    case EXP_PREFIX_ACK:
      packet.setPacketType(Packet::PacketType::ACK);
      break;
    case EXP_PREFIX_BROADCAST:
      packet.setPacketType(Packet::PacketType::BROADCAST);
      break;
    case EXP_PREFIX_PING:
      packet.setPacketType(Packet::PacketType::PING);
      break;
    case EXP_PREFIX_EXCHANGE:
      packet.setPacketType(Packet::PacketType::EXCHANGE);
      break;
    case EXP_PREFIX_PING_RESPONSE:
      packet.setPacketType(Packet::PacketType::PING_RESPONSE);
      break;
    default:
      std::cout << "unknown exp packet type" << std::endl;
      return false;
    }

    if (exp_packet.data.size() < 8) {
      std::cout << "invalid exp packet" << std::endl;
      return false;
    }

    std::string data_str = exp_packet.getStringData();
    if (!packet.setIdentifierFromHex(data_str.substr(0, 8))) {
      std::cout << "invalid exp packet" << std::endl;
      return false;
    }

    if (exp_packet.data.size() > 8) {
      packet.setData(data_str.substr(8));
    }

    return true;
  }

  signal_easel::aprs::PositionPacket pos_packet{};
  if (receiver_.getAprsPosition(pos_packet, frame)) {
    total_packets_received_++;
    packet.setPacketType(Packet::PacketType::LOCATION);

    Packet::Location loc{.latitude = pos_packet.latitude,
                         .longitude = pos_packet.longitude,
                         .altitude = static_cast<uint32_t>(pos_packet.altitude),
                         .speed = static_cast<double>(pos_packet.speed),
                         .heading = pos_packet.course,
                         .time_code = pos_packet.time_code};

    packet.setLocation(loc);
    return true;
  }

  signal_easel::aprs::MessagePacket message_packet{}; // throwaway
  if (receiver_.getOtherAprsPacket(frame) ||
      receiver_.getAprsMessage(message_packet, frame)) {
    return false;
  }
  return false;
}

bool NetworkLayer::txAprsPositionPacket(const Packet &packet) {
  try {
    signal_easel::aprs::PositionPacket pos_packet{};
    pos_packet.source_address = config_.getCallSign();
    pos_packet.source_ssid = config_.getSSID();
    pos_packet.destination_address = config_.getRemoteCallSign();
    pos_packet.destination_ssid = config_.getRemoteSSID();

    auto loc = packet.getLocation();
    pos_packet.latitude = loc.latitude;
    pos_packet.longitude = loc.longitude;
    pos_packet.altitude = loc.altitude;
    pos_packet.speed = loc.speed;
    pos_packet.course = loc.heading;
    pos_packet.time_code = loc.time_code;

    modulator_.encode(pos_packet);
    modulator_.writeToPulseAudio();
    modulator_.clearBuffer();
  } catch (signal_easel::Exception &e) {
    /// @todo report back up
    return false;
  }
  return true;
}

void NetworkLayer::update(Statistics &stats) {
  receiver_.process();
  physical_layer_.update();

  stats.total_packets_sent = total_packets_sent_;
  stats.total_packets_received = total_packets_received_;
  stats.network_layer_latency_ms = receiver_.getLatency();
  stats.volume = receiver_.getVolume();
  stats.signal_to_noise_ratio = receiver_.getSNR();
  // stats.aprs_receiver_stats = receiver_.getStats();
}

} // namespace giraffe::gdl