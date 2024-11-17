/// =*========GIRAFFE========*=
/// A Unified Flight Command and Control System
/// https://github.com/joshua-jerred/Giraffe
/// https://giraffe.joshuajer.red/
/// =*=======================*=
///
/// @file   test_case.hpp
///
/// =*=======================*=
/// @author     Joshua Jerred (https://joshuajer.red)
/// @date       2024-11-16
/// @copyright  2024 (license to be defined)

#pragma once

#include "bit_test_enums.hpp"

namespace bit {
  class TestCase {
  public:
    TestCase(TestGroupId group_id, TestId test_id)
      : group_id_{group_id}, test_id_{test_id} {
    }

  private:
    TestGroupId group_id_;
    TestId test_id_;
    TestStatus status_ = TestStatus::NotRun;
  };
}
