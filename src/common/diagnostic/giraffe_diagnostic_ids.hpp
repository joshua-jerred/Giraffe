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


#ifndef GIRAFFE_DIAGNOSTIC_IDS_HPP_
#define GIRAFFE_DIAGNOSTIC_IDS_HPP_

#include <unordered_map>
#include <string>
#include <cstdint>

enum class DiagnosticId : uint16_t {
  GENERIC_unknown=0xF000, // Unknown
  GENERIC_info=0xF001, // info
  GENERIC_debug=0xF002, // debug
  GENERIC_notImplemented=0xF003, // Not Implemented
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
  DATA_MODULE_cameraNewImagePacketInvalidPath=0x2002, // Camera New Image Packet - Invalid Path
  DATA_MODULE_cameraNewImagePacketInvalidFields=0x2003, // Camera New Image Packet - Invalid Fields
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
  DATA_LOG_fileListFail=0x3029, // Failed to update archive file list
  SYSTEM_MODULE_systemInfoReadFail=0x4001, // Failed to read system info
  SYSTEM_MODULE_cpuTempReadFail=0x4002, // Failed to read CPU temperature
  SYSTEM_MODULE_diskInfoReadFail=0x4003, // Failed to read disk info
  EXTENSION_MODULE_failedToCreate=0x4004, // Failed to Create Extension
  EXTENSION_startCall=0x5001, // Can not start if already running, starting, or sleeping.
  EXTENSION_stopTimeout=0x5002, // Failed to stop in time
  EXTENSION_bme280TemperatureRange=0x5003, // BME280 Temperature Range Warning
  EXTENSION_bme280PressureRange=0x5004, // BME280 Pressure Range Warning
  EXTENSION_bme280HumidityRange=0x5005, // BME280 Humidity Range Warning
  EXTENSION_bme280Div0=0x5006, // BME280 DIV0
  EXTENSION_ds18b20TemperatureRange=0x5007, // DS18B20 Temperature Range Warning
  EXTENSION_samm8qReadWatchdog=0x5008, // SAMM8Q Read Watchdog
  EXTENSION_samm8qConfigureSetProtocol=0x5009, // SAMM8Q Configure Set Protocol
  EXTENSION_samm8qConfigureSetMeasurementRate=0x500A, // SAMM8Q Configure Set Measurement Rate
  EXTENSION_samm8qConfigureSetDynamicModel=0x500B, // SAMM8Q Configure Set Dynamic Model
  EXTENSION_samm8qReadTimeout=0x500C, // SAMM8Q Read Timeout
  EXT_FAULT_none=0x7000, // No fault
  EXT_FAULT_fatalConfig=0x7001, // Fatal Configuration Error
  EXT_FAULT_bme280StartupI2CConnect=0x7002, // BME280 Startup I2C Connect Fault
  EXT_FAULT_bme280StartupHandshake=0x7003, // BME280 Startup Handshake Fault
  EXT_FAULT_bme280StartupConfigure=0x7003, // BME280 Startup Configure Fault
  EXT_FAULT_bme280StartupReadCompensation=0x7003, // BME280 Startup Read Compensation Fault
  EXT_FAULT_bme280ReadTimeout=0x7004, // BME280 Read Timeout Fault
  EXT_FAULT_ds18b20IdInvalid=0x7005, // DS18B20 Invalid ID Fault
  EXT_FAULT_ds18b20DeviceNotFound=0x7006, // DS18B20 Device Not Found Fault
  EXT_FAULT_ds18b20ReadRaw=0x7007, // DS18B20 Timeout - Read Raw
  EXT_FAULT_ds18b20RawTempEmpty=0x7008, // DS18B20 Timeout - Raw Temp Empty
  EXT_FAULT_ds18b20RawTempStoi=0x7009, // DS18B20 Timeout - STOI
  EXT_FAULT_ds18b20InitTimeout=0x700A, // DS18B20 Timeout - Init
  EXT_FAULT_ds18b20ReadTimeout=0x700B, // DS18B20 Timeout - Read
  EXT_FAULT_samm8qI2CConnect=0x700C, // SAMM8Q I2C Connect Fault
  EXT_FAULT_samm8qStartupHandshake=0x700D, // SAMM8Q Startup Handshake Fault
  EXT_FAULT_samm8qResetFailure=0x700E, // SAMM8Q Reset Failure Fault
  GDL_invalidStartCall=0xA000, // Invalid Start Call
  GDL_invalidStopCall=0xA001, // Invalid Stop Call
  GDL_invalidBroadcastCall=0xA002, // Invalid Broadcast Call
  GDL_invalidExchangeCall=0xA003  // Invalid Exchange Call
};

extern const std::unordered_map<DiagnosticId, std::string> diagnosticIdStringMap;

#endif // GIRAFFE_DIAGNOSTIC_IDS_HPP_


/**
 * @endverbatim
 * 
 * &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
 * AUTOMATICALLY GENERATED, DO NOT EDIT MANUALLY
 * &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
 */