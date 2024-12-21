#include "software_version.hpp"
#include "unit_test.hpp"

TEST(common_SoftwareVersion, setNumbersFromString) {
  giraffe::SoftwareVersion sv;

  // Test initial values
  ASSERT_EQ(sv.major, 0);
  ASSERT_EQ(sv.minor, 0);
  ASSERT_EQ(sv.patch, 0);
  // Test the other values as this is the only test for this class right now.
  ASSERT_EQ(sv.stage, giraffe::SoftwareVersion::Stage::UNKNOWN);
  ASSERT_FALSE(sv.clean);

  // Basic happy path tests
  sv.setNumbersFromString("1.2.3");
  EXPECT_EQ(sv.major, 1);
  EXPECT_EQ(sv.minor, 2);
  EXPECT_EQ(sv.patch, 3);

  sv.setNumbersFromString("0.0.0");
  EXPECT_EQ(sv.major, 0);
  EXPECT_EQ(sv.minor, 0);
  EXPECT_EQ(sv.patch, 0);

  sv.setNumbersFromString("999.999.999");
  EXPECT_EQ(sv.major, 999);
  EXPECT_EQ(sv.minor, 999);
  EXPECT_EQ(sv.patch, 999);

  sv.setNumbersFromString("9999.100000.1");
  EXPECT_EQ(sv.major, 9999);
  EXPECT_EQ(sv.minor, 100000);
  EXPECT_EQ(sv.patch, 1);

  // Test invalid strings
  EXPECT_THROW(sv.setNumbersFromString(".1.1"), std::runtime_error);
  EXPECT_EQ(sv.major, 0);
  EXPECT_EQ(sv.minor, 0);
  EXPECT_EQ(sv.patch, 0);

  EXPECT_THROW(sv.setNumbersFromString("1..1"), std::runtime_error);
  EXPECT_THROW(sv.setNumbersFromString("1.1."), std::runtime_error);
  EXPECT_THROW(sv.setNumbersFromString(""), std::runtime_error);

  // Allowed under SemVer, but KISS for now
  EXPECT_THROW(sv.setNumbersFromString("1.1.1."), std::runtime_error);
}

TEST(common_SoftwareVersion, getNumbersAsString) {
  giraffe::SoftwareVersion sv;

  // Test initial values
  ASSERT_EQ(sv.getNumbersAsString(), "0.0.0");

  // Basic happy path tests
  sv.major = 1;
  sv.minor = 2;
  sv.patch = 3;
  EXPECT_EQ(sv.getNumbersAsString(), "1.2.3");

  sv.major = 0;
  sv.minor = 0;
  sv.patch = 0;
  EXPECT_EQ(sv.getNumbersAsString(), "0.0.0");

  sv.major = 999;
  sv.minor = 999;
  sv.patch = 999;
  EXPECT_EQ(sv.getNumbersAsString(), "999.999.999");

  sv.major = 9999;
  sv.minor = 100000;
  sv.patch = 1;
  EXPECT_EQ(sv.getNumbersAsString(), "9999.100000.1");
}

TEST(common_SoftwareVersion, setStageFromString) {
  giraffe::SoftwareVersion sv;

  // Test initial values
  ASSERT_EQ(sv.stage, giraffe::SoftwareVersion::Stage::UNKNOWN);

  // Basic happy path tests
  sv.setStageFromString("development");
  EXPECT_EQ(sv.stage, giraffe::SoftwareVersion::Stage::DEVELOPMENT);

  sv.setStageFromString("release");
  EXPECT_EQ(sv.stage, giraffe::SoftwareVersion::Stage::RELEASE);

  // Test invalid strings
  EXPECT_THROW(sv.setStageFromString("unknown"), std::runtime_error);
  EXPECT_EQ(sv.stage, giraffe::SoftwareVersion::Stage::UNKNOWN);

  EXPECT_THROW(sv.setStageFromString(""), std::runtime_error);
  EXPECT_EQ(sv.stage, giraffe::SoftwareVersion::Stage::UNKNOWN);
}

TEST(common_SoftwareVersion, getStageAsString) {
  giraffe::SoftwareVersion sv;

  // Test initial values
  ASSERT_EQ(sv.getStageAsString(), "unknown");

  // Basic happy path tests
  sv.stage = giraffe::SoftwareVersion::Stage::DEVELOPMENT;
  EXPECT_EQ(sv.getStageAsString(), "development");

  sv.stage = giraffe::SoftwareVersion::Stage::RELEASE;
  EXPECT_EQ(sv.getStageAsString(), "release");

  // Test invalid strings
  sv.stage = giraffe::SoftwareVersion::Stage::UNKNOWN;
  EXPECT_EQ(sv.getStageAsString(), "unknown");
}
