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
 * @date       2024-11-16
 * @copyright  2023 (license to be defined)
 * =*=======================*=
 *
 * @verbatim
 */

#pragma once

#include <cstdint>
#include <string>

/// @ This enum contains all test ids for the Built-In Test (BIT) system.
enum class BitTestId : uint16_t {
  FCS_ErrorTest = 0x0000,
  SYS_ErrorTest = 0x0100,
  DLNK_ConfigLocationDataTest = 0x0200,
  GPS_InstalledTest = 0x0400,
  BATT_AdcTest = 0x0500,
  EXTN_ExtensionsRunningTest = 0x0600,
  ENV_TemperatureTest = 0x0700,
  CAM_InstalledTest = 0x0800,
  IMU_InstalledTest = 0x0900,
  ADC_InstalledTest = 0x0a00,
};

/**
 *
 * Generated on: 2024-11-16
 * 
 * @endverbatim
 * 
 * &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
 * AUTOMATICALLY GENERATED, DO NOT EDIT MANUALLY
 * &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
 */