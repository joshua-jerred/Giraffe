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
 * @date       2024-11-23
 * @copyright  2023 (license to be defined)
 * =*=======================*=
 *
 * @verbatim
 */

#pragma once

#include <vector>
#include <string>

#include "bit_types.hpp"
#include "test_case.hpp"

namespace bit {

  /// @brief Test cases for the FCS group.
  static const std::vector<TestCase> BIT_TEST_GROUP_FCS {
    {TestGroupId::FCS, TestId::FCS_ErrorTest, TestStatus::Unknown},
    {TestGroupId::FCS, TestId::FCS_FlightPhasePredictorTest, TestStatus::Unknown},
    {TestGroupId::FCS, TestId::FCS_LaunchPositionTest, TestStatus::Unknown},
    {TestGroupId::FCS, TestId::FCS_DataModuleProcessingTest, TestStatus::Unknown},
  };

  /// @brief Test cases for the SYS group.
  static const std::vector<TestCase> BIT_TEST_GROUP_SYS {
    {TestGroupId::SYS, TestId::SYS_ErrorTest, TestStatus::Unknown},
  };

  /// @brief Test cases for the DLNK group.
  static const std::vector<TestCase> BIT_TEST_GROUP_DLNK {
    {TestGroupId::DLNK, TestId::DLNK_ConfigLocationDataTest, TestStatus::Unknown},
  };

  /// @brief Test cases for the GPS group.
  static const std::vector<TestCase> BIT_TEST_GROUP_GPS {
    {TestGroupId::GPS, TestId::GPS_InstalledTest, TestStatus::Unknown},
  };

  /// @brief Test cases for the BATT group.
  static const std::vector<TestCase> BIT_TEST_GROUP_BATT {
    {TestGroupId::BATT, TestId::BATT_AdcTest, TestStatus::Unknown},
  };

  /// @brief Test cases for the EXTN group.
  static const std::vector<TestCase> BIT_TEST_GROUP_EXTN {
    {TestGroupId::EXTN, TestId::EXTN_ExtensionsRunningTest, TestStatus::Unknown},
  };

  /// @brief Test cases for the ENV group.
  static const std::vector<TestCase> BIT_TEST_GROUP_ENV {
    {TestGroupId::ENV, TestId::ENV_TemperatureTest, TestStatus::Unknown},
  };

  /// @brief Test cases for the CAM group.
  static const std::vector<TestCase> BIT_TEST_GROUP_CAM {
    {TestGroupId::CAM, TestId::CAM_InstalledTest, TestStatus::Unknown},
  };

  /// @brief Test cases for the IMU group.
  static const std::vector<TestCase> BIT_TEST_GROUP_IMU {
    {TestGroupId::IMU, TestId::IMU_InstalledTest, TestStatus::Unknown},
  };

  /// @brief Test cases for the ADC group.
  static const std::vector<TestCase> BIT_TEST_GROUP_ADC {
    {TestGroupId::ADC, TestId::ADC_InstalledTest, TestStatus::Unknown},
  };

}

/**
 *
 * Generated on: 2024-11-23
 * 
 * @endverbatim
 * 
 * &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
 * AUTOMATICALLY GENERATED, DO NOT EDIT MANUALLY
 * &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
 */