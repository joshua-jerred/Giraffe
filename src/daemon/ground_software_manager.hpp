/// =*========GIRAFFE========*=
/// A Unified Flight Command and Control System
/// https://github.com/joshua-jerred/Giraffe
/// https://giraffe.joshuajer.red/
/// =*=======================*=
///
/// @file   ground_software_manager.hpp
///
/// =*=======================*=
/// @author     Joshua Jerred (https://joshuajer.red)
/// @date       2024-12-19
/// @copyright  2024 (license to be defined)

#pragma once

#include <chrono>
#include <filesystem>
#include <iostream>

#include <BoosterSeat/clock.hpp>
#include <BoosterSeat/filesystem.hpp>
#include <BoosterSeat/string_utils.hpp>
#include <BoosterSeat/timer.hpp>

#include "giraffe_file_paths.hpp"
#include "node.hpp"
#include "update_pack.hpp"

namespace command_line_interface {

/// @brief Checks for software updates and installs them when directed
/// @details This class is responsible for handling software versions and
/// updates.
/// Expected behavior:
/// - Install initial software as soon as it's found.
class GroundSoftwareManager {
public:
  GroundSoftwareManager() {
    // Set up the directories
    try {
      if (!std::filesystem::exists(software_dir_path_)) {
        std::filesystem::create_directory(software_dir_path_);
      }
      if (!std::filesystem::exists(update_software_dir_path_)) {
        std::filesystem::create_directory(update_software_dir_path_);
      }
      if (!std::filesystem::exists(current_software_dir_path_)) {
        std::filesystem::create_directory(current_software_dir_path_);
      }
    } catch (const std::filesystem::filesystem_error &e) {
      error("Error creating directories: " + std::string(e.what()));
    }

    loadCachedValidUpdatePack();
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
  /// @todo replace with a general logger
  void error(const std::string &message) {
    std::cout << "Error: " << message << std::endl;
    status_ = node::Status::ERROR;
  }

  /// @brief Process the contents of the update directory and attempt to find an
  /// update file.
  /// @details This method will look through the software directory for an
  /// update file. If one is found it will be processed. If it is processed
  /// successfully, this method will instead continually check if this file
  /// specifically still exists and if it has been updated. It stops looking
  /// when it has the processed update file. The update file can be thrown away,
  /// if it is, go back to looking.
  bool checkForUpdateFile();

  /// @brief Check the update pack file and check if it is valid.
  /// @details Called when a new update pack file is found.
  /// @param path - The path to the update pack file, must be a regular file in
  /// the agent's software directory.
  void processUpdateFile(const std::filesystem::path &path);

  bool have_valid_update_file_{false};
  UpdatePack update_pack_{};

  std::string valid_update_pack_file_name_{""};
  std::string valid_update_pack_path_{""};
  uint32_t valid_update_pack_time_{0};
  uint32_t valid_update_pack_size_{0};

  node::Status status_{node::Status::UNKNOWN};

  const std::string software_dir_path_{
      giraffe::file_paths::getFlightSystemAgentDirPath() + "/software"};
  const std::string current_software_dir_path_{software_dir_path_ + "/current"};
  const std::string update_software_dir_path_{software_dir_path_ + "/update"};
  const std::string valid_update_pack_cache{update_software_dir_path_ +
                                            "/valid_update_pack_cache"};
};

} // namespace command_line_interface