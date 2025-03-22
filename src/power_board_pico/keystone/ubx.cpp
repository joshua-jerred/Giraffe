/// =*========GIRAFFE========*=
/// A Unified Flight Command and Control System
/// https://github.com/joshua-jerred/Giraffe
/// https://giraffe.joshuajer.red/
/// =*=======================*=
///
/// @file   new_ubx.hpp
///
/// =*=======================*=
/// @author     Joshua Jerred (https://joshuajer.red)
/// @date       2025-03-06
/// @copyright  2025 (license to be defined)

#include "ubx.hpp"

// #include <cstdint>
// #include <span>

/* U2 - Unsigned Short */
inline uint16_t parseU2(uint8_t msb, uint8_t lsb) {
  return static_cast<uint16_t>(lsb) | (static_cast<uint16_t>(msb) << 8);
}

/* U4 - Unsigned Long */
inline uint32_t parseU4(const uint8_t msb, const uint8_t mid_msb,
                        const uint8_t mid_lsb, const uint8_t lsb) {
  return (uint32_t)lsb | ((uint32_t)mid_lsb << 8) | ((uint32_t)mid_msb << 16) |
         ((uint32_t)msb << 24);
}

/* I2 - Signed Short */
inline int16_t parseI2(const uint8_t msb, const uint8_t lsb) {
  return (int16_t)(lsb | (msb << 8));
}

/* I4 - Signed Long */
inline int32_t parseI4(const uint8_t msb, const uint8_t mid_msb,
                       const uint8_t mid_lsb, const uint8_t lsb) {
  return (int32_t)(lsb | (mid_lsb << 8) | (mid_msb << 16) | (msb << 24));
}

