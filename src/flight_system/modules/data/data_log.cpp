/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   data_log.cpp
 * @brief  The implementation of the data log class.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-06-30
 * @copyright  2023 (license to be defined)
 */

#include <filesystem>

#include <BoosterSeat/exception.hpp>
#include <BoosterSeat/filesystem.hpp>
#include <BoosterSeat/time.hpp>
#include <bzip2-cpp-lib.hpp>

#include "data_log.hpp"
#include "giraffe_file_paths.hpp"

namespace mw = data_middleware;
namespace bsfs = bst::filesystem;

// For Logging
inline constexpr node::Identification kNodeId =
    node::Identification::DATA_MODULE;

// File Names
inline constexpr bst::time::TimeZone kDataTimeZone = bst::time::TimeZone::UTC;
inline const std::string kDataFilePrefix = "data_";
inline const std::string kLogFilePrefix = "log_";
inline const std::string kFileExtension = ".json";

// File Creation
inline constexpr int kMaxDirCreateAttempts = 2;

// File System
inline constexpr bsfs::units::Size kDataSizeUnit = bsfs::units::Size::MEGABYTES;

mw::DataLog::DataLog(data::SharedData &shared_data, cfg::Configuration &config)
    : shared_data_(shared_data), config_(config),
      formatter_(config, shared_data) {
  validation_stopwatch_.start();
  data_frame_stopwatch_.start();
  error_frame_stopwatch_.start();

  // Generate the file system.
  createDataDir();
  createLogDir();
  createDataArchiveDir();
  createLogArchiveDir();

  // Clear up the old files if they exist, before creating new ones.
  if (fs_status_.data_dir) {
    archiveOtherFilesInDir(data_dir_path_, data_archive_dir_path_,
                           DiagnosticId::DATA_LOG_archiveOldDataFiles);
  }
  if (fs_status_.log_dir) {
    archiveOtherFilesInDir(log_dir_path_, log_archive_dir_path_,
                           DiagnosticId::DATA_LOG_archiveOldLogFiles);
  }

  // Create the data and log files.
  createNewDataFile();
  createNewLogFile();
  updateFileList();

  // Update the file system status.
  shared_data_.blocks.data_log_stats.set(fs_status_);
}

void mw::DataLog::logDataPacket(const data::DataPacket &packet) {
  updateFileSystem();
  // ignore status packets, there are a lot of them.
  if (packet.type == data::DataPacket::Type::STATUS) {
    return;
  }
  // Format the packet into json and append it to the data file.
  appendToDataFile(formatter_.dataPacketToJsonString(packet));
}

void mw::DataLog::logDataFrame(cfg::gEnum::LogStrategy strategy) {
  int log_interval_ms = config_.data_module_data.getLogIntervalMs();
  int time_since_last_log_ms = data_frame_stopwatch_.elapsedMillisecondsF();

  if (time_since_last_log_ms > log_interval_ms) {
    // first, reset the stopwatch before logging
    data_frame_stopwatch_.reset();
    data_frame_stopwatch_.start();
  } else {
    return; // don't log if the interval hasn't passed
  }

  // Verify that the strategy is valid. If it's not, revert to interval.
  if (strategy != cfg::gEnum::LogStrategy::INTERVAL &&
      strategy != cfg::gEnum::LogStrategy::SELECTION_INTERVAL) {
    shared_data_.streams.log.error(
        kNodeId, DiagnosticId::DATA_LOG_invalidDataframeStrategy);
    strategy = cfg::gEnum::LogStrategy::INTERVAL;
  }

  if (strategy == cfg::gEnum::LogStrategy::INTERVAL) {
    // log all data
    appendToDataFile(formatter_.fullFrame());
  } else if (strategy == cfg::gEnum::LogStrategy::SELECTION_INTERVAL) {
    // log selected data
    /**
     * @todo Implement this. Requires configuration list loading.
     */
    shared_data_.streams.log.error(kNodeId,
                                   DiagnosticId::GENERIC_notImplemented,
                                   "DataLog::logDataFrame SELECTION_INTERVAL");
  }

  updateFileSystem();
}

