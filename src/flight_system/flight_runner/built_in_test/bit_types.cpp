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
 * @date       2024-12-12
 * @copyright  2023 (license to be defined)
 * =*=======================*=
 *
 * @verbatim
 */

#include <map>



#include "giraffe_assert.hpp"
#include "bit_types.hpp"

namespace bit {

  /// @brief This map converts the TestStatus enum to a string.
  static const std::map<TestStatus, const std::string> TestStatusToStringMap{
    {TestStatus::UNKNOWN, "UNK"},
    {TestStatus::NODATA, "N/D"},
    {TestStatus::NOTRUN, "N/R"},
    {TestStatus::WAITING, "WAIT"},
    {TestStatus::RUNNING, "RUN"},
    {TestStatus::PASSED, "PASS"},
    {TestStatus::FAILED, "FAIL"},
    {TestStatus::SKIPPED, "SKIP"},
    {TestStatus::STOPPED, "STOP"},
  };
  /// @brief This function converts a TestStatus enum to a string.
  std::string testStatusToString(TestStatus status)  {
    switch (status) {
    case TestStatus::UNKNOWN: return "UNK";
    case TestStatus::NODATA: return "N/D";
    case TestStatus::NOTRUN: return "N/R";
    case TestStatus::WAITING: return "WAIT";
    case TestStatus::RUNNING: return "RUN";
    case TestStatus::PASSED: return "PASS";
    case TestStatus::FAILED: return "FAIL";
    case TestStatus::SKIPPED: return "SKIP";
    case TestStatus::STOPPED: return "STOP";
    default:
    giraffe_assert(false);
    return "Unknown";
    }
  }

  /// @brief This map converts the TestGroupId enum to a string.
  static const std::map<TestGroupId, const std::string> TestGroupIdToStringMap{
    {TestGroupId::FCS, "FCS"},
    {TestGroupId::SYS, "SYS"},
    {TestGroupId::DLNK, "DLNK"},
    {TestGroupId::GPS, "GPS"},
    {TestGroupId::BATT, "BATT"},
    {TestGroupId::EXTN, "EXTN"},
    {TestGroupId::ENV, "ENV"},
    {TestGroupId::CAM, "CAM"},
    {TestGroupId::IMU, "IMU"},
    {TestGroupId::ADC, "ADC"},
  };
  /// @brief This function converts a TestGroupId enum to a string.
  std::string testGroupIdToString(TestGroupId groupId)  {
    switch (groupId) {
    case TestGroupId::FCS: return "FCS";
    case TestGroupId::SYS: return "SYS";
    case TestGroupId::DLNK: return "DLNK";
    case TestGroupId::GPS: return "GPS";
    case TestGroupId::BATT: return "BATT";
    case TestGroupId::EXTN: return "EXTN";
    case TestGroupId::ENV: return "ENV";
    case TestGroupId::CAM: return "CAM";
    case TestGroupId::IMU: return "IMU";
    case TestGroupId::ADC: return "ADC";
    default:
    giraffe_assert(false);
    return "Unknown";
    }
  }

  /// @brief This map converts the TestId enum to a string.
  static const std::map<TestId, const std::string> TestIdToStringMap{
    {TestId::FCS_ErrorTest, "FCS_ErrorTest"},
    {TestId::FCS_FlightPhasePredictorTest, "FCS_FlightPhasePredictorTest"},
    {TestId::FCS_LaunchPositionTest, "FCS_LaunchPositionTest"},
    {TestId::FCS_DataModuleProcessingTest, "FCS_DataModuleProcessingTest"},
    {TestId::SYS_ErrorTest, "SYS_ErrorTest"},
    {TestId::DLNK_ConfigLocationDataTest, "DLNK_ConfigLocationDataTest"},
    {TestId::GPS_InstalledTest, "GPS_InstalledTest"},
    {TestId::BATT_AdcTest, "BATT_AdcTest"},
    {TestId::EXTN_ExtensionsRunningTest, "EXTN_ExtensionsRunningTest"},
    {TestId::ENV_TemperatureTest, "ENV_TemperatureTest"},
    {TestId::CAM_InstalledTest, "CAM_InstalledTest"},
    {TestId::IMU_InstalledTest, "IMU_InstalledTest"},
    {TestId::ADC_InstalledTest, "ADC_InstalledTest"},
  };
  /// @brief This function converts a TestId enum to a string.
  std::string testIdToString(TestId testId)  {
    switch (testId) {
    case TestId::FCS_ErrorTest: return "FCS_ErrorTest";
    case TestId::FCS_FlightPhasePredictorTest: return "FCS_FlightPhasePredictorTest";
    case TestId::FCS_LaunchPositionTest: return "FCS_LaunchPositionTest";
    case TestId::FCS_DataModuleProcessingTest: return "FCS_DataModuleProcessingTest";
    case TestId::SYS_ErrorTest: return "SYS_ErrorTest";
    case TestId::DLNK_ConfigLocationDataTest: return "DLNK_ConfigLocationDataTest";
    case TestId::GPS_InstalledTest: return "GPS_InstalledTest";
    case TestId::BATT_AdcTest: return "BATT_AdcTest";
    case TestId::EXTN_ExtensionsRunningTest: return "EXTN_ExtensionsRunningTest";
    case TestId::ENV_TemperatureTest: return "ENV_TemperatureTest";
    case TestId::CAM_InstalledTest: return "CAM_InstalledTest";
    case TestId::IMU_InstalledTest: return "IMU_InstalledTest";
    case TestId::ADC_InstalledTest: return "ADC_InstalledTest";
    default:
    giraffe_assert(false);
    return "Unknown";
    }
  }

}

/**
 *
 * Generated on: 2024-12-12
 *
 * @endverbatim
 *
 * &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
 * AUTOMATICALLY GENERATED, DO NOT EDIT MANUALLY
 * &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
 */