/// =*========GIRAFFE========*=
/// A Unified Flight Command and Control System
/// https://github.com/joshua-jerred/Giraffe
/// https://giraffe.joshuajer.red/
/// =*=======================*=
///
/// @file   binary_file.cpp
///
/// =*=======================*=
/// @author     Joshua Jerred (https://joshuajer.red)
/// @date       2024-12-21
/// @copyright  2024 (license to be defined)

#include "gtest/gtest.h"

#include "binary_file.hpp"

using namespace command_line_interface;

// static const std::string

static const std::string TEST_BINARY_NAME = "binary_file_test.bin";
static const std::string BINARY_SOURCE_DIR = ".";
static const std::string BINARY_DESTINATION_DIR =
    BINARY_SOURCE_DIR + "/binary_file_test_workdir";

class BinaryFileTest : public ::testing::Test {
protected:
  virtual void SetUp() {

    try {
      if (std::filesystem::exists(BINARY_DESTINATION_DIR)) {
        std::filesystem::remove_all(BINARY_DESTINATION_DIR);
      }

      std::filesystem::create_directory(BINARY_DESTINATION_DIR);
    } catch (const std::filesystem::filesystem_error &e) {
      std::cerr << "Error creating directories: " << e.what() << std::endl;
    }
  }

  virtual void TearDown() {
  }

  /// @brief UpdatePack with cache loading disabled
  // UpdatePack up_ = UpdatePack(CURRENT_DIR, UPDATE_DIR, false);
};

TEST_F(BinaryFileTest, isValid) {
  BinaryFile bin(giraffe::AppIdentifier::COMMAND_LINE, TEST_BINARY_NAME,
                 BINARY_SOURCE_DIR);

  // initialize false
  EXPECT_FALSE(bin.isValid());
  EXPECT_NO_THROW(bin.checkValid());
  EXPECT_TRUE(bin.isValid());

  // EXPECT_FALSE(bf.setSourcePath("./giraffe"));
  // EXPECT_FALSE(bf.setSourcePath("./giraffe", false));
  // EXPECT_TRUE(bf.setSourcePath("./giraffe", true));
}
