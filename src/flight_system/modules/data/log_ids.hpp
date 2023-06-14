// * * * * GENERATED AUTOMATICALLY, DO NOT MANUALLY MODIFY * * * *

#ifndef LOG_IDS_HPP_
#define LOG_IDS_HPP_

namespace data {
enum class LogId {
  GENERIC_unknown,
  GENERIC_info,
  GENERIC_debug,
  GENERIC_notYetImplemented, // Not yet implemented
  CONFIG_sectionNotFound,
  CONFIG_settingNotFound,
  CONFIG_invalidJsonType,
  CONFIG_invalidSettingValue,
  CONFIG_enumNotString,
  CONFIG_keyNotFoundInEnum,
  CONFIG_failedToSaveToPath,
  CONFIG_failedToOpenConfig,
  CONFIG_configFileDoesNotExist,
  CONFIG_failedToLoadSectionNotFound,
  CONFIG_extensionsNotArray, // Extensions in config file is not an array
  CONFIG_extensionNotObject, // Extension is not an object
  CONFIG_extensionNameAlreadyExists, // Extension Name Already Exists
  CONFIG_extensionNameDoesNotExist, // Extension Name Does Not Exist
  MODULE_startFailure,
  MODULE_moduleAlreadyStarted,
  DATA_MODULE_statusDataPacketUnknownSource,
  DATA_MODULE_dataPacketUnknownType,
  DATA_LOG_invalidDataframeStrategy, // Invalid data log strategy
  DATA_LOG_archiveOldDataFiles, // Failed to archive old data files
  DATA_LOG_archiveOldLogFiles, // Failed to archive old log files
  DATA_LOG_createDataDirBstrst, // Failed to create data directory at startup (BoosterSeat errno)
  DATA_LOG_createDataDirStdexcept, // Failed to create data directory at startup (std::exception)
  DATA_LOG_dataDirDoesNotExist, // Data directory does not exist - after startup
  DATA_LOG_dataDirExistenceBstrst, // Failed to check data directory existence (BoosterSeat errno)
  DATA_LOG_dataDirExistenceStdexcept, // Failed to check data directory existence (std::exception)
  DATA_LOG_dataDirCreateFailed, // Failed after reaching max attempts to create data directory
  DATA_LOG_createDataArchiveDirBstrst, // Failed to create data archive directory at startup (BoosterSeat errno)
  DATA_LOG_createDataArchiveDirStdexcept, // Failed to create data archive directory at startup (std::exception)
  DATA_LOG_dataArchiveDirDoesNotExist, // Data archive directory does not exist - after startup
  DATA_LOG_dataArchiveDirExistenceBstrst, // Failed to check data archive directory existence (BoosterSeat errno)
  DATA_LOG_dataArchiveDirExistenceStdexcept, // Failed to check data archive directory existence (std::exception)
  DATA_LOG_dataArchiveDirCreateFailed, // Failed after reaching max attempts to create data archive directory
  DATA_LOG_createLogDirBstrst, // Failed to create log directory at startup (BoosterSeat errno)
  DATA_LOG_createLogDirStdexcept, // Failed to create log directory at startup (std::exception)
  DATA_LOG_logDirDoesNotExist, // Data directory does not exist - after startup
  DATA_LOG_logDirExistenceBstrst, // Failed to check log directory existence (BoosterSeat errno)
  DATA_LOG_logDirExistenceStdexcept, // Failed to check log directory existence (std::exception)
  DATA_LOG_logDirCreateFailed, // Failed after reaching max attempts to create log directory
  DATA_LOG_createLogArchiveDirBstrst, // Failed to create log archive directory at startup (BoosterSeat errno)
  DATA_LOG_createLogArchiveDirStdexcept, // Failed to create log archive directory at startup (std::exception)
  DATA_LOG_logArchiveDirDoesNotExist, // Data archive directory does not exist - after startup
  DATA_LOG_logArchiveDirExistenceBstrst, // Failed to check log directory existence (BoosterSeat errno)
  DATA_LOG_logArchiveDirExistenceStdexcept, // Failed to check log archive directory existence (std::exception)
  DATA_LOG_logArchiveDirCreateFailed, // Failed after reaching max attempts to create log directory
  DATA_LOG_createNewDataFileBstrst, // Failed to create new data file (BoosterSeat errno)
  DATA_LOG_createNewDataFileStdexcept, // Failed to create new data file (std::exception)
  DATA_LOG_createNewLogFileBstrst, // Failed to create new log file (BoosterSeat errno)
  DATA_LOG_createNewLogFileStdexcept, // Failed to create new log file (std::exception)
  DATA_LOG_appendToDataFile, // Failed to append data to file
  DATA_LOG_appendToLogFile, // Failed to append data to file
  DATA_LOG_dataFileSizeRead, // Failed to read data file size
  DATA_LOG_logFileSizeRead, // Failed to read log file size
  DATA_LOG_dataArchiveDirSizeRead, // Failed to read data archive directory size
  DATA_LOG_logArchiveDirSizeRead, // Failed to read log archive directory size
  DATA_LOG_archiveDataFile, // Failed to archive data file
  DATA_LOG_archiveLogFile, // Failed to archive log file
  DATA_LOG_rotateDataFile, // Failed to rotate data file
  DATA_LOG_rotateLogFile, // Failed to rotate log file
  SYSTEM_MODULE_systemInfoReadFail,
  SYSTEM_MODULE_cpuTempReadFail,
  SYSTEM_MODULE_diskInfoReadFail,
  EXTENSION_MODULE_failedToCreate, // Failed to Create Extension
  EXTENSION_startCall, // Can not start if already running, starting, or sleeping.
  EXTENSION_stopTimeout, // Failed to stop in time
  EXT_FAULT_none, // No fault
  EXT_FAULT_fatalConfig  // Fatal Configuration Error
};
} // namespace data

#endif // LOG_IDS_HPP_

// * * * * GENERATED AUTOMATICALLY, DO NOT MANUALLY MODIFY * * * *