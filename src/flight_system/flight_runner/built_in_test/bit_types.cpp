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
 * @date       2024-11-17
 * @copyright  2023 (license to be defined)
 * =*=======================*=
 *
 * @verbatim
 */

#include <map>
#include <vector>


#include "giraffe_assert.hpp"
#include "bit_types.hpp"

namespace bit {

  /// @brief This map converts the TestStatus enum to a string.
  static const std::map<TestStatus, const std::string> TestStatusToStringMap = {
    { TestStatus::Unknown, "UNK" },
    { TestStatus::NoData, "N/D" },
    { TestStatus::NotRun, "N/R" },
    { TestStatus::Waiting, "WAIT" },
    { TestStatus::Running, "RUN" },
    { TestStatus::Passed, "PASS" },
    { TestStatus::Failed, "FAIL" },
    { TestStatus::Skipped, "SKIP" },
  };

  /// @brief This function converts a TestStatus enum to a string.
  std::string testStatusToString(TestStatus status)  {
    if (!TestStatusToStringMap.contains(status)) {
    giraffe_assert(false);
    return TestStatusToStringMap.at(TestStatus::Unknown);
    }
    return TestStatusToStringMap.at(status);
  }

  /// @brief This map converts the TestGroupId enum to a string.
  static const std::map<TestGroupId, const std::string> TestGroupIdToStringMap = {
    { TestGroupId::FCS, "FCS" },
    { TestGroupId::SYS, "SYS" },
    { TestGroupId::DLNK, "DLNK" },
    { TestGroupId::GPS, "GPS" },
    { TestGroupId::BATT, "BATT" },
    { TestGroupId::EXTN, "EXTN" },
    { TestGroupId::ENV, "ENV" },
    { TestGroupId::CAM, "CAM" },
    { TestGroupId::IMU, "IMU" },
    { TestGroupId::ADC, "ADC" },
  };

  /// @brief This function converts a TestGroupId enum to a string.
  std::string testGroupIdToString(TestGroupId groupId)  {
    if (!TestGroupIdToStringMap.contains(groupId)) {
    giraffe_assert(false);
    return "Unknown";
    }
    return TestGroupIdToStringMap.at(groupId);
  }

  /// @brief This map converts the TestId enum to a string.
  static const std::map<TestId, const std::string> TestIdToStringMap = {
    { TestId::FCS_ErrorTest, "FCS_ErrorTest" },
    { TestId::FCS_FlightPhasePredictorTest, "FCS_FlightPhasePredictorTest" },
    { TestId::FCS_LaunchPositionTest, "FCS_LaunchPositionTest" },
    { TestId::FCS_DataModuleProcessingTest, "FCS_DataModuleProcessingTest" },
    { TestId::SYS_ErrorTest, "SYS_ErrorTest" },
    { TestId::DLNK_ConfigLocationDataTest, "DLNK_ConfigLocationDataTest" },
    { TestId::GPS_InstalledTest, "GPS_InstalledTest" },
    { TestId::BATT_AdcTest, "BATT_AdcTest" },
    { TestId::EXTN_ExtensionsRunningTest, "EXTN_ExtensionsRunningTest" },
    { TestId::ENV_TemperatureTest, "ENV_TemperatureTest" },
    { TestId::CAM_InstalledTest, "CAM_InstalledTest" },
    { TestId::IMU_InstalledTest, "IMU_InstalledTest" },
    { TestId::ADC_InstalledTest, "ADC_InstalledTest" },
  };

  /// @brief This function converts a TestId enum to a string.
  std::string testIdToString(TestId testId)  {
    if (!TestIdToStringMap.contains(testId)) {
    giraffe_assert(false);
    return "Unknown";
    }
    return TestIdToStringMap.at(testId);
  }

  /// @brief Test cases for the FCS group.
  static std::vector<TestCase> BIT_TEST_GROUP_FCS {
    {FCS, TestId::FCS_ErrorTest, TestStatus::Unknown},
    {FCS, TestId::FCS_FlightPhasePredictorTest, TestStatus::Unknown},
    {FCS, TestId::FCS_LaunchPositionTest, TestStatus::Unknown},
    {FCS, TestId::FCS_DataModuleProcessingTest, TestStatus::Unknown},
  }

  /// @brief Test cases for the SYS group.
  static std::vector<TestCase> BIT_TEST_GROUP_SYS {
    {SYS, TestId::SYS_ErrorTest, TestStatus::Unknown},
  }

  /// @brief Test cases for the DLNK group.
  static std::vector<TestCase> BIT_TEST_GROUP_DLNK {
    {DLNK, TestId::DLNK_ConfigLocationDataTest, TestStatus::Unknown},
  }

  /// @brief Test cases for the GPS group.
  static std::vector<TestCase> BIT_TEST_GROUP_GPS {
    {GPS, TestId::GPS_InstalledTest, TestStatus::Unknown},
  }

  /// @brief Test cases for the BATT group.
  static std::vector<TestCase> BIT_TEST_GROUP_BATT {
    {BATT, TestId::BATT_AdcTest, TestStatus::Unknown},
  }

  /// @brief Test cases for the EXTN group.
  static std::vector<TestCase> BIT_TEST_GROUP_EXTN {
    {EXTN, TestId::EXTN_ExtensionsRunningTest, TestStatus::Unknown},
  }

  /// @brief Test cases for the ENV group.
  static std::vector<TestCase> BIT_TEST_GROUP_ENV {
    {ENV, TestId::ENV_TemperatureTest, TestStatus::Unknown},
  }

  /// @brief Test cases for the CAM group.
  static std::vector<TestCase> BIT_TEST_GROUP_CAM {
    {CAM, TestId::CAM_InstalledTest, TestStatus::Unknown},
  }

  /// @brief Test cases for the IMU group.
  static std::vector<TestCase> BIT_TEST_GROUP_IMU {
    {IMU, TestId::IMU_InstalledTest, TestStatus::Unknown},
  }

  /// @brief Test cases for the ADC group.
  static std::vector<TestCase> BIT_TEST_GROUP_ADC {
    {ADC, TestId::ADC_InstalledTest, TestStatus::Unknown},
  }

}

/**
 *
 * Generated on: 2024-11-17
 * 
 * @endverbatim
 * 
 * &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
 * AUTOMATICALLY GENERATED, DO NOT EDIT MANUALLY
 * &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
 */