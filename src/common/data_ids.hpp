/**
 *
 * 
 * &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
 * AUTOMATICALLY GENERATED, DO NOT EDIT MANUALLY
 * &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
 *
 *
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 * 
 * 
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-06-30
 * @copyright  2023 (license to be defined)
 * =*=======================*=
 *
 * @verbatim
 */


#ifndef DATA_IDS_HPP_
#define DATA_IDS_HPP_

#include <unordered_map>
#include <string>
#include <cstdint>

namespace data {
// NOLINTBEGIN(readability-identifier-naming)
enum class DataId : uint16_t {
  GENERIC_unknown=0x0000, // Unknown
  MODULE_statusUpdate=0x0001, // Status Update
  EXTENSION_statusUpdate=0x0002, // Status Update
  ENVIRONMENTAL_temperature=0xE001, // Temperature (Celsius)
  ENVIRONMENTAL_humidity=0xE002, // Relative Humidity (%)
  ENVIRONMENTAL_pressure=0xE003, // Pressure (mbar)
  CAMERA_newImage=0xC001, // New Image
  ADC_count=0xA001, // Raw ADC Count
  TELEMETRY_dataLinkPacketReceived=0x8001, // Data Link Packet Received
  FLIGHT_RUNNER_flightPhaseChange=0x9001  // Flight Phase Change
};
// NOLINTEND(readability-identifier-naming)

extern const std::unordered_map<DataId, std::string> K_DATA_ID_TO_STRING_MAP;
} // namespace data

#endif // DATA_IDS_HPP_


/**
 * @endverbatim
 * 
 * &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
 * AUTOMATICALLY GENERATED, DO NOT EDIT MANUALLY
 * &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
 */