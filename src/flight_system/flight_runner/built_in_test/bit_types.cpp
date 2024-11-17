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

#include <map>

#include "bit_types.hpp"
#include "giraffe_assert.hpp"

namespace bit {

/// @brief This map converts the TestStatus enum to a string.
static const std::map<TestStatus, const std::string> TestStatusToStringMap = {
    {TestStatus::Unknown, "UNK"}, {TestStatus::NoData, "N/D"},
    {TestStatus::NotRun, "N/R"},  {TestStatus::Waiting, "WAIT"},
    {TestStatus::Running, "RUN"}, {TestStatus::Passed, "PASS"},
    {TestStatus::Failed, "FAIL"}, {TestStatus::Skipped, "SKIP"},
};

/// @brief This function converts a TestStatus enum to a string.
std::string testStatusToString(TestStatus status) {
  if (!TestStatusToStringMap.contains(status)) {
    giraffe_assert(false);
    return TestStatusToStringMap.at(TestStatus::Unknown);
  }
  return TestStatusToStringMap.at(status);
}

/// @brief This map converts the TestGroupId enum to a string.
static const std::map<TestGroupId, const std::string> TestGroupIdToStringMap = {
    {TestGroupId::FCS, "FCS"},   {TestGroupId::SYS, "SYS"},
    {TestGroupId::DLNK, "DLNK"}, {TestGroupId::GPS, "GPS"},
    {TestGroupId::BATT, "BATT"}, {TestGroupId::EXTN, "EXTN"},
    {TestGroupId::ENV, "ENV"},   {TestGroupId::CAM, "CAM"},
    {TestGroupId::IMU, "IMU"},   {TestGroupId::ADC, "ADC"},
};

/// @brief This function converts a TestGroupId enum to a string.
std::string testGroupIdToString(TestGroupId groupId) {
  if (!TestGroupIdToStringMap.contains(groupId)) {
    giraffe_assert(false);
    return "Unknown";
  }
  return TestGroupIdToStringMap.at(groupId);
}

/// @brief This map converts the TestId enum to a string.
static const std::map<TestId, const std::string> TestIdToStringMap = {
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
std::string testIdToString(TestId testId) {
  if (!TestIdToStringMap.contains(testId)) {
    giraffe_assert(false);
    return "Unknown";
  }
  return TestIdToStringMap.at(testId);
}

} // namespace bit

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