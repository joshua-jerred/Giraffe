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

#pragma once

#include <cstdint>
#include <span>
#include <string>
#include <vector>

namespace keystone {

namespace UBX {

static constexpr size_t HEADER_NUM_BYTES = 6; // sync + class + msg + length
static constexpr size_t CHECKSUM_NUM_BYTES = 2;
static constexpr uint8_t SYNC_CHAR_1 = 0xB5;
static constexpr uint8_t SYNC_CHAR_2 = 0x62;

/// @brief Combined UBX class and message IDs, MSB is class, LSB is message
/// @details Bringing them in as needed.
enum class MessageClass : uint16_t {
  UNKNOWN = 0x0000,

  // ACK
  ACK_ACK = 0x0501,
  ACK_NACK = 0x0500,

  // CFG
  // CFG_PRT = 0x0600,
  // CFG_MSG = 0x0601,
  // CFG_RESET = 0x0604,

  // NAV
  NAV_PVT = 0x0107,
};

std::string getMessageClassString(MessageClass message_class);

struct Frame {
  Frame(std::vector<uint8_t> &payload_vector_buffer)
      : payload(payload_vector_buffer) {
  }

  uint8_t sync1 = 0;
  uint8_t sync2 = 0;
  uint8_t class_id = 0;
  uint8_t msg_id = 0;
  uint16_t length = 0;
  std::vector<uint8_t> &payload;
  uint8_t ck_a = 0;
  uint8_t ck_b = 0;

  MessageClass getMessageClass() const;
};

void calculateChecksum(const UBX::Frame &frame, uint8_t &calculated_ck_a,
                       uint8_t &calculated_ck_b);

void setFrameForWrite(UBX::Frame &frame, uint8_t new_class_id,
                      uint8_t new_msg_id, std::span<uint8_t> new_payload);

void setFrameForWrite(UBX::Frame &frame, MessageClass message_class,
                      std::span<uint8_t> new_payload);

int processHeader(UBX::Frame &frame, const std::span<uint8_t> &header_buffer);

/// @brief Process a UBX payload and checksum span into a UBX frame. After
/// populating the payload and checksum fields, the is_valid field is set to
/// true if the checksums match.
/// @warning Must be called after processHeader.
/// @param frame[out] - The UBX frame to populate the payload, and checksum
/// fields.
/// @param buffer - The buffer containing the payload and checksum. Must be
/// frame.length + UBX::CHECKSUM_NUM_BYTES in size.
/// @return 0 if the payload and checksum were successfully processed, -1 if
/// the buffer size is incorrect, -2 if the checksums do not match.
int processPayloadAndChecksum(UBX::Frame &frame,
                              const std::vector<uint8_t> &buffer);

std::string getFrameString(const UBX::Frame &frame);

namespace Data {

struct NavPvt {
  uint32_t gps_iTOW = 0; // GPS Time of Week of the navigation epoch (see docs)

  uint16_t gps_utc_year = 0;
  uint8_t gps_utc_month = 0;
  uint8_t gps_utc_day = 0;
  uint8_t gps_utc_hour = 0;
  uint8_t gps_utc_minute = 0;
  uint8_t gps_utc_second = 0;

  // uint8_t gps_valid = 0; // valid bit flags
  bool gps_valid_time = false;
  bool gps_fully_resolved = false;
  bool gps_valid_magnetometer = false;
  bool gps_valid_date = false;

  uint32_t gps_time_accuracy_estimate_ns = 0;
  int32_t gps_nano = 0; // Fractional nanoseconds of second

  uint8_t fix_type = 0; // 0: no fix, 1: dead reckoning only, 2: 2D fix, 3: 3D
  // fix, 4: GPS + dead reckoning combined, 5: Time only fix

  bool gps_flags_gnss_fix_ok = false;
  bool gps_flags_differential_correction = false; // if DGPS is used
  uint8_t gps_flags_psm_state = 0;                // Power Save Mode state
  bool gps_flags_heading_of_motion = false;       // Only set in fusion mode
  uint8_t gps_flags_carr_soln = 0; // 0: no carrier phase, 1: float solution,
                                   // 2: fixed solution
  bool gps_flags_confirmed_avail = false; // valid time, date, and position
  bool gps_flags_confirmed_date = false;  // valid time and position
  bool gps_flags_confirmed_time = false;  // valid position

  uint8_t gps_num_satellites_in_solution = 0;

  int32_t gps_latitude_1e7 = 0;      // Latitude in 1e-7 degrees
  int32_t gps_longitude_1e7 = 0;     // Longitude in 1e-7 degrees
  int32_t gps_altitude_mm = 0;       // Height above Ellipsoid in mm
  int32_t gps_altitude_msl_mm = 0;   // Height above mean sea level in mm
  uint32_t gps_horz_accuracy_mm = 0; // Horizontal accuracy estimate in mm
  uint32_t gps_vert_accuracy_mm = 0; // Vertical accuracy estimate in mm

  int32_t gps_velocity_north_mm_s = 0; // Velocity North in mm/s
  int32_t gps_velocity_east_mm_s = 0;  // Velocity East in mm/s
  int32_t gps_velocity_down_mm_s = 0;  // Velocity Down in mm/s
  int32_t gps_ground_speed_mm_s = 0;   // Ground Speed (2-D) in mm/s

  int32_t gps_heading_of_motion_deg_1e5 = 0; // Heading of motion 2-D in degrees
  uint32_t gps_speed_accuracy_mm_s = 0;      // Speed accuracy estimate in mm/s
  uint32_t gps_heading_accuracy_deg_1e5 =
      0; // Heading accuracy estimate in degrees

  uint16_t gps_pDOP = 0; // Position Degree of Precision

  bool gps_flags_invalid_long_lat_height_and_hMSL = false;
  uint8_t gps_flags_last_correction_age = 0; // Age of last DGPS correction

  /// @brief Decode a UBX NavPvt frame into NAV-PVT data
  /// @param frame - The UBX frame to decode
  /// @return \c 0 if the frame was successfully decoded, non-zero on error
  int decode(const UBX::Frame &frame);

  std::string toString() const;

  double getLatitude() const {
    return (double)gps_latitude_1e7 / 1e7f;
  }

  double getLongitude() const {
    return (double)gps_longitude_1e7 / 1e7f;
  }

  double getAltitudeMsl() const {
    return (double)gps_altitude_msl_mm / 1000.0;
  }
};

} // namespace Data

} // namespace UBX

} // namespace keystone