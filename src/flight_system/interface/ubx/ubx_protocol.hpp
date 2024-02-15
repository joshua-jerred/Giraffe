/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   ubx_protocol.hpp
 * @brief  UBX Protocol implementation header
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-10-13
 * @copyright  2023 (license to be defined)
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

enum class FixType {
  NO_FIX = 0x00,
  DEAD_RECK = 0x01,
  FIX_2D = 0x02,
  FIX_3D = 0x03,
  COMBINED = 0x04,
  TIME_ONLY = 0x05,
  ERROR = 0xFF
};

struct NavData {
  bool valid = true; /// @todo currently, this really doesn't do anything
  int year = 0;
  int month = 0;
  int day = 0;
  int hour = 0;
  int minute = 0;
  int second = 0;

  FixType fix_type = FixType::NO_FIX;

  int num_satellites = 0;
  double latitude = 0.0;      // degrees
  double longitude = 0.0;     // degrees
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
  UBXMessage(uint8_t class_ID, uint8_t msg_ID, uint16_t length,
             std::vector<uint8_t> payload);

  /**
   * @brief Calculates the checksum for the message
   * @details The checksum is calculated with the Message Class, Message ID,
   * Length bytes, and Payload. The checksum is stored in ck_a and ck_b.
   *
   * @return true Checksum calculated successfully and stored in ck_a and ck_b
   * @return false Checksum could not be calculated (payload is nullptr)
   * @see 32.4 UBX Checksum of 'u-blox 8 / u-blox M8 Receiver description'
   */
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
bool parsePVT(const UBXMessage &message, NavData &data);

// ---------- DEBUGGING ----------
std::ostream &operator<<(std::ostream &o, const UBXMessage &ubx);
std::ostream &operator<<(std::ostream &o, const NavData &nv);

// -------------------------------
} // namespace ubx

#endif // UBX_H_