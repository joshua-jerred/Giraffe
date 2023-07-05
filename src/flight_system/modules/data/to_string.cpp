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
#include <BoosterSeat/time.hpp>

namespace util {
std::string to_string(const node::Identification id) noexcept {
  try {
    return node::identification_to_string.at(id);
  } catch (const std::out_of_range &e) {
    return "UNKNOWN";
  }
}

std::string to_string(const data::LogPacket::Level level) noexcept {
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

std::string to_string(const data::LogPacket &packet) noexcept {
  std::string output = "";
  output += "LogPkt: ";
  output += to_string(packet.level) + " ";
  output += to_string(packet.source) + " ";
  if (!packet.secondary_identifier.empty()) {
    output += " " + packet.secondary_identifier + " ";
  }
  output += BoosterSeat::string::intToHex(static_cast<uint16_t>(packet.id));
  output += " " + packet.info;
  return output;
}

std::string to_string(const data::DataId id) noexcept {
  try {
    return data::dataIdStringMap.at(id);
  } catch (const std::out_of_range &e) {
    return "UNKNOWN";
  }
}

std::string to_string(const data::DataPacket &packet) noexcept {
  std::string output = "";
  output += to_string(packet.identifier) + " - ";
  output += packet.value;
  return output;
}

std::string to_string(const data::GpsFix fix_type) noexcept {
  try {
    return data::GpsFixStringMap.at(fix_type);
  } catch (const std::out_of_range &e) {
    return "UNKNOWN";
  }
}

std::string to_string(const data::GpsFrame &frame) noexcept {
  std::string output = "";
  output +=
      "UTC: " +
      BoosterSeat::time::dateAndTimeString(BoosterSeat::time::TimeZone::UTC,
                                           '-', ' ', ':', frame.gps_utc_time) +
      " ";
  output +=
      "Local: " +
      BoosterSeat::time::dateAndTimeString(BoosterSeat::time::TimeZone::LOCAL,
                                           '-', ' ', ':', frame.gps_utc_time) +
      " ";
  output += to_string(frame.fix) + " ";
  output += "lat: " + BoosterSeat::string::f2s(frame.latitude, 6) + " ";
  output += "lon: " + BoosterSeat::string::f2s(frame.longitude, 6) + " ";
  output += "alt: " + BoosterSeat::string::f2s(frame.altitude, 1) + " ";
  output += "hs:" + BoosterSeat::string::f2s(frame.horizontal_speed, 1) + " ";
  return output;
}

std::string to_string(const data::GpsFramePacket &packet) noexcept {
  std::string output = "";
  output += packet.secondary_identifier + " | ";
  output += to_string(packet.frame);
  return output;
}

} // namespace util