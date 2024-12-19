/// =*========GIRAFFE========*=
/// A Unified Flight Command and Control System
/// https://github.com/joshua-jerred/Giraffe
/// https://giraffe.joshuajer.red/
/// =*=======================*=
///
/// @file   software_update.hpp
///
/// =*=======================*=
/// @author     Joshua Jerred (https://joshuajer.red)
/// @date       2024-12-19
/// @copyright  2024 (license to be defined)

#pragma once

#include <filesystem>

#include <BoosterSeat/string_utils.hpp>
#include <BoosterSeat/timer.hpp>

#include "node.hpp"

namespace command_line_interface {

/// @brief Checks for software updates and installs them when directed
class GroundSoftwareManager {
public:
  GroundSoftwareManager() {
    // Set up the directories
    try {

      if (!std::filesystem::exists(software_dir_path_)) {
        std::filesystem::create_directory(software_dir_path_);
      }
      if (!std::filesystem::exists(update_dir_path_)) {
        std::filesystem::create_directory(update_dir_path_);
      }
    } catch (const std::filesystem::filesystem_error &e) {
      error("Error creating directories: " + std::string(e.what()));
    }
  }

  ~GroundSoftwareManager() = default;

  void process() {
    // Update the file list
    checkForUpdateFile();

    status_ = node::Status::RUNNING;
  }

  node::Status getStatus() const {
    return status_;
  }

private:
  const std::string UPDATE_FILE_PREFIX = "giraffe-update";

  bool checkForUpdateFile() {
    bool found_update_file = false;
    try {
      // Keep track of what paths we hit so we can clean up old files
      for (const auto &entry :
           std::filesystem::directory_iterator(software_dir_path_)) {
        if (!entry.is_regular_file()) {
          continue;
        }

        const auto &path = entry.path();
        const auto &filename = path.filename().string();
        if (bst::containsPrefix(filename, UPDATE_FILE_PREFIX)) {
          if (found_update_file) {
            error("Multiple update files found, only one is allowed");
            return false;
          }
          found_update_file = true;
          foundUpdateFile(path);
        }
      }

      return true;
    } catch (const std::filesystem::filesystem_error &e) {

      error("Error checking for update file: " + std::string(e.what()));
      return false;
    }
  }

  void error(const std::string &message) {
    std::cerr << "Error: " << message << std::endl;
    status_ = node::Status::ERROR;
  }

  void foundUpdateFile(const std::filesystem::path &path) {
    std::cout << "Found update file: " << path.string() << std::endl;
  }

  bool have_valid_update_file_{false};
  std::string valid_update_dir_path_{""};

  node::Status status_{node::Status::UNKNOWN};

  const std::string software_dir_path_{
      giraffe::file_paths::getFlightSystemAgentDirPath() + "/software"};
  const std::string update_dir_path_{software_dir_path_ + "/update"};
};

} // namespace command_line_interface