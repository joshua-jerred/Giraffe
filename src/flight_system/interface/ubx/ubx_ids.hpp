/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   ubx_ids.hpp
 * @brief  Contains the UBX message/class IDs
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-07-04
 * @copyright  2023 (license to be defined)
 */

#ifndef UBX_IDS_HPP_
#define UBX_IDS_HPP_

#include <cstdint>
#include <string>
#include <unordered_map>

/** @todo These macros should be changed to something else */

// UBX Message IDs -------------------------------------------------------------
// Class comments above each section. Same order as data sheet
// ACK
#define UBX_ACK_ACK 0x01
#define UBX_ACK_NAK 0x00
// CFG
#define UBX_CFG_CFG 0x09 // Clear, Save, and Load Configurations
#define UBX_CFG_DAT                                                            \
  0x06 // Set User-defined Datum or get the currently defined Datum
#define UBX_CFG_GEOFENCE 0x69  // Geofencing configuration
#define UBX_CFG_GNSS 0x3E      // GNSS system configuration
#define UBX_CFG_INF 0x02       // Poll configuration for one protocol
#define UBX_CFG_ITFM 0x39      // Jamming/Interference Monitor configuration
#define UBX_CFG_LOGFILTER 0x47 // Data Logger Configuration
#define UBX_CFG_MSG 0x01  // Poll a message configuration, set message rate(s)
#define UBX_CFG_NMEA 0x17 // NMEA protocol configuration
#define UBX_CFG_ODO 0x1E  // Odometer, Low-speed COG Engine Settings
#define UBX_CFG_PMS 0x86  // Power Management Settings
#define UBX_CFG_PRT 0x00  // Port Configuration for UART, USB, SPI
#define UBX_CFG_PWR 0x57  // Put receiver in a defined power state
#define UBX_CFG_RATE 0x08 // Navigation/Measurement Rate Settings
#define UBX_CFG_RST 0x04  // Reset Receiver / Clear Backup Data
// INF
#define UBX_INF_DEBUG 0x04   // ASCII output with debug contents
#define UBX_INF_ERROR 0x00   // ASCII output with error contents
#define UBX_INF_NOTICE 0x02  // ASCII output with informational contents
#define UBX_INF_TEST 0x03    // ASCII output with test contents
#define UBX_INF_WARNING 0x01 // ASCII output with warning contents
// MON
#define UBX_MON_HW 0x09    // Hardware Status
#define UBX_MON_IO 0x02    // I/O system status
#define UBX_MON_MSGPP 0x06 // Message Parse and Process Status
#define UBX_MON_RXR 0x21   // Receiver Status Information
#define UBX_MON_VER 0x04   // Receiver/Software Version
// NAV
#define UBX_NAV_AOPSTATUS 0x60 // AssistNow Autonomous Status
#define UBX_NAV_CLOCK 0x22     // Clock Solution
#define UBX_NAV_DOP 0x04       // Dilution of precision
#define UBX_NAV_EELL 0x3D      // Position error ellipse parameters
#define UBX_NAV_EOE 0x61       // End of Epoch
#define UBX_NAV_GEOFENCE 0x39  // Geofencing status
#define UBX_NAV_ODO 0x09       // Odometer Solution
#define UBX_NAV_ORB 0x34       // GNSS Orbit Database Info
#define UBX_NAV_POSECEF 0x01   // Position Solution in ECEF
#define UBX_NAV_POSLLH 0x02    // Geodetic Position Solution
#define UBX_NAV_PVT 0x07       // Navigation Position Velocity Time Solution
#define UBX_NAV_RELPOSNED 0x3C // Relative Positioning Solution
#define UBX_NAV_RESETODO 0x10  // Reset odometer
#define UBX_NAV_SAT 0x35       // Satellite Information
#define UBX_NAV_SBAS 0x32      // SBAS Status
#define UBX_NAV_SOL 0x06       // Navigation Solution Information
#define UBX_NAV_STATUS 0x03    // Receiver Navigation Status
#define UBX_NAV_SVINFO 0x30    // Space Vehicle Information
#define UBX_NAV_TIMELS 0x26    // Leap Second Event Information
#define UBX_NAV_TIMEUTC 0x21   // UTC Time Solution
#define UBX_NAV_VELECEF 0x11   // Velocity Solution in ECEF
#define UBX_NAV_VELNED 0x12    // Velocity Solution in NED

