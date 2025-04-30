/// =*========GIRAFFE========*=
/// A Unified Flight Command and Control System
/// https://github.com/joshua-jerred/Giraffe
/// https://giraffe.joshuajer.red/
/// =*=======================*=
///
/// @file   test_update_pack.hpp
///
/// =*=======================*=
/// @author     Joshua Jerred (https://joshuajer.red)
/// @date       2024-12-19
/// @copyright  2024 (license to be defined)

#include <filesystem>

#include "gtest/gtest.h"

#include "update_pack.hpp"

using namespace command_line_interface;

static const std::string VALID_TAR_PATH = "./giraffe-0.8.0-beta-dev.tar.gz";
static const std::string SOFTWARE_BASE_DIR =
    "./giraffe_update_pack_test_software";
static const std::string CURRENT_DIR = SOFTWARE_BASE_DIR + "/current";
static const std::string UPDATE_DIR = SOFTWARE_BASE_DIR + "/update";
class giraffe_updatePack : public ::testing::Test {
protected:
  virtual void SetUp() {
    initDirs();
  }
  virtual void TearDown() {
  }

  size_t getNumFilesInUpdateDir() {
    size_t count = 0;
    for (const auto &entry : std::filesystem::directory_iterator(UPDATE_DIR)) {
      count++;
    }
    return count;
  }

  void initDirs() {
    try {
      if (std::filesystem::exists(SOFTWARE_BASE_DIR)) {
        std::filesystem::remove_all(SOFTWARE_BASE_DIR);
      }
      std::filesystem::create_directory(SOFTWARE_BASE_DIR);
      std::filesystem::create_directory(CURRENT_DIR);
      std::filesystem::create_directory(UPDATE_DIR);
    } catch (const std::filesystem::filesystem_error &e) {
      std::cerr << "Error creating directories: " << e.what() << std::endl;
    }
  }

  /// @brief UpdatePack with cache loading disabled
  UpdatePack up_ = UpdatePack(CURRENT_DIR, UPDATE_DIR, false);
};

/// @test command_line_interface::UpdatePack - constructor
TEST_F(giraffe_updatePack, constructor) {
  EXPECT_FALSE(up_.isValid());
}

/// @test command_line_interface::UpdatePack::setTarFilePath
TEST_F(giraffe_updatePack, processTarFile_invalid_path) {
  EXPECT_FALSE(up_.processTarFile("invalid_path"));
  EXPECT_FALSE(up_.isValid());
}

/// @test command_line_interface::UpdatePack::setTarFilePath
TEST_F(giraffe_updatePack, processTarFile_valid_path) {
  ASSERT_EQ(getNumFilesInUpdateDir(), 0U); // it should start empty

  EXPECT_TRUE(up_.processTarFile(VALID_TAR_PATH));
  EXPECT_TRUE(up_.isValid());

  // A copy of the software_manifest.json file that was tarred can be found at
  // tests/daemon/test_update_pack/gcli_unit_test_tar/software_manifest.json
  auto sv_opt = up_.getSoftwareVersionOption();
  ASSERT_TRUE(sv_opt.has_value());

  auto sv = sv_opt.value();
  EXPECT_EQ(sv.getNumbersAsString(), "0.8.1");
  EXPECT_EQ(sv.getStageAsString(), "alpha");
  EXPECT_EQ(sv.isDevelopmentBuild(), true);
  EXPECT_EQ(sv.getSemanticVersionString(), "0.8.1-alpha-dev");
}