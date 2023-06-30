#ifndef DATA_LOG_HPP_
#define DATA_LOG_HPP_

#include "configuration.hpp"
#include "data_formatting.hpp"
#include "shared_data.hpp"
#include <BoosterSeat/stopwatch.hpp>

namespace data_middleware {

/**
 * @brief Used to write data and log information to files. Used by the data
 * module.
 *
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
   * @details These methods all use BoosterSeat::filesystem to create the
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
   * BoosterSeat::BoosterSeatException.
   * @param std_except_log_id - The log id for std::exception.
   * @param validity_flag - The flag of fs_status_ to set if the directory is
   * valid.
   *
   * @see DataLog::FsStatus
   */
  void createDirectory(const std::string &path,
                       const data::LogId booster_seat_log_id,
                       const data::LogId std_except_log_id,
                       bool &validity_flag);

  /**
   * @brief Attempt to create a file, log any errors, and set the validity
   * flag in fs_status_.
   *
   * @param new_file_path - The path to the directory.
   * @param booster_seat_log_id - The log id for
   * BoosterSeat::BoosterSeatException.
   * @param std_except_log_id - The log id for std::exception.
   * @param validity_flag - The flag of fs_status_ to set if the directory is
   * valid.
   *
   * @see DataLog::FsStatus
   */
  void createFile(const std::string &new_file_path,
                  const data::LogId booster_seat_log_id,
                  const data::LogId std_except_log_id, bool &validity_flag);

  void createDataDir();
  void createLogDir();
  void createDataArchiveDir();
  void createLogArchiveDir();

  void createNewDataFile();
  void createNewLogFile();

  /**
   * @brief Validates the existence of a directory.
   *
   * @param dir_path
   * @param log_id
   * @param validity_flag
   */
  void validateDirExists(const std::string &path,
                         const data::LogId does_not_exist_log_id,
                         const data::LogId booster_seat_log_id,
                         const data::LogId std_except_log_id,
                         bool &validity_flag);

  /**
   * @brief Validates the existence of a file.
   *
   * @param file_path
   * @param log_id
   * @param file_size
   * @param validity_flag
   */
  void validateFileExists(const std::string &path,
                          const data::LogId does_not_exist_log_id,
                          const data::LogId booster_seat_log_id,
                          const data::LogId std_except_log_id,
                          bool &validity_flag);

  /**
   * @brief Attempts to append data to a file. Only checks the file validity.
   *
   * @param path - The path to the file.
   * @param data - The data to append.
   * @param error_id - The error id to log if an error occurs.
   */
  void appendToFile(const std::string &path, const std::string &data,
                    const data::LogId error_id);

  /**
   * @brief Checks the size of a file.
   *
   * @param file_path - The path to the file.
   * @param file_size - The variable to store the file size in.
   */
  void updateFileSize(const std::string &file_path, const data::LogId error_id,
                      data::blocks::DataLogStats::FileSizeType &file_size);

  /**
   * @brief Checks the size of a directory. (Recursive)
   *
   * @param dir_path - The path to the directory.
   * @param dir_size - The variable to store the directory size in.
   */
  void updateDirSize(const std::string &dir_path, const data::LogId error_id,
                     data::blocks::DataLogStats::FileSizeType &dir_size);

  /**
   * @brief Moves a file to the archive directory according to the archive
   * method.
   *
   * @param file_path
   * @param archive_dir_path
   * @param error_id
   * @return true if the file was archived successfully, false otherwise.
   *
   * @todo Archive Methods
   */
  bool archiveFile(const std::string &file_path,
                   const std::string &archive_dir_path,
                   const data::LogId error_id);

  /**
   * @brief Called on startup. If there are any files in the data or log
   * directories that have been left behind, they will be archived.
   * @param dir_path
   * @param archive_dir_path
   * @param current_file_name
   * @param error_id
   */
  void archiveOtherFilesInDir(const std::string &dir_path,
                              const std::string &archive_dir_path,
                              data::LogId error_id);

  /**
   * @brief This method will check to see if a data or log file needs to be
   * rotated/archived.
   *
   * @details This method will archive the file and create a new one if it
   * exceeds the configured file size limit.
   */
  void rotateFiles();

  /**
   * @brief This method will use the configured archive settings to trim the
   * oldest files in the archive directory if the archive size limit is
   * exceeded.
   */
  void trimArchive();

  /** @} */ // end of DataLog.Filesystem
  //// END //// File System Management Methods

  // START // Private Data Members
  data::SharedData &shared_data_;
  cfg::Configuration &config_;

  data::blocks::DataLogStats fs_status_ = {};

  BoosterSeat::Stopwatch validation_stopwatch_ = BoosterSeat::Stopwatch();
  BoosterSeat::Stopwatch data_frame_stopwatch_ = BoosterSeat::Stopwatch();

  DataFormatter formatter_;
};

}; // namespace data_middleware

#endif /* DATALOG_HPP_ */