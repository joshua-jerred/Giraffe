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

namespace data {
enum class DataId : uint16_t {
  GENERIC_unknown=0x0000, // Unknown
  MODULE_statusUpdate=0x0001, // Status Update
  EXTENSION_statusUpdate=0x0002, // Status Update
  ENVIRONMENTAL_temperature=0xE001, // Temperature (Celsius)
  ENVIRONMENTAL_humidity=0xE002, // Relative Humidity (%)
  ENVIRONMENTAL_pressure=0xE003  // Pressure (mbar)
};
} // namespace data

#endif // DATA_IDS_HPP_


/**
 * @endverbatim
 * 
 * &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
 * AUTOMATICALLY GENERATED, DO NOT EDIT MANUALLY
 * &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
 */