void mw::DataLog::logErrorFrame() {
  int log_interval_ms = config_.data_module_log.getErrorFrameLogInterval();
  int time_since_last_log_ms = error_frame_stopwatch_.elapsedMillisecondsF();

  if (time_since_last_log_ms > log_interval_ms) {
    // first, reset the stopwatch before logging
    error_frame_stopwatch_.reset();
    error_frame_stopwatch_.start();
  } else {
    return; // don't log if the interval hasn't passed
  }

  appendToLogFile(formatter_.fullErrorFrame());

  updateFileSystem();
}

void mw::DataLog::logLogPacket(const data::LogPacket &packet) {
  const auto log_level = config_.data_module_log.getLogLevel();
  const auto packet_level = packet.level;

  if (static_cast<int>(packet_level) >= static_cast<int>(log_level)) {
    appendToLogFile(formatter_.logPacketToJsonString(packet));
  }
}

void mw::DataLog::appendToDataFile(const std::string &content) {
  // There should be plenty of error coverage everywhere else,
  // so we can just return if the file path is not valid.
  if (!fs_status_.data_file) {
    return;
  }

  appendToFile(fs_status_.data_file_path, content,
               DiagnosticId::DATA_LOG_appendToDataFile);
}

void mw::DataLog::appendToLogFile(const std::string &content) {
  // There should be plenty of error coverage everywhere else,
  // so we can just return if the file path is not valid.
  if (!fs_status_.log_file) {
    return;
  }

  appendToFile(fs_status_.log_file_path, content,
               DiagnosticId::DATA_LOG_appendToLogFile);
}

void mw::DataLog::updateFileSystem() {
  long ms_since_last_validation =
      static_cast<int>(validation_stopwatch_.elapsedMillisecondsF());

  if (ms_since_last_validation >
      config_.data_module_data.getFileSystemCheckInterval()) {
    validation_stopwatch_.reset();
    validation_stopwatch_.start();
  } else {
    return; // don't check the filesystem if the interval hasn't passed
  }
  validateFileSystem();
  rotateFiles();

  if (maximum_file_system_update_timer_.isDone()) {
    updateFileList();
    maximum_file_system_update_timer_.reset();
  }

  shared_data_.blocks.data_log_stats.set(fs_status_);
}

