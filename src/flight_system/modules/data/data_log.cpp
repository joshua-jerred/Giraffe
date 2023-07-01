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

#include "data_log.hpp"
#include <BoosterSeat/exception.hpp>
#include <BoosterSeat/filesystem.hpp>
#include <BoosterSeat/time.hpp>
#include <filesystem>

namespace mw = data_middleware;
namespace bs = BoosterSeat;
namespace bsfs = BoosterSeat::filesystem;
namespace bst = BoosterSeat::time;

// For Logging
inline constexpr node::Identification kNodeId =
    node::Identification::DATA_MODULE;

// Directory paths
inline const std::string kDataDirPath = "./data";
inline const std::string kDataArchiveDirPath = kDataDirPath + "/archive";
inline const std::string kLogDirPath = "./log";
inline const std::string kLogArchiveDirPath = kLogDirPath + "/archive";

// File Names
inline constexpr bst::TimeZone kDataTimeZone = bst::TimeZone::UTC;
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
    archiveOtherFilesInDir(kDataDirPath, kDataArchiveDirPath,
                           data::LogId::DATA_LOG_archiveOldDataFiles);
  }
  if (fs_status_.log_dir) {
    archiveOtherFilesInDir(kLogDirPath, kLogArchiveDirPath,
                           data::LogId::DATA_LOG_archiveOldLogFiles);
  }

  // Create the data and log files.
  createNewDataFile();
  createNewLogFile();
  updateFileList();

  // Update the file system status.
  shared_data_.blocks.data_log_stats.set(fs_status_);
};

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
  int time_since_last_log_ms =
      data_frame_stopwatch_.elapsed(BoosterSeat::Resolution::MILLISECONDS);

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
        kNodeId, data::LogId::DATA_LOG_invalidDataframeStrategy);
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
                                   data::LogId::GENERIC_notYetImplemented,
                                   "DataLog::logDataFrame SELECTION_INTERVAL");
  }

  updateFileSystem();
}

void mw::DataLog::logErrorFrame() {
  int log_interval_ms = config_.data_module_log.getErrorFrameLogInterval();
  int time_since_last_log_ms =
      error_frame_stopwatch_.elapsed(BoosterSeat::Resolution::MILLISECONDS);

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
  auto log_level = config_.data_module_log.getLogLevel();
  auto packet_level = packet.level;

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
               data::LogId::DATA_LOG_appendToDataFile);
}

void mw::DataLog::appendToLogFile(const std::string &content) {
  // There should be plenty of error coverage everywhere else,
  // so we can just return if the file path is not valid.
  if (!fs_status_.log_file) {
    return;
  }

  appendToFile(fs_status_.log_file_path, content,
               data::LogId::DATA_LOG_appendToLogFile);
}

void mw::DataLog::updateFileSystem() {
  long ms_since_last_validation = static_cast<int>(
      validation_stopwatch_.elapsed(BoosterSeat::Resolution::MILLISECONDS));

  if (ms_since_last_validation >
      config_.data_module_data.getFileSystemCheckInterval()) {
    validation_stopwatch_.reset();
    validation_stopwatch_.start();
  } else {
    return; // don't check the filesystem if the interval hasn't passed
  }
  validateFileSystem();
  rotateFiles();
  shared_data_.blocks.data_log_stats.set(fs_status_);
}

