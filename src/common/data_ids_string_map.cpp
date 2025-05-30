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


#include "data_ids.hpp"

namespace data {

const std::unordered_map<DataId, std::string> K_DATA_ID_TO_STRING_MAP {
  {DataId::GENERIC_unknown,"GENERIC_unknown"},
  {DataId::MODULE_statusUpdate,"MODULE_statusUpdate"},
  {DataId::EXTENSION_statusUpdate,"EXTENSION_statusUpdate"},
  {DataId::ENVIRONMENTAL_temperature,"ENVIRONMENTAL_temperature"},
  {DataId::ENVIRONMENTAL_humidity,"ENVIRONMENTAL_humidity"},
  {DataId::ENVIRONMENTAL_pressure,"ENVIRONMENTAL_pressure"},
  {DataId::BATTERY_voltageMv,"BATTERY_voltageMv"},
  {DataId::CAMERA_newImage,"CAMERA_newImage"},
  {DataId::ADC_count,"ADC_count"},
  {DataId::TELEMETRY_dataLinkPacketReceived,"TELEMETRY_dataLinkPacketReceived"},
  {DataId::FLIGHT_RUNNER_flightPhaseChange,"FLIGHT_RUNNER_flightPhaseChange"}
};

} // namespace data


/**
 * @endverbatim
 *
 * &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
 * AUTOMATICALLY GENERATED, DO NOT EDIT MANUALLY
 * &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
 */