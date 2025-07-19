/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   data_log.hpp
 * @brief  The data log is responsible for writing data to files and managing
 * the file system.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-06-30
 * @copyright  2023 (license to be defined)
 */

#ifndef DATA_LOG_HPP_
#define DATA_LOG_HPP_

#include "configuration.hpp"
#include "data_formatting.hpp"
#include "shared_data.hpp"
#include <BoosterSeat/stopwatch.hpp>
#include <BoosterSeat/timer.hpp>

namespace data_middleware {

/**
 * @brief Used to write data and log information to files. Used by the data
 * module.
 */
class DataLog {

public:
  DataLog(data::SharedData &shared_data, cfg::Configuration &config);
  ~DataLog() = default;

  /**
   * @brief Used to add a single data packet to the file log.
   * @param packet @see data::DataPacket
   */
  void logDataPacket(const data::DataPacket &packet);

  /**
   * @brief Used for periodic logging of all data.
   * @details The strategy was recently read from the configuration prior to
   * this method being called (from the data module). No need to read it again.
   * @param strategy - The strategy to use for logging.
   */
  void logDataFrame(cfg::gEnum::LogStrategy strategy);

  void logErrorFrame();

  /**
   * @brief Used to log a single log packet to the file log.
   * @param packet @see data::LogPacket
   */
  void logLogPacket(const data::LogPacket &packet);

private:
  /**
   * @brief Used to append data to the data file.
   * @param content - The content to append.
   */
  void appendToDataFile(const std::string &content);

  /**
   * @brief Used to append data to the log file.
   * @param content - The content to append.
   */
  void appendToLogFile(const std::string &content);

  /**
   * @brief Handles the file system management.
   *
   * @details Checks the following:
   * - Creates a new log file if necessary.
   *   - If it's the first start.
   *   - If the current log file is too large.
   * - Checks the log file(s) for validity and size.
   * - Checks the archive directories for size.
   * It then updates the data block with relevant information.
   *
   * @see kMaxDirCreationAttempts
   * @todo Error Log
   */
  void updateFileSystem();

  /**
   * @brief Checks for the existence of the data log directories and files.
   * @details
   * - Checks if the directories exist.
   * - If they do not it will attempt to create them a set number of times.
   *
   *
   */
  void validateFileSystem();

  //// START //// File System Management Methods
  /**
   * @defgroup DataLog.Filesystem
   * @brief Used to generate the file structure and manage the log files.
   * @details These methods all use bst::filesystem to create the
   * necessary directories and files for the data log. Any errors or exceptions
   * should be caught and logged explicitly with unique error codes for each.
   *
   * DataDir: ./data
   * DataArchiveDir: ./data/archive
   * DataFile: ./data/data_YYYY-MM-DD_HHMMSS.json
   *
   * LogDir: ./log
   * LogArchiveDir: ./log/archive
   * LogFile: ./log/log_YYYY-MM-DD_HHMMSS.json
   *
   * @{
   */

  /**
   * @brief Attempt to create a directory, log any errors, and set the validity
   * flag in fs_status_.
   *
   * @param path - The path to the directory.
   * @param booster_seat_log_id - The log id for
   * bst::BoosterSeatException.
   * @param std_except_log_id - The log id for std::exception.
   * @param validity_flag - The flag of fs_status_ to set if the directory is
   * valid.
   *
   * @see DataLog::FsStatus
   */
  void createDirectory(const std::string &path,
                       const DiagnosticId booster_seat_log_id,
                       const DiagnosticId std_except_log_id,
                       bool &validity_flag);

  /**
   * @brief Attempt to create a file, log any errors, and set the validity
   * flag in fs_status_.
   *
   * @param new_file_path - The path to the directory.
   * @param booster_seat_log_id - The log id for
   * bst::BoosterSeatException.
   * @param std_except_log_id - The log id for std::exception.
   * @param validity_flag - The flag of fs_status_ to set if the directory is
   * valid.
   *
   * @see DataLog::FsStatus
   */
  void createFile(const std::string &new_file_path,
                  const DiagnosticId booster_seat_log_id,
                  const DiagnosticId std_except_log_id, bool &validity_flag);

  /**
   * @brief Used to create the data directory with createDirectory().
   */
  void createDataDir();

  /**
   * @brief Used to create the log directory with createDirectory().
   */
  void createLogDir();

  /**
   * @brief Used to create the data archive directory with createDirectory().
   */
  void createDataArchiveDir();

  /**
   * @brief Used to create the log archive directory with createDirectory().
   */
  void createLogArchiveDir();

  /**
   * @brief Generates a new file path and attempts to create a new data file.
   */
  void createNewDataFile();

  /**
   * @brief Generates a new file path and attempts to create a new log file.
   */
  void createNewLogFile();

