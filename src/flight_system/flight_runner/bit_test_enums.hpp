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

#include <cstdint>
#include <string>

namespace bit {

  /// @brief This enum contains the status codes for the Built-In Test (BIT) system.
  enum class TestStatus : uint8_t {
    Unknown = 0, /// @brief UNK
    NoData = 1, /// @brief N/D
    NotRun = 2, /// @brief N/R
    Waiting = 3, /// @brief WAIT
    Running = 4, /// @brief RUN
    Passed = 5, /// @brief PASS
    Failed = 6, /// @brief FAIL
    Skipped = 7, /// @brief SKIP
  };

  extern std::string testStatusToString(TestStatus status);

  /// @brief Identifiers for the different groups of BIT tests.
  enum class TestGroupId : uint8_t {
    Unknown = 0, /// @brief Unknown test group
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
    Unknown = 0x0000, /// @brief Unknown test id
    FCS_ErrorTest = 0x0001,
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
 * Generated on: 2024-11-16
 * 
 * @endverbatim
 * 
 * &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
 * AUTOMATICALLY GENERATED, DO NOT EDIT MANUALLY
 * &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
 */