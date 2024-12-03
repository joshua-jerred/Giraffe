/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   file_paths.hpp
 * @brief  Common file paths for the C++ portion of Giraffe
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-01-23
 * @copyright  2024 (license to be defined)
 */

#ifndef GIRAFFE_FILE_PATHS_HPP_
#define GIRAFFE_FILE_PATHS_HPP_

#include <string>

namespace giraffe {

namespace file_paths {

/**
 * @brief Get the Giraffe Directory Path.
 * @details Requires the HOME environment variable to be set. Will throw a
 * GiraffeException if it is not set.
 *
 * @return std::string The path to the Giraffe directory.
 */
std::string getGiraffeDirectoryPath();

/**
 * @brief Creates the Giraffe directory if it does not exist. Throws a
 * GiraffeException if it cannot be created.
 *
 * @return true - The directory did not exist and was created.
 * @return false - The directory already exists.
 */
bool createGiraffeDirIfNotExists();

/**
 * @brief Returns the path to the Giraffe config file.
 * @return std::string - $HOME/.giraffe/gdl_server.json
 */
std::string getGdlServerConfigFilePath();

/**
 * @brief Returns the path to the Flight Software config file.
 * @return std::string - $HOME/.giraffe/gfs_config.json
 */
std::string getGfsConfigFilePath();

/**
 * @brief Returns the path to the gfs image directory. Creates the directory if
 * it does not exist.
 * @return std::string - $HOME/.giraffe/gfs_images
 */
std::string getGfsImageDirPath();

/**
 * @brief Returns the path to the gfs data directory. Creates the directory if
 * it does not exist.
 * @return std::string - $HOME/.giraffe/gfs_data
 */
std::string getGfsDataDirPath();

/**
 * @brief Returns the path to the gfs data archive directory. Creates the
 * directory if it does not exist.
 * @return std::string - $HOME/.giraffe/gfs_data/archive
 */
std::string getGfsDataArchiveDirPath();

/**
 * @brief Returns the path to the gfs log directory. Creates the directory if
 * it does not exist.
 * @return std::string - $HOME/.giraffe/gfs_log
 */
std::string getGfsLogDirPath();

/**
 * @brief Returns the path to the gfs log archive directory. Creates the
 * directory if it does not exist.
 * @return std::string - $HOME/.giraffe/gfs_log/archive
 */
std::string getGfsLogArchiveDirPath();

/**
 * @brief Get the Gfs Flight Runner Data File Path object
 * @return std::string - $HOME/.giraffe/flight_runner_data.json
 */
std::string getGfsFlightRunnerDataFilePath();

/**
 * @brief Get the path to the Flight System Agent directory.
 * @return std::string - $HOME/.giraffe/flight_system_agent
 */
std::string getFlightSystemAgentDirPath();

/// @brief Get the path to the Flight System Agent log file.
/// @return std::string - $HOME/.giraffe/flight_system_agent.log
std::string getFlightSystemAgentLogFilePath();

/// @brief Get the path to the Flight System Agent config file.
/// @return std::string - $HOME/.giraffe/fsa_config.json
std::string getFlightSystemAgentConfigFilePath();

/// @brief Generates a file name with the current time as a prefix.
/// @param extension - The file extension to use
/// @return <utc data time>.<extension>
/// @warning This function will not generate unique file names if called
/// multiple times within the same second.
std::string generateFileNameWithTimestamp(const std::string &extension);

} // namespace file_paths

} // namespace giraffe

#endif /* GIRAFFE_FILE_PATHS_HPP_ */