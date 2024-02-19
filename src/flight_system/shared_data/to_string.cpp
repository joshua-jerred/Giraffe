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
#include "flight_phase.hpp"
#include "node.hpp"
#include <BoosterSeat/string_formatting.hpp>
#include <BoosterSeat/time.hpp>

namespace util {
std::string to_string(const node::Identification id) noexcept {
  try {
    return node::K_IDENTIFICATION_TO_STRING_MAP.at(id);
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
  output += bst::string::intToHex(static_cast<uint16_t>(packet.id));
  output += " " + packet.info;
  return output;
}

std::string to_string(const data::DataId id) noexcept {
  try {
    return data::K_DATA_ID_TO_STRING_MAP.at(id);
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
    return data::K_GPS_FIX_TO_STRING_MAP.at(fix_type);
  } catch (const std::out_of_range &e) {
    return "UNKNOWN";
  }
}

std::string to_string(const data::GpsFrame &frame) noexcept {
  std::string output = "";
  output += "UTC: " +
            bst::time::dateAndTimeString(bst::time::TimeZone::UTC, '-', ' ',
                                         ':', frame.gps_utc_time) +
            " ";
  output += "Local: " +
            bst::time::dateAndTimeString(bst::time::TimeZone::LOCAL, '-', ' ',
                                         ':', frame.gps_utc_time) +
            " ";
  output += to_string(frame.fix) + " ";
  output += "lat: " + bst::string::f2s(frame.latitude, 6) + " ";
  output += "lon: " + bst::string::f2s(frame.longitude, 6) + " ";
  output += "alt: " + bst::string::f2s(frame.altitude, 1) + " ";
  output += "hs:" + bst::string::f2s(frame.horizontal_speed, 1) + " ";
  return output;
}

std::string to_string(const data::GpsFramePacket &packet) noexcept {
  std::string output = "GpsFramePacket - ";
  output += packet.secondary_identifier + " | ";
  output += to_string(packet.frame);
  return output;
}

std::string to_string(const DiagnosticId diagnostic_id) noexcept {
  if (!K_DIAGNOSTIC_ID_STRING_MAP.contains(diagnostic_id)) {
    return "";
  }
  return K_DIAGNOSTIC_ID_STRING_MAP.at(diagnostic_id);
}

std::string to_string(data::DataPacket::Type type) noexcept {
  switch (type) {
  case data::DataPacket::Type::GENERIC:
    return "GENERIC";
  case data::DataPacket::Type::STATUS:
    return "STATUS";
  default:
    return "UNKNOWN";
  }
}

std::string to_string(const data::ImuFrame &frame) noexcept {
  return "rot (x, y, z): (" + std::to_string(frame.x_angular_velocity) + ", " +
         std::to_string(frame.y_angular_velocity) + ", " +
         std::to_string(frame.z_angular_velocity) + ") accel (x, y, z): (" +
         std::to_string(frame.x_acceleration) + ", " +
         std::to_string(frame.y_acceleration) + ", " +
         std::to_string(frame.z_acceleration) + ")";
}

std::string to_string(const data::ImuFramePacket &packet) noexcept {
  std::string output = "ImuFramePacket - ";
  output += packet.secondary_identifier + " | ";
  output += to_string(packet.frame);
  return output;
}

std::string to_string(FlightPhase phase) {
  switch (phase) {
  case FlightPhase::UNKNOWN:
    return "Unknown";
  case FlightPhase::LAUNCH:
    return "Launch";
  case FlightPhase::ASCENT:
    return "Ascent";
  case FlightPhase::DESCENT:
    return "Descent";
  case FlightPhase::RECOVERY:
    return "Recovery";
  default:
    return "Unknown";
  }
}

} // namespace util