void mw::DataLog::validateFileSystem() {
  // check state of the filesystem and attempt to fix it if necessary
  validateDirExists(data_dir_path_, DiagnosticId::DATA_LOG_dataDirDoesNotExist,
                    DiagnosticId::DATA_LOG_dataDirExistenceBstrst,
                    DiagnosticId::DATA_LOG_dataDirExistenceStdexcept,
                    fs_status_.data_dir);
  validateDirExists(data_archive_dir_path_,
                    DiagnosticId::DATA_LOG_dataArchiveDirDoesNotExist,
                    DiagnosticId::DATA_LOG_dataArchiveDirExistenceBstrst,
                    DiagnosticId::DATA_LOG_dataArchiveDirExistenceStdexcept,
                    fs_status_.data_archive_dir);
  validateDirExists(log_dir_path_, DiagnosticId::DATA_LOG_logDirDoesNotExist,
                    DiagnosticId::DATA_LOG_logDirExistenceBstrst,
                    DiagnosticId::DATA_LOG_logDirExistenceStdexcept,
                    fs_status_.log_dir);
  validateDirExists(log_archive_dir_path_,
                    DiagnosticId::DATA_LOG_logArchiveDirDoesNotExist,
                    DiagnosticId::DATA_LOG_logArchiveDirExistenceBstrst,
                    DiagnosticId::DATA_LOG_logArchiveDirExistenceStdexcept,
                    fs_status_.log_archive_dir);

  // Create the directories if they do not exist.
  if (!fs_status_.data_dir) {
    createDataDir();
  }
  if (!fs_status_.data_archive_dir) {
    createDataArchiveDir();
  }
  if (!fs_status_.log_dir) {
    createLogDir();
  }
  if (!fs_status_.log_archive_dir) {
    createLogArchiveDir();
  }

  // Now do the same for the files. Check if they're valid first, then create
  // them if they don't exist.
  if (fs_status_.data_dir && !fs_status_.data_file) {
    createNewDataFile();
  }
  if (fs_status_.log_dir && !fs_status_.log_file) {
    createNewLogFile();
  }

  // Update File sizes
  if (fs_status_.data_dir && fs_status_.data_file) {
    updateFileSize(fs_status_.data_file_path,
                   DiagnosticId::DATA_LOG_dataFileSizeRead,
                   fs_status_.data_file_size);
  } else {
    fs_status_.data_file_size = 0;
  }

  if (fs_status_.log_dir && fs_status_.log_file) {
    updateFileSize(fs_status_.log_file_path,
                   DiagnosticId::DATA_LOG_logFileSizeRead,
                   fs_status_.log_file_size);
  } else {
    fs_status_.log_file_size = 0;
  }

  // Update Directory sizes
  if (fs_status_.data_archive_dir) {
    updateDirSize(data_archive_dir_path_,
                  DiagnosticId::DATA_LOG_dataArchiveDirSizeRead,
                  fs_status_.data_archive_dir_size);
  } else {
    fs_status_.data_archive_dir_size = 0;
  }

  if (fs_status_.log_archive_dir) {
    updateDirSize(log_archive_dir_path_,
                  DiagnosticId::DATA_LOG_logArchiveDirSizeRead,
                  fs_status_.log_archive_dir_size);
  } else {
    fs_status_.log_archive_dir_size = 0;
  }

  // trim the data archive directory if it's too big
  if (fs_status_.data_archive_dir_size >
      config_.data_module_data.getMaxDataArchiveSizeMb()) {
    trimArchive();
  }
}

void mw::DataLog::createDirectory(const std::string &path,
                                  const DiagnosticId booster_seat_log_id,
                                  const DiagnosticId std_except_log_id,
                                  bool &validity_flag) {
  try {
    // Verify that the directory does not exist before creating it.
    if (!bsfs::doesDirectoryExist(path)) {
      bsfs::createDirectory(path);
      validity_flag = true;
      shared_data_.streams.log.debug(kNodeId, "Dir: " + path + " created.");
    } else {
      validity_flag = true; // Directory already exists.
      shared_data_.streams.log.debug(kNodeId,
                                     "Dir: " + path + " already exists.");
    }
  } catch (const bst::BoosterSeatException &e) {
    validity_flag = false;
    shared_data_.streams.log.errorBoosterSeatException(kNodeId,
                                                       booster_seat_log_id, e);
  } catch (const std::exception &e) {
    validity_flag = false;
    shared_data_.streams.log.errorStdException(kNodeId, std_except_log_id, e);
  }
}

void mw::DataLog::createFile(const std::string &new_file_path,
                             const DiagnosticId booster_seat_log_id,
                             const DiagnosticId std_except_log_id,
                             bool &validity_flag) {
  try {
    bsfs::createFile(new_file_path);
    validity_flag = true;
    shared_data_.streams.log.debug(kNodeId,
                                   "File: " + new_file_path + " created.");
  } catch (const bst::BoosterSeatException &e) {
    validity_flag = false;
    shared_data_.streams.log.errorBoosterSeatException(kNodeId,
                                                       booster_seat_log_id, e);
  } catch (const std::exception &e) {
    validity_flag = false;
    shared_data_.streams.log.errorStdException(kNodeId, std_except_log_id, e);
  }
}

