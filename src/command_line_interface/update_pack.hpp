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

namespace command_line_interface {

class UpdatePack {
public:
  /// @brief constructor
  /// @param current_dir - The current software directory, where the currently
  /// running binaries live.
  /// @param update_dir - The update software directory, where the new binaries
  /// live.
  UpdatePack(const std::string &current_dir, const std::string &update_dir,
             bool load_cache = true)
      : current_dir_(current_dir), update_dir_(update_dir) {
    if (load_cache) {
      loadCachedValidUpdatePack();
    }
  }

  ~UpdatePack() = default;

  UpdatePack(const UpdatePack &) = delete;            // copy constructor
  UpdatePack &operator=(const UpdatePack &) = delete; // copy assignment
  UpdatePack(UpdatePack &&) = delete;                 // move constructor

  bool isSourceTarPathValid() const {
    return is_source_tar_path_valid_;
  }

  /// @brief Process the tar file at the path set by setTarFilePath.
  /// @return The same value as isValid().
  bool processTarFile(const std::filesystem::path &path) {
    info("Processing update pack: " + source_tar_path_);

    if (!setTarFilePath(path)) {
      // error("Error setting tar file path");
      return false;
    }

    // CHECK OUR VERSIONS/CACHE HERE
    // this part can be skipped if we've already unpacked it.
    // clean-up the update directory if it exists

    // if (!prepareUpdateDirectory()) {
    //   error("Error preparing update directory");
    //   return false;
    // }

    if (!isSourceTarPathValid()) {
      // error("Source tar path is invalid - shouldn't be processing");
      return false;
    }

    if (!unTarSourceTarFile()) {
      // error("Error untarring update pack");
      return false;
    }

    if (!readVersionFile()) {
      error("Error reading version file");
      return false;
    }

    is_update_pack_valid_ = true;
    // valid_update_pack_file_name_ = path.filename().string();
    // valid_update_pack_path_ = path.string();
    // last_write_time_seconds =
    //     std::chrono::duration_cast<std::chrono::seconds>(
    //         std::filesystem::last_write_time(path).time_since_epoch())
    //         .count();
    // file_size_bytes = std::filesystem::file_size(path);

    info("Update pack processed successfully");

    cacheValidUpdatePack();
    return true;
  }

  /// @brief Get the validity status of the update pack. This is set after
  /// un-tarring and processing the update pack. This means it's ready to be
  /// applied, real software update files exist somewhere.
  /// @return \c true if the update pack is valid, \c false otherwise.
  bool isValid() const {
    return is_update_pack_valid_;
  }

  void clear() {
    is_source_tar_path_valid_ = false;
    is_update_pack_valid_ = false;
    source_tar_file_name_ = "";
    source_tar_path_ = "";
    source_tar_timestamp_ = 0;
    source_tar_size_bytes_ = 0;
  }

  const std::string &getFileName() const {
    return source_tar_file_name_;
  }

  void cacheValidUpdatePack() {
    // if (!have_valid_update_file_) {
    //   error("No valid update pack to cache");
    //   return;
    // }

    // try {
    //   std::ofstream cache_file(valid_update_pack_cache);
    //   cache_file << have_valid_update_file_ << std::endl;
    //   cache_file << valid_update_pack_file_name_ << std::endl;
    //   cache_file << valid_update_pack_path_ << std::endl;
    //   cache_file << valid_update_pack_time_ << std::endl;
    //   cache_file << valid_update_pack_size_ << std::endl;
    // } catch (const std::exception &e) {
    //   error("Error caching valid update pack: " + std::string(e.what()));
    // }
  }

  void loadCachedValidUpdatePack() {
    // try {
    //   std::ifstream cache_file(valid_update_pack_cache);
    //   cache_file >> have_valid_update_file_;
    //   cache_file >> valid_update_pack_file_name_;
    //   cache_file >> valid_update_pack_path_;
    //   cache_file >> valid_update_pack_time_;
    //   cache_file >> valid_update_pack_size_;

    //   if (have_valid_update_file_) {
    //     std::cout << "Loaded cached valid update pack: "
    //               << valid_update_pack_path_ << std::endl;
    //   }
    // } catch (const std::exception &e) {
    //   error("Error loading cached valid update pack: " +
    //   std::string(e.what()));
    // }
  }

  /// @brief Check if two UpdatePack *source* tar files are the same-ish (based
  /// on name, size, path, timestamp, not a hash).
  bool operator==(const UpdatePack &other) const {
    return source_tar_file_name_ == other.source_tar_file_name_ &&
           source_tar_path_ == other.source_tar_path_ &&
           source_tar_timestamp_ == other.source_tar_timestamp_ &&
           source_tar_size_bytes_ == other.source_tar_size_bytes_;
  }

private:
  /// @brief Set the source tar file path and load it's metadata. Does not
  /// un-tar the file.
  /// @param path - The path to the source tar file.
  /// @return \c true if the file exists and is a regular file, \c false
  /// otherwise.
  bool setTarFilePath(const std::filesystem::path &path);

  bool unTarSourceTarFile() {
    info("Untarring update pack |\n  from - " + source_tar_path_ +
         " |\n  to   - " + update_dir_);

    try {
      // --strip-components=1 will remove the top level directory from the tar
      // file but keep the inner contents.
      bst::Process untar_process("tar", {"--strip-components", "1", "-xvf",
                                         source_tar_path_, "-C", update_dir_});

      constexpr size_t TIMEOUT_MS = 3000;
      untar_process.waitToComplete(TIMEOUT_MS);

      if (untar_process.getExitCode() != 0) {
        error("Error untarring update pack: " + untar_process.getStderr());
        return false;
      }
    } catch (const std::exception &e) {
      error("Error untarring update pack: " + std::string(e.what()));
      return false;
    }

    info("Untar complete");
    return true;
  }

  bool readVersionFile() {
    info("Reading version file");
    // try {
    //   // Open the version file and check if it is valid
    //   const std::filesystem::path version_file_path =
    //       update_dir_ + "/version.json";
    //   if (!bst::filesystem::doesFileExist(version_file_path)) {
    //     error("Update pack missing version file");
    //     return;
    //   }

    //   // Read the version file
    //   std::ifstream version_file(version_file_path);
    //   Json version_json = Json::parse(version_file);

    // } catch (const std::exception &e) {
    //   error("Error reading version file: " + std::string(e.what()));
    // }
    info("Version file read successfully");
    return true;
  }

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

  /// @brief Validity of the update pack (after un-tarring and processing)
  bool is_update_pack_valid_ = false;

  /// @brief The current software directory, where the currently running
  /// binaries live.
  const std::string current_dir_;

  /// @brief The update software directory, where the new binaries live.
  const std::string update_dir_;
};

} // namespace command_line_interface