void mw::DataLog::validateFileSystem() {
  // check state of the filesystem and attempt to fix it if necessary
  validateDirExists(kDataDirPath, data::LogId::DATA_LOG_dataDirDoesNotExist,
                    data::LogId::DATA_LOG_dataDirExistenceBstrst,
                    data::LogId::DATA_LOG_dataDirExistenceStdexcept,
                    fs_status_.data_dir);
  validateDirExists(kDataArchiveDirPath,
                    data::LogId::DATA_LOG_dataArchiveDirDoesNotExist,
                    data::LogId::DATA_LOG_dataArchiveDirExistenceBstrst,
                    data::LogId::DATA_LOG_dataArchiveDirExistenceStdexcept,
                    fs_status_.data_archive_dir);
  validateDirExists(kLogDirPath, data::LogId::DATA_LOG_logDirDoesNotExist,
                    data::LogId::DATA_LOG_logDirExistenceBstrst,
                    data::LogId::DATA_LOG_logDirExistenceStdexcept,
                    fs_status_.log_dir);
  validateDirExists(kLogArchiveDirPath,
                    data::LogId::DATA_LOG_logArchiveDirDoesNotExist,
                    data::LogId::DATA_LOG_logArchiveDirExistenceBstrst,
                    data::LogId::DATA_LOG_logArchiveDirExistenceStdexcept,
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
                   data::LogId::DATA_LOG_dataFileSizeRead,
                   fs_status_.data_file_size);
  } else {
    fs_status_.data_file_size = 0;
  }

  if (fs_status_.log_dir && fs_status_.log_file) {
    updateFileSize(fs_status_.log_file_path,
                   data::LogId::DATA_LOG_logFileSizeRead,
                   fs_status_.log_file_size);
  } else {
    fs_status_.log_file_size = 0;
  }

  // Update Directory sizes
  if (fs_status_.data_archive_dir) {
    updateDirSize(kDataArchiveDirPath,
                  data::LogId::DATA_LOG_dataArchiveDirSizeRead,
                  fs_status_.data_archive_dir_size);
  } else {
    fs_status_.data_archive_dir_size = 0;
  }

  if (fs_status_.log_archive_dir) {
    updateDirSize(kLogArchiveDirPath,
                  data::LogId::DATA_LOG_logArchiveDirSizeRead,
                  fs_status_.log_archive_dir_size);
  } else {
    fs_status_.log_archive_dir_size = 0;
  }
}

void mw::DataLog::createDirectory(const std::string &path,
                                  const data::LogId booster_seat_log_id,
                                  const data::LogId std_except_log_id,
                                  bool &validity_flag) {
  try {
    // Verify that the directory does not exist before creating it.
    if (!bsfs::doesDirectoryExist(path)) {
      bsfs::createDirectory(path);
      validity_flag = true;
      shared_data_.debugLog(kNodeId, "Dir: " + path + " created.");
    } else {
      validity_flag = true; // Directory already exists.
      shared_data_.debugLog(kNodeId, "Dir: " + path + " already exists.");
    }
  } catch (const bs::BoosterSeatException &e) {
    validity_flag = false;
    shared_data_.streams.log.errorBoosterSeatException(kNodeId,
                                                       booster_seat_log_id, e);
  } catch (const std::exception &e) {
    validity_flag = false;
    shared_data_.streams.log.errorStdException(kNodeId, std_except_log_id, e);
  }
}

void mw::DataLog::createFile(const std::string &new_file_path,
                             const data::LogId booster_seat_log_id,
                             const data::LogId std_except_log_id,
                             bool &validity_flag) {
  try {
    bsfs::createFile(new_file_path);
    validity_flag = true;
    shared_data_.debugLog(kNodeId, "File: " + new_file_path + " created.");
  } catch (const bs::BoosterSeatException &e) {
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
                                   data::LogId::DATA_LOG_dataDirCreateFailed);
    return;
  }

  createDirectory(kDataDirPath, data::LogId::DATA_LOG_createDataDirBstrst,
                  data::LogId::DATA_LOG_createDataDirStdexcept,
                  fs_status_.data_dir);
}

void mw::DataLog::createDataArchiveDir() {
  static int num_attempts = 0;
  num_attempts++;

  if (num_attempts > kMaxDirCreateAttempts) {
    return;
  } else if (num_attempts == kMaxDirCreateAttempts) {
    shared_data_.streams.log.error(kNodeId,
                                   data::LogId::DATA_LOG_dataDirCreateFailed);
    return;
  }

  createDirectory(kDataArchiveDirPath,
                  data::LogId::DATA_LOG_createDataArchiveDirBstrst,
                  data::LogId::DATA_LOG_createDataArchiveDirStdexcept,
                  fs_status_.data_archive_dir);
}