void mw::DataLog::createDataDir() {
  static int num_attempts = 0;
  num_attempts++;

  // If the directory creation fails, report the error once.
  if (num_attempts > kMaxDirCreateAttempts) {
    return;
  } else if (num_attempts == kMaxDirCreateAttempts) {
    shared_data_.streams.log.error(kNodeId,
                                   DiagnosticId::DATA_LOG_dataDirCreateFailed);
    return;
  }

  data_dir_path_ = giraffe::file_paths::getGfsDataDirPath();

  if (bsfs::doesDirectoryExist(data_dir_path_)) {
    fs_status_.data_dir = true;
  }
}

void mw::DataLog::createDataArchiveDir() {
  static int num_attempts = 0;
  num_attempts++;

  if (num_attempts > kMaxDirCreateAttempts) {
    return;
  } else if (num_attempts == kMaxDirCreateAttempts) {
    shared_data_.streams.log.error(kNodeId,
                                   DiagnosticId::DATA_LOG_dataDirCreateFailed);
    return;
  }

  data_archive_dir_path_ = giraffe::file_paths::getGfsDataArchiveDirPath();

  if (bsfs::doesDirectoryExist(data_archive_dir_path_)) {
    fs_status_.data_archive_dir = true;
  }
}

void mw::DataLog::createLogDir() {
  static int num_attempts = 0;
  num_attempts++;

  if (num_attempts > kMaxDirCreateAttempts) {
    return;
  } else if (num_attempts == kMaxDirCreateAttempts) {
    shared_data_.streams.log.error(kNodeId,
                                   DiagnosticId::DATA_LOG_dataDirCreateFailed);
    return;
  }

  log_dir_path_ = giraffe::file_paths::getGfsLogDirPath();

  if (bsfs::doesDirectoryExist(log_dir_path_)) {
    fs_status_.log_dir = true;
  }
}

void mw::DataLog::createLogArchiveDir() {
  static int num_attempts = 0;
  num_attempts++;

  if (num_attempts > kMaxDirCreateAttempts) {
    return;
  } else if (num_attempts == kMaxDirCreateAttempts) {
    shared_data_.streams.log.error(kNodeId,
                                   DiagnosticId::DATA_LOG_dataDirCreateFailed);
    return;
  }

  log_archive_dir_path_ = giraffe::file_paths::getGfsLogArchiveDirPath();

  if (bsfs::doesDirectoryExist(log_archive_dir_path_)) {
    fs_status_.log_archive_dir = true;
  }
}

/**
 * @brief Generates a file path with the current date and time.
 *
 * @param path - The path to the directory where the file will be created.
 * @param file_prefix - The prefix before the date and time.
 * @return std::string - The generated file path.
 */
inline std::string generateFilePath(const std::string path,
                                    const std::string &file_prefix) {
  return path + "/" + file_prefix +
         bst::time::dateAndTimeString(kDataTimeZone, '-', '_', '.') +
         kFileExtension;
}

void mw::DataLog::createNewDataFile() {
  std::string new_file_path = generateFilePath(data_dir_path_, kDataFilePrefix);

  createFile(new_file_path, DiagnosticId::DATA_LOG_createNewDataFileBstrst,
             DiagnosticId::DATA_LOG_createNewDataFileStdexcept,
             fs_status_.data_file);

  if (fs_status_.data_file) {
    fs_status_.data_file_path = new_file_path;
  }
}

void mw::DataLog::createNewLogFile() {
  std::string new_file_path = generateFilePath(log_dir_path_, kLogFilePrefix);
  createFile(new_file_path, DiagnosticId::DATA_LOG_createNewLogFileBstrst,
             DiagnosticId::DATA_LOG_createNewLogFileStdexcept,
             fs_status_.log_file);

  if (fs_status_.log_file) {
    fs_status_.log_file_path = new_file_path;
  }
}

