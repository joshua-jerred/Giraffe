/// =*========GIRAFFE========*=
/// A Unified Flight Command and Control System
/// https://github.com/joshua-jerred/Giraffe
/// https://giraffe.joshuajer.red/
/// =*=======================*=
///
/// @file   ground_software_manager.cpp
///
/// =*=======================*=
/// @author     Joshua Jerred (https://joshuajer.red)
/// @date       2024-12-19
/// @copyright  2024 (license to be defined)

#include <BoosterSeat/process.hpp>

#include "json.hpp"

#include "ground_software_manager.hpp"

namespace command_line_interface {

// bool GroundSoftwareManager::checkForUpdateFile() {
//   const std::string UPDATE_FILE_PREFIX = "giraffe-update";

//   // Once we have an update file, we don't need to check for more as long
//   // as it is still there.
//   if (update_pack_.isValid()) {
//     try {
//       // If the update file still exists, make sure it is still identical
//       // to what we marked as valid.
//       if (bst::filesystem::doesFileExist(valid_update_pack_path_)) {
//         return true;
//       }

//       have_valid_update_file_ = false;
//       return false;
//     } catch (const std::filesystem::filesystem_error &e) {
//       error("Error checking for update file: " + std::string(e.what()));
//       have_valid_update_file_ = false;
//       return false;
//     }
//   }

//   try {
//     // Keep track of what paths we hit so we can clean up old files
//     for (const auto &entry :
//          std::filesystem::directory_iterator(software_dir_path_)) {
//       if (!update_pack_.setTarFilePath(entry.path())) {
//         continue;
//       }

//       if (bst::containsPrefix(update_pack_.getFileName(),
//       UPDATE_FILE_PREFIX)) {
//         update_pack_.processTarFile();
//       }
//     }

//   } catch (const std::filesystem::filesystem_error &e) {

//     error("Error checking for update file: " + std::string(e.what()));
//     return false;
//   }

//   return true;
// }

// void GroundSoftwareManager::processUpdateFile(
//     const std::filesystem::path &path) {
//   std::cout << "processing update file: " << path.string() << std::endl;

//   try {

//     // Prepare the update directory (delete and recreate)
//     if (std::filesystem::exists(update_software_dir_path_)) {
//       std::filesystem::remove_all(update_software_dir_path_);
//     }
//     std::filesystem::create_directory(update_software_dir_path_);

//     // std::filesystem::copy(path, update_software_dir_path_ /
//     path.filename());

//     // Un-tar the update pack

//     // Check for file existence and size

//   } catch (const std::exception &e) {
//     error("Error processing update file: " + std::string(e.what()));
//   }
// }

} // namespace command_line_interface