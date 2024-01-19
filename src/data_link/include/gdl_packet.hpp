/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   gdl_packet.hpp
 * @brief  Data Link Packet - Used in the transport and network layers
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-01-18
 * @copyright  2024 (license to be defined)
 */

#ifndef GDL_PACKET_HPP_
#define GDL_PACKET_HPP_

#include "gdl_message.hpp"

namespace giraffe::gdl {

/**
 * @brief The packet class is used in the transport and network layers. It
 * contains some extra information.
 */
class Packet : public Message {
public:
  enum class PacketType : uint8_t {
    UNKNOWN,
    BROADCAST,
    EXCHANGE,
    LOCATION,
    PING,
    PING_RESPONSE,
    ACK
  };

  Packet() = default;
  Packet(const Message &message) : Message(message) {
  }

  void setPacketType(PacketType packet_type) {
    packet_type_ = packet_type;
  }

  PacketType getPacketType() const {
    return packet_type_;
  }

  void incrementSendAttempts() {
    send_attempts_++;
  }

  uint32_t getSendAttempts() const {
    return send_attempts_;
  }

private:
  PacketType packet_type_ = PacketType::UNKNOWN;
  uint32_t send_attempts_ = 0;
};
} // namespace giraffe::gdl

#endif /* GDL_PACKET_HPP_ */