void mw::DataLog::createLogDir() {
  static int num_attempts = 0;
  num_attempts++;

  if (num_attempts > kMaxDirCreateAttempts) {
    return;
  } else if (num_attempts == kMaxDirCreateAttempts) {
    shared_data_.streams.log.error(kNodeId,
                                   data::LogId::DATA_LOG_dataDirCreateFailed);
    return;
  }

  createDirectory(kLogDirPath, data::LogId::DATA_LOG_createLogDirBstrst,
                  data::LogId::DATA_LOG_createLogDirStdexcept,
                  fs_status_.log_dir);
}

void mw::DataLog::createLogArchiveDir() {
  static int num_attempts = 0;
  num_attempts++;

  if (num_attempts > kMaxDirCreateAttempts) {
    return;
  } else if (num_attempts == kMaxDirCreateAttempts) {
    shared_data_.streams.log.error(kNodeId,
                                   data::LogId::DATA_LOG_dataDirCreateFailed);
    return;
  }

  createDirectory(kLogArchiveDirPath,
                  data::LogId::DATA_LOG_createLogArchiveDirBstrst,
                  data::LogId::DATA_LOG_createLogArchiveDirStdexcept,
                  fs_status_.log_archive_dir);
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
         bst::dateAndTimeString(kDataTimeZone, '-', '_', ':') + kFileExtension;
}

void mw::DataLog::createNewDataFile() {
  std::string new_file_path = generateFilePath(kDataDirPath, kDataFilePrefix);

  createFile(new_file_path, data::LogId::DATA_LOG_createNewDataFileBstrst,
             data::LogId::DATA_LOG_createNewDataFileStdexcept,
             fs_status_.data_file);

  if (fs_status_.data_file) {
    fs_status_.data_file_path = new_file_path;
  }
}

void mw::DataLog::createNewLogFile() {
  std::string new_file_path = generateFilePath(kLogDirPath, kLogFilePrefix);
  createFile(new_file_path, data::LogId::DATA_LOG_createNewLogFileBstrst,
             data::LogId::DATA_LOG_createNewLogFileStdexcept,
             fs_status_.log_file);

  if (fs_status_.log_file) {
    fs_status_.log_file_path = new_file_path;
  }
}

void mw::DataLog::validateDirExists(const std::string &path,
                                    const data::LogId does_not_exist_log_id,
                                    const data::LogId booster_seat_log_id,
                                    const data::LogId std_except_log_id,
                                    bool &validity_flag) {
  try {
    if (bsfs::doesDirectoryExist(path)) {
      validity_flag = true;
    } else {
      validity_flag = false;
      shared_data_.streams.log.error(kNodeId, does_not_exist_log_id);
    }
  } catch (const bs::BoosterSeatException &e) {
    validity_flag = false;
    shared_data_.streams.log.errorBoosterSeatException(kNodeId,
                                                       booster_seat_log_id, e);
  } catch (const std::exception &e) {
    validity_flag = false;
    shared_data_.streams.log.errorStdException(kNodeId, std_except_log_id, e);
  }
}

void mw::DataLog::validateFileExists(const std::string &path,
                                     const data::LogId does_not_exist_log_id,
                                     const data::LogId booster_seat_log_id,
                                     const data::LogId std_except_log_id,
                                     bool &validity_flag) {
  try {
    if (bsfs::doesFileExist(path)) {
      validity_flag = true;
    } else {
      validity_flag = false;
      shared_data_.streams.log.error(kNodeId, does_not_exist_log_id);
    }
  } catch (const bs::BoosterSeatException &e) {
    validity_flag = false;
    shared_data_.streams.log.errorBoosterSeatException(kNodeId,
                                                       booster_seat_log_id, e);
  } catch (const std::exception &e) {
    validity_flag = false;
    shared_data_.streams.log.errorStdException(kNodeId, std_except_log_id, e);
  }
}

void mw::DataLog::appendToFile(const std::string &path, const std::string &data,
                               const data::LogId error_id) {
  try {
    bsfs::appendToFile(path, data);
  } catch (const bs::BoosterSeatException &e) {
    shared_data_.streams.log.errorBoosterSeatException(kNodeId, error_id, e);
  } catch (const std::exception &e) {
    shared_data_.streams.log.errorStdException(kNodeId, error_id, e);
  }
}

