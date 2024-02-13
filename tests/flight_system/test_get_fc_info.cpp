#include "unit_test.hpp"

#include "flight_computer_info.hpp"

TEST(GetFCInfo, not_unknown) {
  auto type = giraffe::getFlightComputerInfo();

  EXPECT_NE(type.architecture, giraffe::FCInfo::Architecture::UNKNOWN);
  EXPECT_NE(type.type, giraffe::FCInfo::Type::UNKNOWN);
}