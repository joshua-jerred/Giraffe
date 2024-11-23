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

#include <BoosterSeat/filesystem.hpp>
#include <BoosterSeat/time.hpp>

#include "giraffe_exception.hpp"
#include "giraffe_file_paths.hpp"

namespace giraffe::file_paths {

const std::string GIRAFFE_DIRECTORY = ".giraffe";

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

  if (!bst::filesystem::doesDirectoryExist(home)) {
    throw GiraffeException(DiagnosticId::GENERIC_homeEnvVarNotDir);
  }

  return home + "/" + GIRAFFE_DIRECTORY;
}

bool createGiraffeDirIfNotExists() {
  std::string giraffe_dir_path = getGiraffeDirectoryPath();

  if (bst::filesystem::doesDirectoryExist(giraffe_dir_path)) {
    return false;
  }

  try {
    bst::filesystem::createDirectory(giraffe_dir_path);
  } catch (const std::exception &e) {
    throw GiraffeException(DiagnosticId::GENERIC_failedToCreateGiraffeDir,
                           "Failed to create giraffe directory: " +
                               std::string(e.what()));
  }

  if (!bst::filesystem::doesDirectoryExist(giraffe_dir_path)) {
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

std::string getGfsFlightRunnerDataFilePath() {
  return getGiraffeDirectoryPath() + "/flight_runner_data.json";
}

std::string getGfsImageDirPath() {
  createGiraffeDirIfNotExists();
  std::string dir_path = getGiraffeDirectoryPath() + "/gfs_images";

  // -- create the directory if it does not exist
  if (!bst::filesystem::doesDirectoryExist(dir_path)) {
    try {
      bst::filesystem::createDirectory(dir_path);
    } catch (const std::exception &e) {
      throw GiraffeException(DiagnosticId::GENERIC_failedToCreateGfsImageDir,
                             "0: " + std::string(e.what()));
    }

    if (!bst::filesystem::doesDirectoryExist(dir_path)) {
      throw GiraffeException(DiagnosticId::GENERIC_failedToCreateGfsImageDir,
                             "1");
    }
  }

  return dir_path;
}

std::string getGfsDataDirPath() {
  createGiraffeDirIfNotExists();
  std::string dir_path = getGiraffeDirectoryPath() + "/gfs_data";

  // -- create the directory if it does not exist
  if (!bst::filesystem::doesDirectoryExist(dir_path)) {
    try {
      bst::filesystem::createDirectory(dir_path);
    } catch (const std::exception &e) {
      throw GiraffeException(DiagnosticId::GENERIC_failedToCreateGfsDataDir,
                             "0: " + std::string(e.what()));
    }

    if (!bst::filesystem::doesDirectoryExist(dir_path)) {
      throw GiraffeException(DiagnosticId::GENERIC_failedToCreateGfsDataDir,
                             "1");
    }
  }

  return dir_path;
}

std::string getGfsDataArchiveDirPath() {
  createGiraffeDirIfNotExists();
  std::string dir_path = getGfsDataDirPath() + "/archive";

  // -- create the directory if it does not exist
  if (!bst::filesystem::doesDirectoryExist(dir_path)) {
    try {
      bst::filesystem::createDirectory(dir_path);
    } catch (const std::exception &e) {
      throw GiraffeException(DiagnosticId::GENERIC_failedToCreateGfsDataDir,
                             "0: " + std::string(e.what()));
    }

    if (!bst::filesystem::doesDirectoryExist(dir_path)) {
      throw GiraffeException(DiagnosticId::GENERIC_failedToCreateGfsDataDir,
                             "1");
    }
  }

  return dir_path;
}

std::string getGfsLogDirPath() {
  createGiraffeDirIfNotExists();
  std::string dir_path = getGiraffeDirectoryPath() + "/gfs_log";

  // -- create the directory if it does not exist
  if (!bst::filesystem::doesDirectoryExist(dir_path)) {
    try {
      bst::filesystem::createDirectory(dir_path);
    } catch (const std::exception &e) {
      throw GiraffeException(DiagnosticId::GENERIC_failedToCreateGfsLogDir,
                             "0:0: " + std::string(e.what()));
    }

    if (!bst::filesystem::doesDirectoryExist(dir_path)) {
      throw GiraffeException(DiagnosticId::GENERIC_failedToCreateGfsLogDir,
                             "0:1");
    }
  }

  return dir_path;
}

std::string getGfsLogArchiveDirPath() {
  createGiraffeDirIfNotExists();
  std::string dir_path = getGfsLogDirPath() + "/archive";

  // -- create the directory if it does not exist
  if (!bst::filesystem::doesDirectoryExist(dir_path)) {
    try {
      bst::filesystem::createDirectory(dir_path);
    } catch (const std::exception &e) {
      throw GiraffeException(DiagnosticId::GENERIC_failedToCreateGfsLogDir,
                             "1:0: " + std::string(e.what()));
    }

    if (!bst::filesystem::doesDirectoryExist(dir_path)) {
      throw GiraffeException(DiagnosticId::GENERIC_failedToCreateGfsLogDir,
                             "1:1");
    }
  }

  return dir_path;
}

std::string getFlightSystemAgentDirPath() {
  createGiraffeDirIfNotExists();
  std::string dir_path = getGiraffeDirectoryPath() + "/flight_system_agent";

  // -- create the directory if it does not exist
  if (!bst::filesystem::doesDirectoryExist(dir_path)) {
    try {
      bst::filesystem::createDirectory(dir_path);
    } catch (const std::exception &e) {
      throw GiraffeException(DiagnosticId::GENERIC_failedToCreateGfsLogDir,
                             "0:0: " + std::string(e.what()));
    }

    if (!bst::filesystem::doesDirectoryExist(dir_path)) {
      throw GiraffeException(DiagnosticId::GENERIC_failedToCreateGfsLogDir,
                             "0:1");
    }
  }

  return dir_path;
}

std::string getFlightSystemAgentLogFilePath() {
  const std::string log_file_path =
      getFlightSystemAgentDirPath() + "/flight_system_agent.log";
  if (!bst::filesystem::doesFileExist(log_file_path)) {
    try {
      bst::filesystem::createFile(log_file_path);
    } catch (const std::exception &e) {
      throw GiraffeException(DiagnosticId::GENERIC_failedToCreateGfsLogDir,
                             "agent-log-1" + std::string(e.what()));
    }

    if (!bst::filesystem::doesFileExist(log_file_path)) {
      throw GiraffeException(DiagnosticId::GENERIC_failedToCreateGfsLogDir,
                             "agent-log-2");
    }
  }

  return log_file_path;
}

std::string generateFileNameWithTimestamp(const std::string &extension) {
  constexpr bst::time::TimeZone K_TIME_ZONE = bst::time::TimeZone::UTC;
  constexpr char K_DATE_DELIMITER = '_';
  constexpr char K_BETWEEN_DELIMITER = '_';
  constexpr char K_TIME_DELIMITER = '\0';
  std::string timestamp = bst::time::dateAndTimeString(
      K_TIME_ZONE, K_DATE_DELIMITER, K_BETWEEN_DELIMITER, K_TIME_DELIMITER);

  return timestamp + extension;
}

} // namespace giraffe::file_paths