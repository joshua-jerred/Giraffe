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
#include "bit_test_enums.hpp"

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

  std::string testStatusToString(TestStatus status)  {
    return;
  }

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