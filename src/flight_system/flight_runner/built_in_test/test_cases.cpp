/// =*========GIRAFFE========*=
/// A Unified Flight Command and Control System
/// https://github.com/joshua-jerred/Giraffe
/// https://giraffe.joshuajer.red/
/// =*=======================*=
///
/// @file   test_cases_fcs.cpp
///
/// =*=======================*=
/// @author     Joshua Jerred (https://joshuajer.red)
/// @date       2024-12-03
/// @copyright  2024 (license to be defined)

#include "BoosterSeat/sleep.hpp"

#include "test_cases.hpp"

namespace bit {
namespace test_case {

TestResult runTestFCS_0001(data::SharedData &shared_data) {
  TestResult result{.test_id = TestId::FCS_ErrorFrameEmptyTest,
                    .group_id = TestGroupId::FCS,
                    .status = TestStatus::UNKNOWN,
                    .failure_reason = ""};

  if (shared_data.frames.error_frame.numActiveErrors() > 0) {
    result.status = TestStatus::FAILED;
    result.failure_reason =
        "Error frame has " +
        std::to_string(shared_data.frames.error_frame.numActiveErrors()) +
        " active errors.";
  } else {
    result.status = TestStatus::PASSED;
  }

  return result;
}

TestResult runTestFCS_0004(data::SharedData &shared_data) {
  TestResult result{.test_id = TestId::FCS_DataModuleProcessingTest,
                    .group_id = TestGroupId::FCS,
                    .status = TestStatus::SKIPPED,
                    .failure_reason = ""};
  (void)shared_data;
  /// @todo Make sure the data module is processing data correctly/at a
  /// reasonable rate

  return result;
}

TestResult runTestGPS_0400(data::SharedData &shared_data) {
  TestResult result{.test_id = TestId::GPS_InstalledTest,
                    .group_id = TestGroupId::GPS,
                    .status = TestStatus::UNKNOWN,
                    .failure_reason = ""};

  const auto location_data = shared_data.blocks.location_data.get();

  if (!location_data.have_gps_source) {
    result.status = TestStatus::FAILED;
    result.failure_reason = "GPS hardware is not configured.";
  } else {
    result.status = TestStatus::PASSED;
  }

  return result;
}

} // namespace test_case
} // namespace bit