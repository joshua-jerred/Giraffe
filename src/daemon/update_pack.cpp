/// =*========GIRAFFE========*=
/// A Unified Flight Command and Control System
/// https://github.com/joshua-jerred/Giraffe
/// https://giraffe.joshuajer.red/
/// =*=======================*=
///
/// @file   update_pack.cpp
///
/// =*=======================*=
/// @author     Joshua Jerred (https://joshuajer.red)
/// @date       2024-12-19
/// @copyright  2024 (license to be defined)

#include "json.hpp"

#include "update_pack.hpp"

namespace command_line_interface {

UpdatePack::UpdatePack(const std::string &current_dir,
                       const std::string &update_dir, bool load_cache)
    : current_dir_(current_dir), update_dir_(update_dir) {
  if (load_cache) {
    loadCachedValidUpdatePack();
  }
}

bool UpdatePack::processTarFile(const std::filesystem::path &path) {
  info("Processing update pack: " + path.string());

  if (!setTarFilePath(path)) {
    // error("Error setting tar file path");
    return false;
  }

  // CHECK OUR VERSIONS/CACHE HERE
  // this part can be skipped if we've already unpacked it.
  // clean-up the update directory if it exists
  if (!isSourceTarFileNew()) {
    info("Update pack already processed");
    return true; // only early return in a happy state, we rely on the previous
                 // processing results.
  }

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

  try {
    validateBinaryFiles();
  } catch (const std::exception &e) {
    error("Error validating binary files: " + std::string(e.what()));
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

bool UpdatePack::setTarFilePath(const std::filesystem::path &path) {
  try {
    if (!std::filesystem::exists(path) ||
        !std::filesystem::is_regular_file(path)) {
      error("Invalid tar file path " + path.string());
      return false;
    }

    source_tar_file_name_ = path.filename().string();
    source_tar_path_ = path.string();
    source_tar_timestamp_ =
        std::filesystem::last_write_time(path).time_since_epoch().count();
    source_tar_size_bytes_ = std::filesystem::file_size(path);

    is_source_tar_path_valid_ = true;
    return true;
  } catch (const std::exception &e) {
    error("Error setting tar file path: " + std::string(e.what()));
    return false;
  }

  return is_source_tar_path_valid_;
}

bool UpdatePack::isSourceTarFileNew() {
  // if (!have_valid_update_file_) {
  //   return false;
  // }

  // if (source_tar_path_ == valid_update_pack_path_) {
  //   return true;
  // }

  return true;
}

bool UpdatePack::prepareUpdateDirectory() {
  info("Preparing update directory");

  /// @todo this may already be handled by the upper level class, consider
  /// implementation carefully.

  // if (std::filesystem::exists(update_software_dir_path_)) {
  //   std::filesystem::remove_all(update_software_dir_path_);
  // }
  // std::filesystem::create_directory(update_software_dir_path_);
  info("Update directory prepared");
  return true;
}

bool UpdatePack::unTarSourceTarFile() {
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

bool UpdatePack::readVersionFile() {
  info("Reading version file");
  try {
    // Open the version file and check if it is valid
    const std::filesystem::path manifest_file_path =
        update_dir_ + "/giraffe_manifest.json";
    if (!std::filesystem::exists(manifest_file_path)) {
      error("Update pack missing manifest file");
      return false;
    }

    // Read the manifest file and parse it
    const Json manifest_json = Json::parse(std::ifstream(manifest_file_path));
    const Json &version_json = manifest_json.at("version");

    // Parse the version and stage. These will throw if they are invalid.
    valid_update_pack_version_.major = version_json.at("major").get<int>();
    valid_update_pack_version_.minor = version_json.at("minor").get<int>();
    valid_update_pack_version_.patch = version_json.at("patch").get<int>();
    valid_update_pack_version_.setStageFromString(
        version_json.at("stage").get<std::string>());
    valid_update_pack_version_.clean = version_json.at("clean").get<bool>();

    const std::string generated_version_string =
        valid_update_pack_version_.getSemanticVersionString();

    std::cout << "we go a version: " << generated_version_string << std::endl;

    info("Update pack version: " +
         valid_update_pack_version_.getNumbersAsString() + " " +
         valid_update_pack_version_.getStageAsString());

  } catch (const std::exception &e) {
    error("Error reading version file: " + std::string(e.what()));
    valid_update_pack_version_.zero();
    return false;
  }

  info("Version file read successfully");
  return true;
}

void UpdatePack::validateBinaryFiles(){
    /// @todo implement this method
};

void UpdatePack::cacheValidUpdatePack() {
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

void UpdatePack::loadCachedValidUpdatePack() {
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

} // namespace command_line_interface