/**
 *
 * 
 * &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
 * AUTOMATICALLY GENERATED, DO NOT EDIT MANUALLY
 * &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
 *
 *
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 * 
 * 
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-06-30
 * @copyright  2023 (license to be defined)
 * =*=======================*=
 *
 * @verbatim
 */


#ifndef LOG_IDS_HPP_
#define LOG_IDS_HPP_

namespace data {
enum class LogId : uint16_t {
  GENERIC_unknown=0xF000, // Unknown
  GENERIC_info=0xF001, // Info
  GENERIC_debug=0xF002, // Debug
  GENERIC_notYetImplemented=0xF003, // Not yet implemented
  CONFIG_sectionNotFound=0x000E, // Section not found
  CONFIG_settingNotFound=0x0001, // Setting not found
  CONFIG_invalidJsonType=0x0002, // Invalid JSON type
  CONFIG_invalidSettingValue=0x0003, // Invalid setting value
  CONFIG_enumNotString=0x0004, // Enum value is not a string
  CONFIG_keyNotFoundInEnum=0x0005, // Key not found in enum
  CONFIG_failedToSaveToPath=0x0006, // Failed to save to path
  CONFIG_failedToOpenConfig=0x0007, // Failed to open config
  CONFIG_configFileDoesNotExist=0x0008, // Config file does not exist
  CONFIG_failedToLoadSectionNotFound=0x0009, // Failed to load config, section not found
  CONFIG_extensionsNotArray=0x000A, // Extensions in config file is not an array
  CONFIG_extensionNotObject=0x000B, // Extension is not an object
  CONFIG_extensionNameAlreadyExists=0x000C, // Extension Name Already Exists
  CONFIG_extensionNameDoesNotExist=0x000D, // Extension Name Does Not Exist
  MODULE_startFailure=0x1000, // Failed to start module
  MODULE_moduleAlreadyStarted=0x1001, // Module already started
  DATA_MODULE_statusDataPacketUnknownSource=0x2000, // Unknown data packet source
  DATA_MODULE_dataPacketUnknownType=0x2001, // Unknown data packet type
  DATA_LOG_invalidDataframeStrategy=0x3000, // Invalid data log strategy
  DATA_LOG_archiveOldDataFiles=0x3001, // Failed to archive old data files
  DATA_LOG_archiveOldLogFiles=0x3002, // Failed to archive old log files
  DATA_LOG_createDataDirBstrst=0x3003, // Failed to create data directory at startup (BoosterSeat errno)
  DATA_LOG_createDataDirStdexcept=0x3004, // Failed to create data directory at startup (std::exception)
  DATA_LOG_dataDirDoesNotExist=0x3005, // Data directory does not exist - after startup
  DATA_LOG_dataDirExistenceBstrst=0x3006, // Failed to check data directory existence (BoosterSeat errno)
  DATA_LOG_dataDirExistenceStdexcept=0x3007, // Failed to check data directory existence (std::exception)
  DATA_LOG_dataDirCreateFailed=0x3008, // Failed after reaching max attempts to create data directory
  DATA_LOG_createDataArchiveDirBstrst=0x3009, // Failed to create data archive directory at startup (BoosterSeat errno)
  DATA_LOG_createDataArchiveDirStdexcept=0x300A, // Failed to create data archive directory at startup (std::exception)
  DATA_LOG_dataArchiveDirDoesNotExist=0x300B, // Data archive directory does not exist - after startup
  DATA_LOG_dataArchiveDirExistenceBstrst=0x300C, // Failed to check data archive directory existence (BoosterSeat errno)
  DATA_LOG_dataArchiveDirExistenceStdexcept=0x300D, // Failed to check data archive directory existence (std::exception)
  DATA_LOG_dataArchiveDirCreateFailed=0x300E, // Failed after reaching max attempts to create data archive directory
  DATA_LOG_createLogDirBstrst=0x300F, // Failed to create log directory at startup (BoosterSeat errno)
  DATA_LOG_createLogDirStdexcept=0x3010, // Failed to create log directory at startup (std::exception)
  DATA_LOG_logDirDoesNotExist=0x3011, // Data directory does not exist - after startup
  DATA_LOG_logDirExistenceBstrst=0x3012, // Failed to check log directory existence (BoosterSeat errno)
  DATA_LOG_logDirExistenceStdexcept=0x3013, // Failed to check log directory existence (std::exception)
  DATA_LOG_logDirCreateFailed=0x3014, // Failed after reaching max attempts to create log directory
  DATA_LOG_createLogArchiveDirBstrst=0x3015, // Failed to create log archive directory at startup (BoosterSeat errno)
  DATA_LOG_createLogArchiveDirStdexcept=0x3016, // Failed to create log archive directory at startup (std::exception)
  DATA_LOG_logArchiveDirDoesNotExist=0x3017, // Data archive directory does not exist - after startup
  DATA_LOG_logArchiveDirExistenceBstrst=0x3018, // Failed to check log directory existence (BoosterSeat errno)
  DATA_LOG_logArchiveDirExistenceStdexcept=0x3019, // Failed to check log archive directory existence (std::exception)
  DATA_LOG_logArchiveDirCreateFailed=0x301A, // Failed after reaching max attempts to create log directory
  DATA_LOG_createNewDataFileBstrst=0x301B, // Failed to create new data file (BoosterSeat errno)
  DATA_LOG_createNewDataFileStdexcept=0x301C, // Failed to create new data file (std::exception)
  DATA_LOG_createNewLogFileBstrst=0x301D, // Failed to create new log file (BoosterSeat errno)
  DATA_LOG_createNewLogFileStdexcept=0x301E, // Failed to create new log file (std::exception)
  DATA_LOG_appendToDataFile=0x301F, // Failed to append data to file
  DATA_LOG_appendToLogFile=0x3020, // Failed to append data to file
  DATA_LOG_dataFileSizeRead=0x3021, // Failed to read data file size
  DATA_LOG_logFileSizeRead=0x3022, // Failed to read log file size
  DATA_LOG_dataArchiveDirSizeRead=0x3023, // Failed to read data archive directory size
  DATA_LOG_logArchiveDirSizeRead=0x3024, // Failed to read log archive directory size
  DATA_LOG_archiveDataFile=0x3025, // Failed to archive data file
  DATA_LOG_archiveLogFile=0x3026, // Failed to archive log file
  DATA_LOG_rotateDataFile=0x3027, // Failed to rotate data file
  DATA_LOG_rotateLogFile=0x3028, // Failed to rotate log file
  SYSTEM_MODULE_systemInfoReadFail=0x4001, // Failed to read system info
  SYSTEM_MODULE_cpuTempReadFail=0x4002, // Failed to read CPU temperature
  SYSTEM_MODULE_diskInfoReadFail=0x4003, // Failed to read disk info
  EXTENSION_MODULE_failedToCreate=0x4004, // Failed to Create Extension
  EXTENSION_startCall=0x5001, // Can not start if already running, starting, or sleeping.
  EXTENSION_stopTimeout=0x5002, // Failed to stop in time
  EXT_FAULT_none=0x6000, // No fault
  EXT_FAULT_fatalConfig=0x6001  // Fatal Configuration Error
};
} // namespace data

#endif // LOG_IDS_HPP_


/**
 * @endverbatim
 * 
 * &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
 * AUTOMATICALLY GENERATED, DO NOT EDIT MANUALLY
 * &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
 */