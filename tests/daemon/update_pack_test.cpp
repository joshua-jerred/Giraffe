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

static const std::string VALID_TAR_PATH = "gcli_unit_test_tar.tar.gz";
static const std::string SOFTWARE_BASE_DIR = "./gcli_update_pack_test_software";
static const std::string CURRENT_DIR = SOFTWARE_BASE_DIR + "/current";
static const std::string UPDATE_DIR = SOFTWARE_BASE_DIR + "/update";
class gcli_UpdatePack : public ::testing::Test {
protected:
  virtual void SetUp() {
    initDirs();
  }
  virtual void TearDown() {
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

/// @test command_line_interface::UpdatePack-constructor
TEST_F(gcli_UpdatePack, constructor) {
  EXPECT_FALSE(up_.isSourceTarPathValid());
  EXPECT_FALSE(up_.isValid());
}

/// @test command_line_interface::UpdatePack::setTarFilePath
TEST_F(gcli_UpdatePack, processTarFile_invalid_path) {
  EXPECT_FALSE(up_.processTarFile("invalid_path"));
  EXPECT_FALSE(up_.isSourceTarPathValid());
  EXPECT_FALSE(up_.isValid());
}

/// @test command_line_interface::UpdatePack::setTarFilePath
TEST_F(gcli_UpdatePack, processTarFile_valid_path) {
  EXPECT_TRUE(up_.processTarFile(VALID_TAR_PATH));
  EXPECT_TRUE(up_.isSourceTarPathValid());
  EXPECT_TRUE(up_.isValid());
}