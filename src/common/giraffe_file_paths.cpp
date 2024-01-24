/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   giraffe_file_paths.cpp
 * @brief  Common file paths for the C++ portion of Giraffe
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-01-23
 * @copyright  2024 (license to be defined)
 */

#include "BoosterSeat/filesystem.hpp"

#include "giraffe_exception.hpp"
#include "giraffe_file_paths.hpp"

namespace giraffe::file_paths {

constexpr char GIRAFFE_DIRECTORY[] = ".giraffe";

std::string getGiraffeDirectoryPath() {
  char *home_env = getenv("HOME");

  if (home_env == nullptr) {
    throw GiraffeException(DiagnosticId::GENERIC_homeEnvVarNotSet,
                           "home_env is nullptr");
  }

  std::string home(home_env);

  if (home.empty()) {
    throw GiraffeException(DiagnosticId::GENERIC_homeEnvVarNotSet,
                           "home is empty");
  }

  if (!BoosterSeat::filesystem::doesDirectoryExist(home)) {
    throw GiraffeException(DiagnosticId::GENERIC_homeEnvVarNotDir);
  }

  return home + "/" + GIRAFFE_DIRECTORY;
}

bool createGiraffeDirIfNotExists() {
  std::string giraffe_dir_path = getGiraffeDirectoryPath();

  if (BoosterSeat::filesystem::doesDirectoryExist(giraffe_dir_path)) {
    return false;
  }

  try {
    BoosterSeat::filesystem::createDirectory(giraffe_dir_path);
  } catch (const std::exception &e) {
    throw GiraffeException(DiagnosticId::GENERIC_failedToCreateGiraffeDir,
                           "Failed to create giraffe directory: " +
                               std::string(e.what()));
  }

  if (!BoosterSeat::filesystem::doesDirectoryExist(giraffe_dir_path)) {
    throw GiraffeException(DiagnosticId::GENERIC_failedToCreateGiraffeDir,
                           "Failed to create giraffe directory");
  }

  return true;
}

std::string getGdlServerConfigFilePath() {
  return getGiraffeDirectoryPath() + "/gdl_server.json";
}

std::string getGfsConfigFilePath() {
  return getGiraffeDirectoryPath() + "/gfs_config.json";
}

} // namespace giraffe::file_paths