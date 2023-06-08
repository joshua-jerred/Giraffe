#include "data_log.hpp"
#include <BoosterSeat/exception.hpp>
#include <BoosterSeat/filesystem.hpp>
#include <BoosterSeat/time.hpp>
#include <nlohmann/json.hpp>

#include <iostream>

typedef nlohmann::ordered_json json;
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
inline constexpr bst::TimeZone kFileNameTimeZone = bst::TimeZone::UTC;
inline const std::string kDataFilePrefix = "data_";
inline const std::string kLogFilePrefix = "log_";
inline const std::string kFileExtension = ".json";

// File Creation
inline constexpr int kMaxDirCreateAttempts = 2;

mw::DataLog::DataLog(data::SharedData &shared_data, cfg::Configuration &config)
    : shared_data_(shared_data), config_(config) {
  data_frame_stopwatch_.start();
  validation_stopwatch_.start();

  // Generate the file system.
  createDataDir();
  createLogDir();
  createDataArchiveDir();
  createLogArchiveDir();

  // Create the data and log files.
  createNewDataFile();
  createNewLogFile();

  shared_data_.blocks.data_log_stats.set(fs_status_);
};

void mw::DataLog::logDataPacket(const data::DataPacket &packet) {
  updateFileSystem();
  // ignore status packets, there are a lot of them.
  if (packet.type == data::DataPacket::Type::GENERIC) {
    std::cout << "DataLog::logDataPacket" << std::endl;
    std::cout << node::identification_to_string.at(packet.source) << std::endl;
    std::cout << static_cast<int>(packet.identifier) << std::endl;
    std::cout << packet.value << std::endl;
  }
}

void mw::DataLog::logDataFrame() {
  int log_interval_ms = config_.data_module_data.getLogIntervalMs();
  int time_since_last_log_ms =
      data_frame_stopwatch_.elapsed(BoosterSeat::Resolution::MILLISECONDS);

  if (time_since_last_log_ms > log_interval_ms) {
    data_frame_stopwatch_.reset();
    data_frame_stopwatch_.start();
  } else {
    return;
  }
}

void mw::DataLog::logLogPacket(const data::LogPacket &packet) {
  (void)packet;
}

void mw::DataLog::updateFileSystem() {
  long ms_since_last_validation = static_cast<int>(
      validation_stopwatch_.elapsed(BoosterSeat::Resolution::MILLISECONDS));

  // Only check the filesystem every at a specified interval.
  if (ms_since_last_validation >
      config_.data_module_data.getFileSystemCheckInterval()) {
    validation_stopwatch_.reset();
    validation_stopwatch_.start();
  } else {
    return;
  }

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
  } else if (!fs_status_.data_archive_dir) {
    createDataArchiveDir();
  } else if (!fs_status_.log_dir) {
    createLogDir();
  } else if (!fs_status_.log_archive_dir) {
    createLogArchiveDir();
  }

  // Now do the same for the files. Check if they're valid first, then create
  // them if they don't exist.
  if (fs_status_.data_dir && !fs_status_.data_file) {
    createNewDataFile();
  } else if (fs_status_.log_dir && !fs_status_.log_file) {
    createNewLogFile();
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
 * @brief Generates a file name with a prefix followed by the current date
 * and time.
 * @param prefix - The prefix to use for the file name.
 * @return std::string - The generated file name.
 */
inline std::string generateFilePath(const std::string path,
                                    const std::string &file_prefix) {
  return path + "/" + file_prefix + "_" +
         bst::dateAndTimeString(kFileNameTimeZone, '-', '_', ':') +
         kFileExtension;
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