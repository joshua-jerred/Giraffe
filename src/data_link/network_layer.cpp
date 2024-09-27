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

#ifdef SSTV_ENABLED
#include "SignalEasel/sstv.hpp"
#endif

#include "SignalEasel/aprs.hpp"
#include "SignalEasel/exception.hpp"

#include "gdl_config_and_stats.hpp"
#include "gdl_packet.hpp"

#include "layers/network_layer.hpp"

namespace giraffe::gdl {

inline constexpr char EXP_PREFIX_ACK = 'a';
inline constexpr char EXP_PREFIX_BROADCAST = 'b';
inline constexpr char EXP_PREFIX_PING = 'p';
inline constexpr char EXP_PREFIX_PING_RESPONSE = 'r';
inline constexpr char EXP_PREFIX_EXCHANGE = 'e';

NetworkLayer::NetworkLayer(Config &config,
                           const std::shared_ptr<PhysicalLayer> &physical_layer)
    : config_(config), physical_layer_(physical_layer) {
}

NetworkLayer::~NetworkLayer() {
  if (physical_layer_.get() != nullptr) {
    physical_layer_->disable();
  }
}

bool NetworkLayer::txPacket(Packet &packet) {
  // Check if we are ready to transmit
  if (physical_layer_->getState() != PhysicalLayer::State::IDLE &&
      physical_layer_->getState() != PhysicalLayer::State::RECEIVING) {
    return false;
  }

  total_packets_sent_++;

  // If we're not an ASCII packet, encode transmit with APRS or SSTV
  if (packet.getPacketType() == Packet::PacketType::LOCATION) {
    return txAprsPositionPacket(packet);
  }
  if (packet.getPacketType() == Packet::PacketType::SSTV) {
    return txSstvImage(packet);
  }
  if (packet.getPacketType() == Packet::PacketType::APRS_TELEMETRY) {
    return txAprsTelemetryPacket(packet);
  }

  signal_easel::aprs::ExperimentalPacket exp_aprs_packet{};
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

  if (physical_layer_->getMode() != PhysicalLayer::Mode::DATA) {
    if (!physical_layer_->changeMode(PhysicalLayer::Mode::DATA)) {
      /// @todo report error, get rid of cout
      std::cout << "gdl nwl | failed to change mode - data" << std::endl;
      return false;
    }
  }

  if (!physical_layer_->startTransmit()) {
    /// @todo report error, get rid of cout
    std::cout << "gdl nwl | failed to start transmit" << std::endl;
    return false;
  }
  /// @todo tx start delay here
  modulator_.writeToPulseAudio();
  modulator_.clearBuffer();
  if (!physical_layer_->stopTransmit()) {
    /// @todo report error, get rid of cout
    std::cout << "gdl nwl | failed to stop transmit" << std::endl;
  }
  return true;
}

bool NetworkLayer::rxPacket(Packet &packet) {
  signal_easel::aprs::ExperimentalPacket exp_packet{};
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

    Message::Location loc{.latitude = pos_packet.latitude,
                          .longitude = pos_packet.longitude,
                          .altitude =
                              static_cast<uint32_t>(pos_packet.altitude),
                          .speed = static_cast<double>(pos_packet.speed),
                          .heading = pos_packet.course,
                          .time_code = pos_packet.time_code};

    packet.setLocation(loc);
    return true;
  }

  signal_easel::aprs::TelemetryPacket telemetry_packet{};
  if (receiver_.getAprsTelemetry(telemetry_packet, frame)) {
    total_packets_received_++;
    packet.setPacketType(Packet::PacketType::APRS_TELEMETRY);

    Message::AprsTelemetry telemetry{
        .telemetry_type = telemetry_packet.telemetry_type,
        .telemetry_data = telemetry_packet.telemetry_data};
    packet.setTelemetry(telemetry);
    return true;
  }

  signal_easel::aprs::MessagePacket message_packet{}; // throwaway
  if (receiver_.getOtherAprsPacket(frame) ||
      receiver_.getAprsMessage(message_packet, frame)) {
    return false;
  }
  return false;
}