void mw::DataLog::validateDirExists(const std::string &path,
                                    const DiagnosticId does_not_exist_log_id,
                                    const DiagnosticId booster_seat_log_id,
                                    const DiagnosticId std_except_log_id,
                                    bool &validity_flag) {
  try {
    if (bsfs::doesDirectoryExist(path)) {
      validity_flag = true;
    } else {
      validity_flag = false;
      shared_data_.streams.log.error(kNodeId, does_not_exist_log_id);
    }
  } catch (const bst::BoosterSeatException &e) {
    validity_flag = false;
    shared_data_.streams.log.errorBoosterSeatException(kNodeId,
                                                       booster_seat_log_id, e);
  } catch (const std::exception &e) {
    validity_flag = false;
    shared_data_.streams.log.errorStdException(kNodeId, std_except_log_id, e);
  }
}

void mw::DataLog::validateFileExists(const std::string &path,
                                     const DiagnosticId does_not_exist_log_id,
                                     const DiagnosticId booster_seat_log_id,
                                     const DiagnosticId std_except_log_id,
                                     bool &validity_flag) {
  try {
    if (bsfs::doesFileExist(path)) {
      validity_flag = true;
    } else {
      validity_flag = false;
      shared_data_.streams.log.error(kNodeId, does_not_exist_log_id);
    }
  } catch (const bst::BoosterSeatException &e) {
    validity_flag = false;
    shared_data_.streams.log.errorBoosterSeatException(kNodeId,
                                                       booster_seat_log_id, e);
  } catch (const std::exception &e) {
    validity_flag = false;
    shared_data_.streams.log.errorStdException(kNodeId, std_except_log_id, e);
  }
}

void mw::DataLog::appendToFile(const std::string &path, const std::string &data,
                               const DiagnosticId error_id) {
  try {
    bsfs::appendToFile(path, data);
  } catch (const bst::BoosterSeatException &e) {
    shared_data_.streams.log.errorBoosterSeatException(kNodeId, error_id, e);
  } catch (const std::exception &e) {
    shared_data_.streams.log.errorStdException(kNodeId, error_id, e);
  }
}

void mw::DataLog::updateFileSize(
    const std::string &file_path, const DiagnosticId error_id,
    data::blocks::DataLogStats::FileSizeType &file_size) {
  try {
    file_size = bsfs::getFileSize(file_path, kDataSizeUnit);
  } catch (const bst::BoosterSeatException &e) {
    shared_data_.streams.log.errorBoosterSeatException(kNodeId, error_id, e);
  } catch (const std::exception &e) {
    shared_data_.streams.log.errorStdException(kNodeId, error_id, e);
  }
}

void mw::DataLog::updateDirSize(
    const std::string &dir_path, const DiagnosticId error_id,
    data::blocks::DataLogStats::FileSizeType &dir_size) {
  try {
    dir_size = bsfs::getDirectorySize(dir_path, kDataSizeUnit);
  } catch (const bst::BoosterSeatException &e) {
    shared_data_.streams.log.errorBoosterSeatException(kNodeId, error_id, e);
  } catch (const std::exception &e) {
    shared_data_.streams.log.errorStdException(kNodeId, error_id, e);
  }
}

bool mw::DataLog::archiveFile(const std::string &file_path,
                              const std::string &archive_dir_path,
                              const DiagnosticId error_id) {
  try {
    if (!bsfs::doesFileExist(file_path) ||
        !bsfs::doesDirectoryExist(archive_dir_path)) {
      shared_data_.streams.log.error(kNodeId, error_id);
      return false;
    }
    std::string file_name = bsfs::getFileName(file_path);
    std::string new_file_path = archive_dir_path + "/" + file_name;

    auto method = config_.data_module_data.getArchiveMethod();

    if (method == cfg::gEnum::ArchiveMethod::PLAIN_TEXT) {
      bsfs::moveFile(file_path, new_file_path);
    } else if (method == cfg::gEnum::ArchiveMethod::BZIP2) {
      auto res = bzip2::compress(file_path, new_file_path + ".bz2");
      if (res != bzip2::Result::SUCCESS) {
        shared_data_.streams.log.error(kNodeId, error_id);
        return false;
      }
      bsfs::deleteFile(file_path);
    }
  } catch (const bst::BoosterSeatException &e) {
    shared_data_.streams.log.errorBoosterSeatException(kNodeId, error_id, e);
    return false;
  } catch (const std::exception &e) {
    shared_data_.streams.log.errorStdException(kNodeId, error_id, e);
    return false;
  }
  return true;
}

