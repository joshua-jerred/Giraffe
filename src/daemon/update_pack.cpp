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

#include "update_pack.hpp"

namespace command_line_interface {

bool UpdatePack::setTarFilePath(const std::filesystem::path &path) {
  try {
    if (!std::filesystem::exists(path) ||
        !std::filesystem::is_regular_file(path)) {
      is_source_tar_path_valid_ = false;
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
    return false;
  }

  return is_source_tar_path_valid_;
}

} // namespace command_line_interface