void NetworkLayer::update(Statistics &stats) {
  receiver_.process();

  // We don't have a physical layer yet
  if (physical_layer_.get() == nullptr) {
    std::cout << "gdl nwl | physical layer is null" << std::endl;
    return;
  }

  if (physical_layer_->getState() == PhysicalLayer::State::DISABLED ||
      physical_layer_->getState() == PhysicalLayer::State::ERROR) {
    std::cout << "gdl nwl | physical layer is disabled, enabling" << std::endl;
    return physical_layer_->enable();
  }

  physical_layer_->update();

  // Network layer stats
  stats.total_packets_sent = total_packets_sent_;
  stats.total_packets_received = total_packets_received_;
  stats.network_layer_latency_ms = receiver_.getLatency();
  stats.volume = receiver_.getVolume();
  stats.signal_to_noise_ratio = receiver_.getSNR();

  // Physical layer stats
  stats.physical_layer_rssi = physical_layer_->getRssi();
  stats.physical_layer_state =
      static_cast<uint32_t>(physical_layer_->getState());

  // stats.aprs_receiver_stats = receiver_.getStats();
}

bool NetworkLayer::txAprsPositionPacket(const Packet &packet) {
  if (physical_layer_->getMode() != PhysicalLayer::Mode::DATA) {
    if (!physical_layer_->changeMode(PhysicalLayer::Mode::DATA)) {
      /// @todo report error, get rid of cout
      std::cout << "gdl nwl | failed to change mode - aprs" << std::endl;
      return false;
    }
  }

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
    if (!physical_layer_->startTransmit()) {
      /// @todo report error, get rid of cout
      std::cout << "gdl nwl | failed to start transmit" << std::endl;
      return false;
    }
    modulator_.writeToPulseAudio();
    if (!physical_layer_->stopTransmit()) {
      /// @todo report error, get rid of cout
      std::cout << "gdl nwl | failed to stop transmit" << std::endl;
    }
    modulator_.clearBuffer();
  } catch (signal_easel::Exception &e) {
    /// @todo report back up
    return false;
  }
  return true;
}

bool NetworkLayer::txSstvImage(const Packet &packet) {
#ifdef SSTV_ENABLED

  if (physical_layer_->getMode() != PhysicalLayer::Mode::DATA) {
    if (!physical_layer_->changeMode(PhysicalLayer::Mode::DATA)) {
      /// @todo report error, get rid of cout
      std::cout << "gdl nwl | failed to change mode - sstv" << std::endl;
      return false;
    }
  }

  try {
    signal_easel::sstv::Settings settings{};
    settings.overlay_call_sign = true;
    settings.mode = signal_easel::sstv::Settings::SstvMode::ROBOT36_COLOR;
    settings.call_sign = config_.getCallSign();

    signal_easel::sstv::Modulator sstv_modulator =
        signal_easel::sstv::Modulator(settings);
    sstv_modulator.encodeImage(packet.getImage().path);
    if (!physical_layer_->startTransmit()) {
      /// @todo report error, get rid of cout
      std::cout << "gdl nwl | failed to start transmit" << std::endl;
      return false;
    }
    sstv_modulator.writeToPulseAudio();

    if (!physical_layer_->stopTransmit()) {
      /// @todo report error, get rid of cout
      std::cout << "gdl nwl | failed to stop transmit" << std::endl;
    }
    sstv_modulator.clearBuffer();

  } catch (signal_easel::Exception &e) {
    /// @todo report back up
    return false;
  }
  return true;
#else
  (void)packet;
  return false;
#endif
};

bool NetworkLayer::txAprsTelemetryPacket(const Packet &packet) {
  if (physical_layer_->getMode() != PhysicalLayer::Mode::APRS) {
    if (!physical_layer_->changeMode(PhysicalLayer::Mode::APRS)) {
      /// @todo report error, get rid of cout
      std::cout << "gdl tml nwl | failed to change mode - aprs" << std::endl;
      return false;
    }
  }

  try {
    signal_easel::aprs::TelemetryPacket telem_packet{};
    telem_packet.source_address = config_.getCallSign();
    telem_packet.source_ssid = config_.getSSID();
    telem_packet.destination_address = config_.getRemoteCallSign();
    telem_packet.destination_ssid = config_.getRemoteSSID();

    auto telem = packet.getTelemetry();
    telem_packet.telemetry_data = telem.telemetry_data;
    telem_packet.telemetry_type = telem.telemetry_type;

    modulator_.encode(telem_packet);
    if (!physical_layer_->startTransmit()) {
      /// @todo report error, get rid of cout
      std::cout << "gdl tml nwl | failed to start transmit" << std::endl;
      return false;
    }
    modulator_.writeToPulseAudio();
    if (!physical_layer_->stopTransmit()) {
      /// @todo report error, get rid of cout
      std::cout << "gdl tml nwl | failed to stop transmit" << std::endl;
    }
    modulator_.clearBuffer();
  } catch (signal_easel::Exception &e) {
    /// @todo report back up
    std::cout << "gdl tml nwl | exception: " << e.what() << std::endl;
    return false;
  }
  return true;
}

} // namespace giraffe::gdl