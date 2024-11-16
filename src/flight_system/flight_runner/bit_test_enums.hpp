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

/// @brief This enum contains the status codes for the Built-In Test (BIT)
/// system.
enum class BitTestStatus : uint8_t {
  /// @brief None
  Unknown = 0,
  /// @brief None
  NoData = 1,
  /// @brief None
  NotRun = 2,
  /// @brief None
  Waiting = 3,
  /// @brief None
  Running = 4,
  /// @brief None
  Passed = 5,
  /// @brief None
  Failed = 6,
  /// @brief None
  Skipped = 7,
};

enum class BitTestGroup : uint8_t {
  /// @brief Flight Control System
  FlightControlSystem,
  /// @brief System
  System,
  /// @brief Data Link
  DataLink,
  /// @brief GPS
  GPS,
  /// @brief Battery
  Battery,
  /// @brief Extensions
  Extensions,
  /// @brief Environment
  Environment,
  /// @brief Camera
  Camera,
  /// @brief Inertial Measurement Unit
  InertialMeasurementUnit,
  /// @brief Analog to Digital Converter
  AnalogtoDigitalConverter,
};

/// @brief This enum contains the test ids for the Built-In Test (BIT) system.
enum class BitTestId : uint16_t {
  /// @brief None
  FCS_ErrorTest = 0x0000,
  /// @brief None
  SYS_ErrorTest = 0x0100,
  /// @brief None
  DLNK_ConfigLocationDataTest = 0x0200,
  /// @brief None
  GPS_InstalledTest = 0x0400,
  /// @brief None
  BATT_AdcTest = 0x0500,
  /// @brief None
  EXTN_ExtensionsRunningTest = 0x0600,
  /// @brief None
  ENV_TemperatureTest = 0x0700,
  /// @brief None
  CAM_InstalledTest = 0x0800,
  /// @brief None
  IMU_InstalledTest = 0x0900,
  /// @brief None
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