void mw::DataLog::updateFileSize(
    const std::string &file_path, const data::LogId error_id,
    data::blocks::DataLogStats::FileSizeType &file_size) {
  try {
    file_size = bsfs::getFileSize(file_path, kDataSizeUnit);
  } catch (const bs::BoosterSeatException &e) {
    shared_data_.streams.log.errorBoosterSeatException(kNodeId, error_id, e);
  } catch (const std::exception &e) {
    shared_data_.streams.log.errorStdException(kNodeId, error_id, e);
  }
}

void mw::DataLog::updateDirSize(
    const std::string &dir_path, const data::LogId error_id,
    data::blocks::DataLogStats::FileSizeType &dir_size) {
  try {
    dir_size = bsfs::getDirectorySize(dir_path, kDataSizeUnit);
  } catch (const bs::BoosterSeatException &e) {
    shared_data_.streams.log.errorBoosterSeatException(kNodeId, error_id, e);
  } catch (const std::exception &e) {
    shared_data_.streams.log.errorStdException(kNodeId, error_id, e);
  }
}

bool mw::DataLog::archiveFile(const std::string &file_path,
                              const std::string &archive_dir_path,
                              const data::LogId error_id) {
  try {
    if (!bsfs::doesFileExist(file_path) ||
        !bsfs::doesDirectoryExist(archive_dir_path)) {
      shared_data_.streams.log.error(kNodeId, error_id);
      return false;
    }
    std::string file_name = bsfs::getFileName(file_path);
    std::string new_file_path = archive_dir_path + "/" + file_name;
    bsfs::moveFile(file_path, new_file_path);
  } catch (const bs::BoosterSeatException &e) {
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
                                         data::LogId error_id) {
  try {
    std::filesystem::path dir(dir_path);
    for (const auto &file : std::filesystem::directory_iterator(dir)) {
      std::string file_path = file.path().string();
      archiveFile(file_path, archive_dir_path, error_id);
    }
  } catch (const std::exception &e) {
    shared_data_.streams.log.errorStdException(kNodeId, error_id, e);
  }
}

void mw::DataLog::rotateFiles() {
  int max_data_size = config_.data_module_data.getMaxDataLogFileSizeMb();
  int max_log_size = config_.data_module_log.getMaxLogFileSizeMb();

  if (fs_status_.data_file && fs_status_.data_file_size >= max_data_size) {
    if (!archiveFile(fs_status_.data_file_path, kDataArchiveDirPath,
                     data::LogId::DATA_LOG_archiveDataFile)) {
      shared_data_.streams.log.error(kNodeId,
                                     data::LogId::DATA_LOG_rotateDataFile);
      return;
    }
    createNewDataFile();
  }

  if (fs_status_.log_file && fs_status_.log_file_size >= max_log_size) {
    if (!archiveFile(fs_status_.log_file_path, kLogArchiveDirPath,
                     data::LogId::DATA_LOG_archiveLogFile)) {
      shared_data_.streams.log.error(kNodeId,
                                     data::LogId::DATA_LOG_rotateLogFile);
      return;
    }
    createNewLogFile();
  }

  updateFileList();
}

/**
 * @todo Implement this function
 */
void mw::DataLog::trimArchive() {
}

void getFileNamesInDir(const std::string &dir_path,
                       std::vector<std::string> &file_list) {
  file_list.clear();
  std::filesystem::path dir(dir_path);
  for (const auto &file : std::filesystem::directory_iterator(dir)) {
    std::string file_path = file.path().filename().string();
    file_list.push_back(file_path);
  }
}

void mw::DataLog::updateFileList() {
  try {
    getFileNamesInDir(kDataArchiveDirPath, fs_status_.archived_data_files_list);
    getFileNamesInDir(kLogArchiveDirPath, fs_status_.archived_log_files_list);
  } catch (const std::exception &e) {
    shared_data_.streams.log.errorStdException(
        kNodeId, data::LogId::DATA_LOG_fileListFail, e);
  }
}