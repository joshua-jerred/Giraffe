/// =*========GIRAFFE========*=
/// A Unified Flight Command and Control System
/// https://github.com/joshua-jerred/Giraffe
/// https://giraffe.joshuajer.red/
/// =*=======================*=
///
/// @file   binary_file.hpp
///
/// =*=======================*=
/// @author     Joshua Jerred (https://joshuajer.red)
/// @date       2024-12-21
/// @copyright  2024 (license to be defined)

#pragma once

#include <cstdint>
#include <filesystem>
#include <string>

#include "giraffe_types.hpp"

namespace command_line_interface {

class BinaryFile {
public:
  /// @brief Construct a new Binary File object
  /// @param app_id - The application identifier.
  /// @param binary_filename - The name of the binary file with it's extension,
  /// doesn't include the path.
  BinaryFile(const giraffe::AppIdentifier app_id,
             const std::string &binary_filename,
             const std::filesystem::path &src_dir_path,
             const std::filesystem::path &dst_dir_path,
             const bool need_read_access = true,
             const bool need_write_access = false,
             const bool need_execute_access = false)
      : app_id_(app_id), source_dir_path_(src_dir_path),
        destination_dir_path_(dst_dir_path), binary_filename_(binary_filename),
        need_read_access_(need_read_access),
        need_write_access_(need_write_access),
        need_execute_access_(need_execute_access) {
  }

  /// @brief Check if the binary file is valid. Returns false if it is not.
  /// @return \c true if the binary file is valid, \c false otherwise.
  bool checkValid() {
    try {
      assertValid();
    } catch (const std::runtime_error &e) {
      error_message_ = e.what();
      have_error_message_ = true;
      return false;
    }

    return true;
  }

  /// @brief Returns the previously checked file validity status.
  /// @return \c true if the binary file is valid, \c false otherwise.
  bool isValid() {
    return is_valid_binary_file_;
  }

  /// @brief Check if the binary fhave_error_message_ile is valid. Throws an
  /// exception if it is not.
  /// @throw std::runtime_error if the binary file is not valid.
  void assertValid() {
    reset();

    does_parent_dir_exist_ = false;
    does_binary_exist_ = false;

    // Check if the parent directory exists
    if (!std::filesystem::exists(source_dir_path_)) {
      throw std::runtime_error("Binary parent directory does not exist " +
                               source_dir_path_.string() + " for " +
                               binary_filename_);
    }
    if (!std::filesystem::is_directory(source_dir_path_)) {
      throw std::runtime_error("Binary parent path is not a directory " +
                               source_dir_path_.string() + " for " +
                               binary_filename_);
    }
    does_parent_dir_exist_ = true;

    // Check if the binary file exists
    const std::filesystem::path binary_file_path =
        source_dir_path_ / binary_filename_;
    if (!std::filesystem::exists(binary_file_path)) {
      throw std::runtime_error("Binary file does not exist " +
                               binary_file_path.string());
    }
    if (!std::filesystem::is_regular_file(binary_file_path)) {
      throw std::runtime_error("Binary file is not a regular file " +
                               binary_file_path.string());
    }
    does_binary_exist_ = true;

    binary_file_size_ = std::filesystem::file_size(binary_file_path);

    auto permissions = std::filesystem::status(binary_file_path).permissions();
    have_read_access_ = (permissions & std::filesystem::perms::owner_read) !=
                        std::filesystem::perms::none;
    have_write_access_ = (permissions & std::filesystem::perms::owner_write) !=
                         std::filesystem::perms::none;
    have_execute_access_ = (permissions & std::filesystem::perms::owner_exec) !=
                           std::filesystem::perms::none;

    if (need_read_access_ && !have_read_access_) {
      throw std::runtime_error("Binary file does not have read access " +
                               binary_file_path.string());
    }

    if (need_write_access_ && !have_write_access_) {
      throw std::runtime_error("Binary file does not have write access " +
                               binary_file_path.string());
    }

    if (need_execute_access_ && !have_execute_access_) {
      throw std::runtime_error("Binary file does not have execute access " +
                               binary_file_path.string());
    }

    is_valid_binary_file_ = true;
  }

private:
  void reset() {
    have_read_access_ = false;
    have_write_access_ = false;
    have_execute_access_ = false;
    does_parent_dir_exist_ = false;
    does_binary_exist_ = false;
    binary_file_size_ = 0;

    is_valid_binary_file_ = false;

    have_error_message_ = false;
    error_message_ = "";
  }

  const giraffe::AppIdentifier app_id_;
  const std::filesystem::path source_dir_path_;
  const std::string binary_filename_;

  const bool need_read_access_;
  const bool need_write_access_;
  const bool need_execute_access_;

  bool does_parent_dir_exist_ = false;
  bool does_binary_exist_ = false;
  uint64_t binary_file_size_ = 0;

  bool have_read_access_ = false;
  bool have_write_access_ = false;
  bool have_execute_access_ = false;

  /// @brief The final 'yes' or 'no' answer to the question of whether the
  /// binary file is valid.
  bool is_valid_binary_file_ = false;

  bool have_error_message_ = false;
  std::string error_message_{};
};

} // namespace command_line_interface