namespace ubx {

inline constexpr uint8_t kNavClassId = 0x01;
inline constexpr uint8_t kInfClassId = 0x04;
inline constexpr uint8_t kAckClassId = 0x05;
inline constexpr uint8_t kCfgClassId = 0x06;
inline constexpr uint8_t kMonClassId = 0x0A;
inline constexpr uint8_t kMgaClassId = 0x13;

inline constexpr uint8_t kNavPvt = 0x07;

const std::unordered_map<uint8_t, std::string> UbxClassToString = {
    {kNavClassId, "NAV"}, {kInfClassId, "INF"}, {kAckClassId, "ACK"},
    {kCfgClassId, "CFG"}, {kMonClassId, "MON"}, {kMgaClassId, "MGA"}};

const std::unordered_map<uint8_t, std::string> UbxIdToString = {
    {UBX_ACK_ACK, "ACK"},
    {UBX_ACK_NAK, "NAK"},
    {UBX_CFG_CFG, "CFG"},
    {UBX_CFG_DAT, "DAT"},
    {UBX_CFG_GEOFENCE, "GEOFENCE"},
    {UBX_CFG_GNSS, "GNSS"},
    {UBX_CFG_INF, "INF"},
    {UBX_CFG_ITFM, "ITFM"},
    {UBX_CFG_LOGFILTER, "LOGFILTER"},
    {UBX_CFG_MSG, "MSG"},
    {UBX_CFG_NMEA, "NMEA"},
    {UBX_CFG_ODO, "ODO"},
    {UBX_CFG_PMS, "PMS"},
    {UBX_CFG_PRT, "PRT"},
    {UBX_CFG_PWR, "PWR"},
    {UBX_CFG_RATE, "RATE"},
    {UBX_CFG_RST, "RST"},
    {UBX_INF_DEBUG, "DEBUG"},
    {UBX_INF_ERROR, "ERROR"},
    {UBX_INF_NOTICE, "NOTICE"},
    {UBX_MON_MSGPP, "MSGPP"},
    {UBX_MON_RXR, "RXR"},
    {UBX_MON_VER, "VER"},
    {UBX_NAV_AOPSTATUS, "AOPSTATUS"},
    {UBX_NAV_CLOCK, "CLOCK"},
    {UBX_NAV_DOP, "DOP"},
    {UBX_NAV_EELL, "EELL"},
    {UBX_NAV_EOE, "EOE"},
    {UBX_NAV_GEOFENCE, "GEOFENCE"},
    {UBX_NAV_ODO, "ODO"},
    {UBX_NAV_ORB, "ORB"},
    {UBX_NAV_POSECEF, "POSECEF"},
    {UBX_NAV_POSLLH, "POSLLH"},
    {UBX_NAV_PVT, "PVT"},
    {UBX_NAV_RELPOSNED, "RELPOSNED"},
    {UBX_NAV_RESETODO, "RESETODO"},
    {UBX_NAV_SAT, "SAT"},
    {UBX_NAV_SBAS, "SBAS"},
    {UBX_NAV_SOL, "SOL"},
    {UBX_NAV_STATUS, "STATUS"},
    {UBX_NAV_SVINFO, "SVINFO"},
    {UBX_NAV_TIMELS, "TIMELS"},
    {UBX_NAV_TIMEUTC, "TIMEUTC"},
    {UBX_NAV_VELECEF, "VELECEF"},
    {UBX_NAV_VELNED, "VELNED"}};

} // namespace ubx

#endif /* UBX_IDS_HPP_ */