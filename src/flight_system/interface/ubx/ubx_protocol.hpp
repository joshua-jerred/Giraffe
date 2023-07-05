/**
 * @file ubx.h
 * @author Joshua Jerred (https://joshuajer.red/)
 * @brief Public interface for the u-blox protocol
 *
 * @version 0.3
 * @date 2023-01-06
 * @copyright Copyright (c) 2023
 *
 * @todo remove the macros, they're not needed
 */

#ifndef UBX_HPP_
#define UBX_HPP_

#include "i2c_interface.hpp"
#include <cstdint>
#include <vector>

namespace ubx {

enum class ACK { ACK, NACK, NONE, WRITE_ERROR, READ_ERROR };

enum class DYNAMIC_MODEL {
  PORTABLE = 0x00,
  STATIONARY = 0x02,
  PEDESTRIAN = 0x03,
  AUTOMOTIVE = 0x04,
  SEA = 0x05,
  AIRBORNE_1G = 0x06,
  AIRBORNE_2G = 0x07,
  AIRBORNE_4G = 0x08,
  ERROR = 0xFF
};

enum class FIX_TYPE {
  NO_FIX = 0x00,
  DEAD_RECK = 0x01,
  FIX_2D = 0x02,
  FIX_3D = 0x03,
  COMBINED = 0x04,
  TIME_ONLY = 0x05,
  ERROR = 0xFF
};

struct NAV_DATA {
  bool valid = false;
  int year = 0;
  int month = 0;
  int day = 0;
  int hour = 0;
  int minute = 0;
  int second = 0;

  FIX_TYPE fixType = FIX_TYPE::NO_FIX;

  int num_satellites = 0;
  double longitude = 0.0;     // degrees
  double latitude = 0.0;      // degrees
  double horz_accuracy = 0.0; // meters

  double altitude = 0.0;      // meters
  double vert_accuracy = 0.0; // meters

  double ground_speed = 0.0;   // meters/second
  double speed_accuracy = 0.0; // meters/second

  double heading_of_motion = 0.0; // degrees
  double heading_accuracy = 0.0;  // degrees
};

typedef struct UBXMessage {
  UBXMessage(){};

  /**
   * @brief Construct a new UBXMessage, if the payload is not null, the checksum
   * is calculated and stored in ck_a and ck_b. If the payload is null, the
   * checksum is not calculated, sync1, sync2, ck_a, and ck_b are set to 0.
   *
   * @param class_ID
   * @param msg_ID
   * @param length
   * @param payload
   */
  UBXMessage(std::uint8_t class_ID, std::uint8_t msg_ID, std::uint16_t length,
             std::vector<uint8_t> payload);
  bool calculateChecksum();
  bool verifyChecksum();
  std::uint8_t sync1 = 0;
  std::uint8_t sync2 = 0;
  std::uint8_t mClass = 0;
  std::uint8_t mID = 0;
  std::uint16_t length = 0;
  std::vector<std::uint8_t> payload{};
  std::uint8_t ck_a = 0;
  std::uint8_t ck_b = 0;
} UBXMessage;

int getStreamSize(I2cInterface &i2c);
bool flushStream(I2cInterface &i2c);
bool writeUBX(I2cInterface &i2c, const UBXMessage &message);
bool readNextUBX(I2cInterface &i2c, UBXMessage &message);
bool readSpecificMessage(
    I2cInterface &i2c,
    ubx::UBXMessage &message); // message should have class, id, and length set
ACK checkForAck(I2cInterface &i2c, const uint8_t msg_class,
                const uint8_t msg_id);

bool sendResetCommand(I2cInterface &i2c);

ACK setProtocolDDC(I2cInterface &i2c, const bool extended_timeout);
ACK setMessageRate(I2cInterface &i2c, uint8_t msg_class, uint8_t msg_id,
                   uint8_t rate);
ACK setMeasurementRate(I2cInterface &i2c, const uint16_t rate_ms);
ACK setDynamicModel(I2cInterface &i2c, const DYNAMIC_MODEL model);

bool pollMessage(I2cInterface &i2c, ubx::UBXMessage &message,
                 const uint8_t msg_class, const uint8_t msg_id,
                 const int expected_size, const unsigned int timeout_ms = 1500);

// UBXMessage getConfiguration
bool parsePVT(const UBXMessage &message, NAV_DATA &data);

// ---------- DEBUGGING ----------
std::ostream &operator<<(std::ostream &o, const UBXMessage &ubx);
std::ostream &operator<<(std::ostream &o, const NAV_DATA &nv);

// -------------------------------
} // namespace ubx

#endif // UBX_H_