  /**
   * @brief Validates the existence of a directory.
   * @param path - The path to the directory.
   * @param does_not_exist_log_id - The error id to report if the directory does
   * not exist.
   * @param booster_seat_log_id - The error id to report if
   * bst::BoosterSeatException is thrown.
   * @param std_except_log_id - The error id to report if std::exception is
   * thrown.
   * @param validity_flag (out) - The 'valid' flag to set if the directory is
   * valid.
   */
  void validateDirExists(const std::string &path,
                         const DiagnosticId does_not_exist_log_id,
                         const DiagnosticId booster_seat_log_id,
                         const DiagnosticId std_except_log_id,
                         bool &validity_flag);

  /**
   * @brief Validates the existence of a file.
   * @param path - The path to the file.
   * @param does_not_exist_log_id - The error id to report if the file does not
   * exist.
   * @param booster_seat_log_id - The error id to report if
   * bst::BoosterSeatException is thrown.
   * @param std_except_log_id - The error id to report if std::exception is
   * thrown.
   * @param validity_flag (out) - The 'valid' flag to set if the file is valid.
   */
  void validateFileExists(const std::string &path,
                          const DiagnosticId does_not_exist_log_id,
                          const DiagnosticId booster_seat_log_id,
                          const DiagnosticId std_except_log_id,
                          bool &validity_flag);

  /**
   * @brief Attempts to append data to a file. Only checks the file validity.
   *
   * @param path - The path to the file.
   * @param data - The string to append.
   * @param error_id - The error id to log if an error occurs.
   */
  void appendToFile(const std::string &path, const std::string &data,
                    const DiagnosticId error_id);

  /**
   * @brief Attempts to read the size of a  file.
   * @param file_path - The path to the file.
   * @param error_id - The error id to log if an error occurs.
   * @param file_size (out) - The variable to store the file size in.
   */
  void updateFileSize(const std::string &file_path, const DiagnosticId error_id,
                      data::blocks::DataLogStats::FileSizeType &file_size);

  /**
   * @brief Attempts to read the size of a directory.
   *
   * @param dir_path - The path to the directory.
   * @param error_id - The error id to log if an error occurs.
   * @param dir_size - The variable to store the directory size in.
   */
  void updateDirSize(const std::string &dir_path, const DiagnosticId error_id,
                     data::blocks::DataLogStats::FileSizeType &dir_size);

  /**
   * @brief Moves a file to the archive directory according to the archive
   * method.
   *
   * @param file_path - The path to the file to archive.
   * @param archive_dir_path - The path to the archive directory.
   * @param error_id - The error id to log if an error occurs.
   * @return true if the file was archived successfully, false otherwise.
   *
   * @todo Archive Methods
   */
  bool archiveFile(const std::string &file_path,
                   const std::string &archive_dir_path,
                   const DiagnosticId error_id);

  /**
   * @brief Called only startup. If there are any files in the data or log
   * directories that have been left behind, they will be archived. Not
   * recursive.
   * @param dir_path - The path to the directory to check for stray files.
   * @param archive_dir_path - The path to the archive directory.
   * @param error_id - The error id to log if an error occurs.
   */
  void archiveOtherFilesInDir(const std::string &dir_path,
                              const std::string &archive_dir_path,
                              DiagnosticId error_id);

  /**
   * @brief This method checks the size of the data and log files and archives
   * them if they have exceeded the maximum size.
   * @details This method first checks the size of the two files, if they are
   * both under the maximum size, nothing happens. If a file is over the maximum
   * configured size, it will attempt to archive the file and then create a new
   * one.
   */
  void rotateFiles();

  /**
   * @brief This method will use the configured archive settings to trim the
   * oldest files in the archive directory if the archive size limit is
   * exceeded.
   */
  void trimArchive();

  /**
   * @brief This method will update the file list in the relevant data block.
   */
  void updateFileList();

  /** @} */ // end of DataLog.Filesystem
  //// END //// File System Management Methods

  std::string data_dir_path_{};
  std::string data_archive_dir_path_{};
  std::string log_dir_path_{};
  std::string log_archive_dir_path_{};

  /**
   * @brief The shared data.
   */
  data::SharedData &shared_data_;

  /**
   * @brief The configuration.
   */
  cfg::Configuration &config_;

  /**
   * @brief The file system status data block. Used to keep track of the file
   * system locally to reduce the number of times the data block is set.
   */
  data::blocks::DataLogStats fs_status_ = {};

  /**
   * @brief Used within 'updateFileSystem'. Keeps track of the last time the
   * file system was checked.
   */
  bst::Stopwatch validation_stopwatch_{};

  /**
   * @brief Used to time how often the full data frame is logged if the
   * appropriate log strategy is selected.
   */
  bst::Stopwatch data_frame_stopwatch_ = bst::Stopwatch();

  /**
   * @brief Used to time how often the error data frame is logged if the
   * appropriate error log strategy is selected.
   */
  bst::Stopwatch error_frame_stopwatch_ = bst::Stopwatch();

  /**
   * @brief Used to format the data before it is written to the files. (turns
   * things into json strings.)
   */
  DataFormatter formatter_;

  /// @brief A timer used to reduce the number of file system updates.
  /// @details Updates the file listing at a maximum of every 5 seconds.
  bst::Timer maximum_file_system_update_timer_{5000};
};

} // namespace data_middleware

#endif /* DATALOG_HPP_ */