#ifndef DATA_LOG_HPP_
#define DATA_LOG_HPP_

#include "configuration.hpp"
#include "shared_data.hpp"
#include <BoosterSeat/stopwatch.hpp>

namespace data_middleware {

class DataLog {
  /**
   * @brief Used to store the validity status of the data log files and paths.
   * @details All "sizes" are in MB.
   */

public:
  /**
   * @brief Construct a new DataLog::DataLog object
   *
   * @param shared_data @see data::SharedData
   * @param config @see cfg::Configuration
   */
  DataLog(data::SharedData &shared_data, cfg::Configuration &config);
  ~DataLog() = default;

  /**
   * @brief Used to add a single data packet to the file log.
   * @param packet @see data::DataPacket
   */
  void logDataPacket(const data::DataPacket &packet);

  /**
   * @brief Used for periodic logging of all data.
   */
  void logDataFrame();

  /**
   * @brief Used to log a single log packet to the file log.
   * @param packet @see data::LogPacket
   */
  void logLogPacket(const data::LogPacket &packet);

private:
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

  /** @} */ // end of DataLog.Filesystem
  //// END //// File System Management Methods

  // START // Private Data Members
  data::blocks::DataLogStats fs_status_ = {};

  BoosterSeat::Stopwatch validation_stopwatch_ = BoosterSeat::Stopwatch();
  BoosterSeat::Stopwatch data_frame_stopwatch_ = BoosterSeat::Stopwatch();

  data::SharedData &shared_data_;
  cfg::Configuration &config_;
};

}; // namespace data_middleware

#endif /* DATALOG_HPP_ */