/// =*========GIRAFFE========*=
/// A Unified Flight Command and Control System
/// https://github.com/joshua-jerred/Giraffe
/// https://giraffe.joshuajer.red/
/// =*=======================*=
///
/// @file   test_built_in_self_test.cpp
///
/// =*=======================*=
/// @author     Joshua Jerred (https://joshuajer.red)
/// @date       2024-11-16
/// @copyright  2024 (license to be defined)

#include "test_cases.hpp"
#include "unit_test.hpp"

TEST(BIT_TestCase, FCS_0001) {
  data::SharedData sharedData;
  auto results = bit::test_case::runTestFCS_0001(sharedData);
  EXPECT_EQ(results.status, bit::TestStatus::PASSED);

  // Add an error to the error frame
  data::LogPacket errorPacket{};
  errorPacket.id = DiagnosticId::CONFIG_adcMappingLabelAlreadyExists;
  errorPacket.level = data::LogPacket::Level::ERROR;
  sharedData.frames.error_frame.addError(errorPacket);

  results = bit::test_case::runTestFCS_0001(sharedData);
  EXPECT_EQ(results.status, bit::TestStatus::FAILED);
}