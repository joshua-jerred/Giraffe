/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   file_system.hpp
 * @brief  Management of the giraffe data file system
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-02-19
 * @copyright  2024 (license to be defined)
 */

#ifndef FILE_SYSTEM_HPP_
#define FILE_SYSTEM_HPP_

#include <BoosterSeat/filesystem.hpp>
#include <BoosterSeat/timer.hpp>
#include <filesystem>

#include "configuration.hpp"
#include "giraffe_file_paths.hpp"
#include "logger.hpp"
#include "shared_data.hpp"

namespace data_middleware {

constexpr size_t FILE_SYSTEM_MANAGER_UPDATE_INTERVAL_MS = 2500;

class FileSystemManager {
public:
  FileSystemManager(data::SharedData &shared_data, cfg::Configuration &config)
      : shared_data_(shared_data), config_(config),
        logger_(shared_data.streams.log, node::Identification::DATA_MODULE,
                "fs") {
    /// @todo refactor this (get rid of management through giraffe_filepaths)
    giraffe_directory_path_ = giraffe::file_paths::getGiraffeDirectoryPath();
    gfs_image_dir_path_ = giraffe::file_paths::getGfsImageDirPath();

    /// @todo add error logging
    giraffe_directory_valid_ =
        bst::filesystem::doesDirectoryExist(giraffe_directory_path_);
    gfs_image_dir_valid_ =
        bst::filesystem::doesDirectoryExist(gfs_image_dir_path_);

    // initialize the update timer
    update_timer_.reset();
  }

  ~FileSystemManager() {
  }

  void update() {
    if (!update_timer_.isDone()) {
      return;
    }
    updateGfsImageDirData();

    // update the local data block
    fs_data_.giraffe_directory_valid = giraffe_directory_valid_;
    fs_data_.giraffe_directory_path = giraffe_directory_path_;
    fs_data_.gfs_image_dir_valid = gfs_image_dir_valid_;
    fs_data_.gfs_image_dir_path = gfs_image_dir_path_;

    // update the shared data block and reset the timer
    shared_data_.blocks.file_system_data.set(fs_data_);
    update_timer_.reset();
  }

  bool isGiraffeDirectoryValid() const {
    return giraffe_directory_valid_;
  }

  bool isGfsImageDirValid() const {
    return gfs_image_dir_valid_;
  }

private:
  void updateGfsImageDirData() {
    if (!gfs_image_dir_valid_) {
      return;
    }

    constexpr uint32_t K_MAX_FILES_TO_RECORD = 50;
    uint32_t recorded_file_count = 0;

    fs_data_.gfs_image_files.clear();

    /// @todo The order is undefined, so eventually we should sort the files.
    for (const auto &entry :
         std::filesystem::directory_iterator(gfs_image_dir_path_)) {
      gfs_image_dir_file_count_++;
      if (recorded_file_count >= K_MAX_FILES_TO_RECORD) {
        break;
      }

      if (entry.is_regular_file()) {
        recorded_file_count++;
        fs_data_.gfs_image_files.push_back(entry.path().filename());
      }
    }

    fs_data_.gfs_image_dir_file_count = gfs_image_dir_file_count_;
  }

  bool giraffe_directory_valid_ = false;
  std::string giraffe_directory_path_{};

  bool gfs_image_dir_valid_ = false;
  std::string gfs_image_dir_path_{};
  uint32_t gfs_image_dir_file_count_ = 0;

  data::SharedData &shared_data_;
  cfg::Configuration &config_;
  giraffe::Logger logger_;

  data::blocks::FileSystemData fs_data_{};

  bst::Timer update_timer_{FILE_SYSTEM_MANAGER_UPDATE_INTERVAL_MS};
};

} // namespace data_middleware

#endif /* FILE_MANAGEMENT_HPP_ */
