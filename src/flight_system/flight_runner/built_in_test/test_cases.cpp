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

#include "test_cases.hpp"

namespace bit {

TestResult runTestFCS_0001(data::SharedData &shared_data) {
  TestResult result{.test_id = TestId::FCS_ErrorTest,
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

} // namespace bit