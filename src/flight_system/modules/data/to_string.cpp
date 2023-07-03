/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   to_string.cpp
 * @brief  Generic to_string functions for a variety of types.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-06-30
 * @copyright  2023 (license to be defined)
 */

#include "to_string.hpp"
#include "node.hpp"
#include <BoosterSeat/string_formatting.hpp>

namespace util {
std::string to_string(const data::LogPacket::Level &level) {
  switch (level) {
  case data::LogPacket::Level::DEBUG:
    return "DEBUG";
  case data::LogPacket::Level::INFO:
    return "INFO";
  case data::LogPacket::Level::WARN:
    return "WARN";
  case data::LogPacket::Level::ERROR:
    return "ERROR";
  default:
    return "UNKNOWN";
  }
}

std::string to_string(const data::LogPacket &packet) {
  std::string output = "";
  output += "LogPkt: ";
  output += to_string(packet.level) + " ";
  output += node::identification_to_string.at(packet.source) + " " +
            packet.secondary_identifier;
  output += BoosterSeat::string::intToHex(static_cast<uint16_t>(packet.id));
  output += " " + packet.info;
  return output;
}

std::string to_string(const data::DataPacket &packet) {
  std::string output = "";
  output += data::dataIdStringMap.at(packet.identifier);
  output += " - ";
  output += packet.value;
  return output;
}

} // namespace util