namespace keystone {

namespace UBX {

MessageClass Frame::getMessageClass() const {
  return static_cast<MessageClass>((class_id << 8) | msg_id);
}

// struct NavPvtData {};

std::string getMessageClassString(MessageClass message_class) {
  switch (message_class) {
  case MessageClass::ACK_ACK:
    return "ACK_ACK";
  case MessageClass::ACK_NACK:
    return "ACK_NACK";
  case MessageClass::NAV_PVT:
    return "NAV_PVT";
  case MessageClass::UNKNOWN:
    return "UNKNOWN";
  default:
    return "NULL 0x" +
           std::to_string(static_cast<uint16_t>(message_class) >> 8) + " 0x" +
           std::to_string(static_cast<uint16_t>(message_class) & 0xFF);
  }
}

void calculateChecksum(const UBX::Frame &frame, uint8_t &calculated_ck_a,
                       uint8_t &calculated_ck_b) {
  uint8_t new_ck_a = 0;
  uint8_t new_ck_b = 0;

  auto addByte = [&](uint8_t byte) {
    new_ck_a += byte;
    new_ck_b += new_ck_a;
  };

  addByte(frame.class_id);
  addByte(frame.msg_id);
  addByte(frame.length & 0xFF);
  addByte((frame.length >> 8) & 0xFF);

  for (uint8_t byte : frame.payload) {
    addByte(byte);
  }

  calculated_ck_a = new_ck_a;
  calculated_ck_b = new_ck_b;
}

void setFrameForWrite(UBX::Frame &frame, uint8_t new_class_id,
                      uint8_t new_msg_id, std::span<uint8_t> new_payload) {
  frame.sync1 = SYNC_CHAR_1;
  frame.sync2 = SYNC_CHAR_2;
  frame.class_id = new_class_id;
  frame.msg_id = new_msg_id;
  frame.length = new_payload.size();
  frame.payload = std::vector<uint8_t>(new_payload.begin(), new_payload.end());
};

void setFrameForWrite(UBX::Frame &frame, MessageClass message_class,
                      std::span<uint8_t> new_payload) {
  setFrameForWrite(frame, (static_cast<uint8_t>(message_class) >> 8),
                   (static_cast<uint8_t>(message_class) & 0xFF), new_payload);
};

int processHeader(UBX::Frame &frame, const std::span<uint8_t> &header_buffer) {
  if (header_buffer.size() != HEADER_NUM_BYTES) {
    return -1;
  }

  if (header_buffer[0] != keystone::UBX::SYNC_CHAR_1 ||
      header_buffer[1] != keystone::UBX::SYNC_CHAR_2) {
    return -2;
  }

  frame.class_id = header_buffer[2];
  frame.msg_id = header_buffer[3];
  frame.length = (header_buffer[5] << 8) | header_buffer[4];

  return 0;
}

// /// @brief Process a UBX payload and checksum span into a UBX frame. After
// /// populating the payload and checksum fields, the is_valid field is set to
// /// true if the checksums match.
// /// @warning Must be called after processHeader.
// /// @param frame[out] - The UBX frame to populate the payload, and checksum
// /// fields.
// /// @param buffer - The buffer containing the payload and checksum. Must be
// /// frame.length + UBX::CHECKSUM_NUM_BYTES in size.
// /// @return 0 if the payload and checksum were successfully processed, -1 if
// /// the buffer size is incorrect, -2 if the checksums do not match.
int processPayloadAndChecksum(UBX::Frame &frame,
                              const std::vector<uint8_t> &buffer) {
  if (frame.length + UBX::CHECKSUM_NUM_BYTES != buffer.size()) {
    return -1;
  }

  frame.payload =
      std::vector<uint8_t>(buffer.begin(), buffer.begin() + frame.length);
  frame.ck_a = buffer[frame.length];
  frame.ck_b = buffer[frame.length + 1];

  uint8_t calculated_ck_a = 0;
  uint8_t calculated_ck_b = 0;
  calculateChecksum(frame, calculated_ck_a, calculated_ck_b);

  if (frame.ck_a != calculated_ck_a || frame.ck_b != calculated_ck_b) {
    return -2;
  }

  return 0;
}

std::string getFrameString(const UBX::Frame &frame) {
  std::string frame_string = "UBX Frame: ";
  frame_string += getMessageClassString(frame.getMessageClass());
  frame_string += ", Length: " + std::to_string(frame.length);
  frame_string += ", Payload: ";

  if (frame.getMessageClass() == MessageClass::ACK_NACK) {
    frame_string += "NACK[ ";
    uint8_t nack_class = frame.payload[0];
    uint8_t nack_msg = frame.payload[1];
    std::string nack_class_string = "Class: 0x" + std::to_string(nack_class);
    std::string nack_msg_string = "Msg: 0x" + std::to_string(nack_msg);
    frame_string += nack_class_string + ", " + nack_msg_string + " ]";
  } else {
    for (uint8_t byte : frame.payload) {
      frame_string += "0x";
      if (byte < 0x10) {
        frame_string += "0";
      }
      frame_string += std::to_string(byte) + " ";
    }
  }

  // Checksum
  frame_string += ", ck_a_b: 0x";
  if (frame.ck_a < 0x10) {
    frame_string += "0";
  }
  frame_string += std::to_string(frame.ck_a) + " 0x";
  if (frame.ck_b < 0x10) {
    frame_string += "0";
  }
  frame_string += std::to_string(frame.ck_b);

  return frame_string;
}

namespace Data {
int NavPvt::decode(const UBX::Frame &frame) {
  if (frame.getMessageClass() != MessageClass::NAV_PVT) {
    return -1;
  }

  if (frame.length != 92) {
    return -2;
  }

  // Decode the payload
  gps_iTOW = parseU4(frame.payload.at(3), frame.payload.at(2),
                     frame.payload.at(1), frame.payload.at(0));
  gps_utc_year = parseU2(frame.payload.at(5), frame.payload.at(4));
  gps_utc_month = frame.payload.at(6);
  gps_utc_day = frame.payload.at(7);
  gps_utc_hour = frame.payload.at(8);
  gps_utc_minute = frame.payload.at(9);
  gps_utc_second = frame.payload.at(10);

  uint8_t valid_flags = frame.payload.at(11);
  gps_valid_magnetometer = valid_flags & 0x01;
  gps_fully_resolved = valid_flags & 0x02;
  gps_valid_time = valid_flags & 0x04;
  gps_valid_date = valid_flags & 0x08;

  gps_time_accuracy_estimate_ns =
      parseU4(frame.payload.at(15), frame.payload.at(14), frame.payload.at(13),
              frame.payload.at(12));

  gps_nano = parseI4(frame.payload.at(19), frame.payload.at(18),
                     frame.payload.at(17), frame.payload.at(16));

  fix_type = frame.payload.at(21);
  uint8_t flags = frame.payload.at(22);
  gps_flags_gnss_fix_ok = flags & 0x01;
  gps_flags_differential_correction = flags & 0x02;
  gps_flags_psm_state = (flags >> 3) & 0x07;
  gps_flags_heading_of_motion = flags & 0x40;
  gps_flags_carr_soln = (flags >> 6) & 0x03;
  flags = frame.payload.at(23);
  gps_flags_confirmed_avail = flags & 0x01;
  gps_flags_confirmed_date = flags & 0x02;
  gps_flags_confirmed_time = flags & 0x04;

  gps_num_satellites_in_solution = frame.payload.at(23);

  gps_longitude_1e7 = parseI4(frame.payload.at(27), frame.payload.at(26),
                              frame.payload.at(25), frame.payload.at(24));
  gps_latitude_1e7 = parseI4(frame.payload.at(31), frame.payload.at(30),
                             frame.payload.at(29), frame.payload.at(28));
  gps_altitude_mm = parseI4(frame.payload.at(35), frame.payload.at(34),
                            frame.payload.at(33), frame.payload.at(32));
  gps_altitude_msl_mm = parseI4(frame.payload.at(39), frame.payload.at(38),
                                frame.payload.at(37), frame.payload.at(36));

  gps_horz_accuracy_mm = parseU4(frame.payload.at(43), frame.payload.at(42),
                                 frame.payload.at(41), frame.payload.at(40));
  gps_vert_accuracy_mm = parseU4(frame.payload.at(47), frame.payload.at(46),
                                 frame.payload.at(45), frame.payload.at(44));

  gps_velocity_north_mm_s = parseI4(frame.payload.at(51), frame.payload.at(50),
                                    frame.payload.at(49), frame.payload.at(48));
  gps_velocity_east_mm_s = parseI4(frame.payload.at(55), frame.payload.at(54),
                                   frame.payload.at(53), frame.payload.at(52));
  gps_velocity_down_mm_s = parseI4(frame.payload.at(59), frame.payload.at(58),
                                   frame.payload.at(57), frame.payload.at(56));

  gps_ground_speed_mm_s = parseI4(frame.payload.at(63), frame.payload.at(62),
                                  frame.payload.at(61), frame.payload.at(60));

  gps_heading_of_motion_deg_1e5 =
      parseI4(frame.payload.at(67), frame.payload.at(66), frame.payload.at(65),
              frame.payload.at(64));
  gps_speed_accuracy_mm_s = parseU4(frame.payload.at(71), frame.payload.at(70),
                                    frame.payload.at(69), frame.payload.at(68));

  gps_heading_accuracy_deg_1e5 =
      parseU4(frame.payload.at(75), frame.payload.at(74), frame.payload.at(73),
              frame.payload.at(72));

  gps_pDOP = parseU2(frame.payload.at(77), frame.payload.at(76));

  uint8_t flags3 = frame.payload.at(78);
  gps_flags_invalid_long_lat_height_and_hMSL = flags3 & 0x01;
  gps_flags_last_correction_age = (flags3 >> 1) & 0xFF;

  return 0;
}

std::string NavPvt::toString() const {
  std::string nav_pvt_string = "NAV-PVT: ";
  nav_pvt_string += "iTOW " + std::to_string(gps_iTOW) + ", ";
  nav_pvt_string += "Y-M-D h:m:s " + std::to_string(gps_utc_year);
  nav_pvt_string +=
      "-" + std::to_string(gps_utc_month) + "-" + std::to_string(gps_utc_day);
  nav_pvt_string +=
      " " + std::to_string(gps_utc_hour) + ":" + std::to_string(gps_utc_minute);
  nav_pvt_string += ":" + std::to_string(gps_utc_second) + ", ";

  // valid flags
  nav_pvt_string += "Valid [MRTD]: ";
  nav_pvt_string += gps_valid_magnetometer ? "1" : "0";
  nav_pvt_string += gps_fully_resolved ? "1" : "0";
  nav_pvt_string += gps_valid_time ? "1" : "0";
  nav_pvt_string += gps_valid_date ? "1" : "0";
  nav_pvt_string += ",\n";

  nav_pvt_string +=
      "Time Accuracy: " + std::to_string(gps_time_accuracy_estimate_ns) +
      "ns, ";

  nav_pvt_string += "Nano: " + std::to_string(gps_nano) + ", ";

  nav_pvt_string += "Fix Type: ";
  switch (fix_type) {
  case 0x00:
    nav_pvt_string += "No Fix";
    break;
  case 0x01:
    nav_pvt_string += "Dead Reckoning Only";
    break;
  case 0x02:
    nav_pvt_string += "2D Fix";
    break;
  case 0x03:
    nav_pvt_string += "3D Fix";
    break;
  case 0x04:
    nav_pvt_string += "GNSS + Dead Reckoning Combined";
    break;
  case 0x05:
    nav_pvt_string += "Time Only Fix";
    break;
  default:
    nav_pvt_string += "Unknown";
    break;
  }
  nav_pvt_string += ", ";

  nav_pvt_string += "Flags: ";
  nav_pvt_string += "gnss_fix_ok: ";
  nav_pvt_string += gps_flags_gnss_fix_ok ? "1" : "0";
  nav_pvt_string += ", differential_correction: ";
  nav_pvt_string += gps_flags_differential_correction ? "1" : "0";
  nav_pvt_string += ", psm_state: " + std::to_string(gps_flags_psm_state);
  nav_pvt_string += ", heading_of_motion: ";
  nav_pvt_string += gps_flags_heading_of_motion ? "1" : "0";
  nav_pvt_string += ", carr_soln: " + std::to_string(gps_flags_carr_soln);
  nav_pvt_string += ", confirmed_avail: ";
  nav_pvt_string += gps_flags_confirmed_avail ? "1" : "0";
  nav_pvt_string += ", confirmed_date: ";
  nav_pvt_string += gps_flags_confirmed_date ? "1" : "0";
  nav_pvt_string += ", confirmed_time: ";
  nav_pvt_string += gps_flags_confirmed_time ? "1" : "0";
  nav_pvt_string += ",";
  nav_pvt_string +=
      "Num Sats In Sol: " + std::to_string(gps_num_satellites_in_solution) +
      ",\n";

  nav_pvt_string +=
      "Longitude: " + std::to_string(gps_longitude_1e7 / 1e7f) +
      ", Latitude: " + std::to_string(gps_latitude_1e7 / 1e7f) +
      ", Altitude: " + std::to_string(gps_altitude_mm / 1000.0f) +
      "m, Altitude MSL: " + std::to_string(gps_altitude_msl_mm / 1000.0f) +
      "m,\n";
  nav_pvt_string +=
      "Acc Horz|Vert: " + std::to_string(gps_horz_accuracy_mm / 1000.0f) + "m|";
  nav_pvt_string += std::to_string(gps_vert_accuracy_mm / 1000.0f) + "m, ";
  nav_pvt_string +=
      "Ground Speed : " + std::to_string(gps_ground_speed_mm_s / 1000.0f) +
      "m/s, ";
  nav_pvt_string +=
      "Speed Acc: " + std::to_string(gps_speed_accuracy_mm_s / 1000.0f) +
      "m/s, ";

  nav_pvt_string +=
      "Velocity N|E|D: " + std::to_string(gps_velocity_north_mm_s / 1000.0f) +
      "m/s|" + std::to_string(gps_velocity_east_mm_s / 1000.0f) + "m/s|" +
      std::to_string(gps_velocity_down_mm_s / 1000.0f) + "m/s, ";
  nav_pvt_string += "Heading of Motion: " +
                    std::to_string(gps_heading_of_motion_deg_1e5 / 1e5f) +
                    "deg, ";
  nav_pvt_string +=
      "Heading Acc: " + std::to_string(gps_heading_accuracy_deg_1e5 / 1e5f) +
      "deg, ";
  nav_pvt_string += "pDOP: " + std::to_string(gps_pDOP) + ", ";
  nav_pvt_string += "Flags: ";
  nav_pvt_string += gps_flags_invalid_long_lat_height_and_hMSL ? "1" : "0";
  nav_pvt_string +=
      ", Last Correction Age: " + std::to_string(gps_flags_last_correction_age);

  return nav_pvt_string;
}

} // namespace Data

} // namespace UBX

} // namespace keystone