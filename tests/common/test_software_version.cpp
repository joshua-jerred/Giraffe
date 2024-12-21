#include "software_version.hpp"
#include "unit_test.hpp"

TEST(common_SoftwareVersion, setAndGetSemanticString) {
  giraffe::SoftwareVersion sv;

  // Test initial values
  ASSERT_EQ(sv.major, 0);
  ASSERT_EQ(sv.minor, 0);
  ASSERT_EQ(sv.patch, 0);
  // Test the other values as this is the only test for this class right now.
  ASSERT_EQ(sv.stage, giraffe::SoftwareVersion::Stage::UNKNOWN);
  ASSERT_EQ(sv.clean, false);

  // Basic happy path tests
  sv.setVersionFromString("1.2.3");
  EXPECT_EQ(sv.major, 1);
  EXPECT_EQ(sv.minor, 2);
  EXPECT_EQ(sv.patch, 3);
  EXPECT_EQ(sv.stage, giraffe::SoftwareVersion::Stage::RELEASE);
  EXPECT_EQ(sv.clean, true);
  EXPECT_EQ(sv.getNumbersAsString(), "1.2.3");
  EXPECT_EQ(sv.getVersionString(), "1.2.3");

  sv.setVersionFromString("0.0.0-alpha");
  EXPECT_EQ(sv.major, 0);
  EXPECT_EQ(sv.minor, 0);
  EXPECT_EQ(sv.patch, 0);
  EXPECT_EQ(sv.stage, giraffe::SoftwareVersion::Stage::ALPHA);
  EXPECT_EQ(sv.clean, true);
  EXPECT_EQ(sv.getNumbersAsString(), "0.0.0");
  EXPECT_EQ(sv.getVersionString(), "0.0.0-alpha");

  sv.setVersionFromString("999.999.999-rc-dev");
  EXPECT_EQ(sv.major, 999);
  EXPECT_EQ(sv.minor, 999);
  EXPECT_EQ(sv.patch, 999);
  EXPECT_EQ(sv.stage, giraffe::SoftwareVersion::Stage::RELEASE_CANDIDATE);
  EXPECT_EQ(sv.clean, false);
  EXPECT_EQ(sv.getNumbersAsString(), "999.999.999");
  EXPECT_EQ(sv.getVersionString(), "999.999.999-rc-dev");

  sv.setVersionFromString("9999.100000.1-beta");
  EXPECT_EQ(sv.major, 9999);
  EXPECT_EQ(sv.minor, 100000);
  EXPECT_EQ(sv.patch, 1);
  EXPECT_EQ(sv.stage, giraffe::SoftwareVersion::Stage::BETA);
  EXPECT_EQ(sv.clean, true);
  EXPECT_EQ(sv.getNumbersAsString(), "9999.100000.1");
  EXPECT_EQ(sv.getVersionString(), "9999.100000.1-beta");

  // Test invalid strings
  EXPECT_THROW(sv.setVersionFromString(".1.1"), std::runtime_error);
  EXPECT_EQ(sv.major, 0);
  EXPECT_EQ(sv.minor, 0);
  EXPECT_EQ(sv.patch, 0);
  EXPECT_EQ(sv.stage, giraffe::SoftwareVersion::Stage::UNKNOWN);
  EXPECT_EQ(sv.clean, false);

  EXPECT_THROW(sv.setVersionFromString("1..1"), std::runtime_error);
  EXPECT_THROW(sv.setVersionFromString("1.1."), std::runtime_error);
  EXPECT_THROW(sv.setVersionFromString(""), std::runtime_error);

  // Allowed under SemVer, but KISS for now
  EXPECT_THROW(sv.setVersionFromString("1.1.1."), std::runtime_error);
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