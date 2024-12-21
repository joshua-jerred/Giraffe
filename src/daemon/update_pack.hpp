/// =*========GIRAFFE========*=
/// A Unified Flight Command and Control System
/// https://github.com/joshua-jerred/Giraffe
/// https://giraffe.joshuajer.red/
/// =*=======================*=
///
/// @file   update_pack.hpp
///
/// =*=======================*=
/// @author     Joshua Jerred (https://joshuajer.red)
/// @date       2024-12-19
/// @copyright  2024 (license to be defined)

#pragma once

#include <cstdint>
#include <filesystem>
#include <string>

#include <BoosterSeat/process.hpp>

#include "software_version.hpp"

namespace command_line_interface {

class UpdatePack {
public:
  /// @brief constructor
  /// @param current_dir - The current software directory, where the currently
  /// running binaries live.
  /// @param update_dir - The update software directory, where the new binaries
  /// live.
  UpdatePack(const std::string &current_dir, const std::string &update_dir,
             bool load_cache = true);
  ~UpdatePack() = default;

  UpdatePack(const UpdatePack &) = delete;            // copy constructor
  UpdatePack &operator=(const UpdatePack &) = delete; // copy assignment
  UpdatePack(UpdatePack &&) = delete;                 // move constructor

  /// @brief Process the tar file at the path set by setTarFilePath.
  /// @return The same value as isValid().
  bool processTarFile(const std::filesystem::path &path);

  /// @brief Get the validity status of the update pack. This is set after
  /// un-tarring and processing the update pack. This means it's ready to be
  /// applied, real software update files exist somewhere.
  /// @return \c true if the update pack is valid, \c false otherwise.
  bool isValid() const {
    return is_update_pack_valid_;
  }

  /// @brief Get the software version of the update pack.
  /// @return \c giraffe::SoftwareVersion object if the update pack is valid and
  /// we have version information, \c std::nullopt otherwise.
  std::optional<giraffe::SoftwareVersion> getSoftwareVersionOption() const {
    if (is_update_pack_valid_) {
      return valid_update_pack_version_;
    }
    return std::nullopt;
  }

  const std::string &getFileName() const {
    return source_tar_file_name_;
  }

  /// @brief Check if two UpdatePack *source* tar files are the same-ish (based
  /// on name, size, path, timestamp, not a hash).
  bool operator==(const UpdatePack &other) const {
    return source_tar_file_name_ == other.source_tar_file_name_ &&
           source_tar_path_ == other.source_tar_path_ &&
           source_tar_timestamp_ == other.source_tar_timestamp_ &&
           source_tar_size_bytes_ == other.source_tar_size_bytes_;
  }

  void clear() {
    is_source_tar_path_valid_ = false;
    is_update_pack_valid_ = false;
    source_tar_file_name_ = "";
    source_tar_path_ = "";
    source_tar_timestamp_ = 0;
    source_tar_size_bytes_ = 0;
  }

private:
  struct BinaryFile {};

  /// @deprecated --- this doesn't may not need to be public any more with
  /// everything wrapped into processTarFile. Also,if it's made private, it
  /// should use an actual filesystem::exists call.
  /// @brief Get the validity status of the source tar file. This only indicates
  /// if the source tar file exists and is a regular file, not if it's contents
  /// are valid.
  /// @return \c true if the source tar file is valid, \c false otherwise.
  bool isSourceTarPathValid() const {
    return is_source_tar_path_valid_;
  }

  /// @brief Set the source tar file path and load it's metadata. Does not
  /// un-tar the file.
  /// @param path - The path to the source tar file.
  /// @return \c true if the file exists and is a regular file, \c false
  /// otherwise.
  bool setTarFilePath(const std::filesystem::path &path);

  /// @brief Check if the source tar file is difference from the current valid
  /// update pack. This is used to prevent re-processing the same update pack.
  /// @return \c true if the source tar file is different from the current valid
  /// update pack, \c false otherwise.
  bool isSourceTarFileNew();

  /// @brief Clean up the update directory if it exists. This is called after
  /// we have a *new* update pack to process.
  bool prepareUpdateDirectory();

  /// @brief Un-tar the source tar file into the update directory.
  /// @return \c true if the un-tar process was successful, \c false otherwise.
  bool unTarSourceTarFile();

  /// @brief Read the version file of the update pack that was un-tarred.
  /// @return \c true if the version file was read successfully, \c false
  /// otherwise.
  bool readVersionFile();

  /// @brief Validate the binary files in the update directory. If any are
  /// invalid, an exception is thrown.
  /// @todo Consider handling the other methods above like this: use a single
  /// try/catch block around the call to this method.
  void validateBinaryFiles();

  /// @brief Save details about the valid update pack to a cache file. This
  /// prevents us from re-processing the same update pack if it's already been
  /// processed.
  void cacheValidUpdatePack();

  /// @brief Load details about a previously cached valid update pack. If cache
  /// does not exist, nothing happen.
  void loadCachedValidUpdatePack();

  /// @todo replace with common logger
  void info(const std::string &message) {
    std::cout << "info: " << message << std::endl;
  }

  /// @todo replace with common logger
  void error(const std::string &message) {
    std::cout << "error: " << message << std::endl;
  }

  /// @brief Validity of the source tar file (does it exist and is it a file)
  bool is_source_tar_path_valid_ = false;
  /// @brief The name of the file
  std::string source_tar_file_name_{};
  /// @brief The path to the file
  std::string source_tar_path_{};
  /// @brief The last write time of the file
  /// @todo this timestamp is not correct, but it is at least unique so it can
  // be used to detect changes. It's negative when stored as a int32_t for
  // some reason.
  uint32_t source_tar_timestamp_{};
  /// @brief The size of the file in bytes
  /// @details 4GB should be reasonable here, I really hope these update packs
  /// remain a reasonable size for what they are. If it's bigger than 4GB, don't
  /// change this data type, make it smaller lol.
  uint32_t source_tar_size_bytes_{};

  /// @brief The version of the update pack. This is only valid when the update
  /// pack is marked as valid.
  giraffe::SoftwareVersion valid_update_pack_version_{};

  /// @brief Validity of the update pack (after un-tarring and processing)
  bool is_update_pack_valid_ = false;

  /// @brief The current software directory, where the currently running
  /// binaries live.
  const std::string current_dir_;

  /// @brief The update software directory, where the new binaries live.
  const std::string update_dir_;
};

} // namespace command_line_interface