void mw::DataLog::archiveOtherFilesInDir(const std::string &dir_path,
                                         const std::string &archive_dir_path,
                                         DiagnosticId error_id) {
  try {
    std::filesystem::path dir(dir_path);
    for (const auto &file : std::filesystem::directory_iterator(dir)) {
      std::string file_path = file.path().string();
      if (std::filesystem::is_regular_file(file_path)) {
        archiveFile(file_path, archive_dir_path, error_id);
      }
    }
  } catch (const std::exception &e) {
    shared_data_.streams.log.errorStdException(kNodeId, error_id, e);
  }
}

void mw::DataLog::rotateFiles() {
  int max_data_size = config_.data_module_data.getMaxDataLogFileSizeMb();
  int max_log_size = config_.data_module_log.getMaxLogFileSizeMb();

  if (fs_status_.data_file && fs_status_.data_file_size >= max_data_size) {
    if (!archiveFile(fs_status_.data_file_path, data_archive_dir_path_,
                     DiagnosticId::DATA_LOG_archiveDataFile)) {
      shared_data_.streams.log.error(kNodeId,
                                     DiagnosticId::DATA_LOG_rotateDataFile);
      return;
    }
    createNewDataFile();
  }

  if (fs_status_.log_file && fs_status_.log_file_size >= max_log_size) {
    if (!archiveFile(fs_status_.log_file_path, log_archive_dir_path_,
                     DiagnosticId::DATA_LOG_archiveLogFile)) {
      shared_data_.streams.log.error(kNodeId,
                                     DiagnosticId::DATA_LOG_rotateLogFile);
      return;
    }
    createNewLogFile();
  }

  updateFileList();
}

void mw::DataLog::trimArchive() {
  try {
    std::filesystem::path dir(data_archive_dir_path_);
    int num_files = 0;
    std::filesystem::path oldest_file_path;
    std::chrono::time_point<std::chrono::file_clock> oldest_file_time;

    // Find the oldest file
    for (const auto &file : std::filesystem::directory_iterator(dir)) {
      if (num_files == 0) {
        oldest_file_path = file.path();
        oldest_file_time = file.last_write_time();
      } else {
        if (file.last_write_time() < oldest_file_time) {
          oldest_file_path = file.path();
          oldest_file_time = file.last_write_time();
        }
      }
      num_files++;
    }
    if (num_files > 0) {
      bsfs::deleteFile(oldest_file_path.string());
      shared_data_.streams.log.error(kNodeId,
                                     DiagnosticId::DATA_LOG_dataArchiveTrimmed,
                                     oldest_file_path.string());
    }
  } catch (const std::exception &e) {
    shared_data_.streams.log.errorStdException(
        kNodeId, DiagnosticId::DATA_LOG_dataArchiveTrimFailed, e);
  }
}

void getFileNamesInDir(const std::string &dir_path,
                       std::vector<std::string> &file_list) {
  file_list.clear();
  std::filesystem::path dir(dir_path);

  constexpr size_t MAX_FILE_LIST_SIZE = 100;
  for (const auto &file : std::filesystem::directory_iterator(dir)) {
    std::string file_path = file.path().filename().string();
    file_list.push_back(file_path);
  }
}

void mw::DataLog::updateFileList() {
  try {
    getFileNamesInDir(data_archive_dir_path_,
                      fs_status_.archived_data_files_list);
    getFileNamesInDir(log_archive_dir_path_,
                      fs_status_.archived_log_files_list);
  } catch (const std::exception &e) {
    shared_data_.streams.log.errorStdException(
        kNodeId, DiagnosticId::DATA_LOG_fileListFail, e);
  }
}