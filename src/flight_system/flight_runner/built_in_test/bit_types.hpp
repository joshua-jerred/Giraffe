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
 * @date       2025-05-23
 * @copyright  2023 (license to be defined)
 * =*=======================*=
 *
 * @verbatim
 */

#pragma once

#include <cstdint>
#include <string>

namespace bit {

  /// @brief This enum contains the status codes for the Built-In Test (BIT) system.
  enum class TestStatus : uint8_t {
    UNKNOWN = 0, /// @brief UNK
    NODATA = 1, /// @brief N/D
    NOTRUN = 2, /// @brief N/R
    WAITING = 3, /// @brief WAIT
    RUNNING = 4, /// @brief RUN
    PASSED = 5, /// @brief PASS
    FAILED = 6, /// @brief FAIL
    SKIPPED = 7, /// @brief SKIP
    STOPPED = 8, /// @brief STOP
  };

  extern std::string testStatusToString(TestStatus status);

  /// @brief Identifiers for the different groups of BIT tests.
  enum class TestGroupId : uint8_t {
    FCS = 1, /// @brief Flight Control System
    SYS = 2, /// @brief System
    DLNK = 3, /// @brief Data Link
    GPS = 4, /// @brief GPS
    BATT = 5, /// @brief Battery
    EXTN = 6, /// @brief Extensions
    ENV = 7, /// @brief Environment
    CAM = 8, /// @brief Camera
    IMU = 9, /// @brief Inertial Measurement Unit
    ADC = 10, /// @brief Analog to Digital Converter
  };

  extern std::string testGroupIdToString(TestGroupId groupId);

  /// @brief This enum contains the test ids for the Built-In Test (BIT) system.
  enum class TestId : uint16_t {
    FCS_NoErrorsTest = 0x0001,
    FCS_FlightPhasePredictorTest = 0x0002,
    FCS_LaunchPositionTest = 0x0003,
    FCS_DataModuleProcessingTest = 0x0004,
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

  extern std::string testIdToString(TestId testId);

}

/**
 *
 * Generated on: 2025-05-23
 *
 * @endverbatim
 *
 * &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
 * AUTOMATICALLY GENERATED, DO NOT EDIT MANUALLY
 * &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
 */