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


#include "giraffe_diagnostic_ids.hpp"


const std::unordered_map<DiagnosticId, std::string> K_DIAGNOSTIC_ID_STRING_MAP {
  {DiagnosticId::GENERIC_unknown,"GENERIC_unknown"},
  {DiagnosticId::GENERIC_info,"GENERIC_info"},
  {DiagnosticId::GENERIC_debug,"GENERIC_debug"},
  {DiagnosticId::GENERIC_notImplemented,"GENERIC_notImplemented"},
  {DiagnosticId::GENERIC_homeEnvVarNotSet,"GENERIC_homeEnvVarNotSet"},
  {DiagnosticId::GENERIC_homeEnvVarNotDir,"GENERIC_homeEnvVarNotDir"},
  {DiagnosticId::GENERIC_failedToCreateGiraffeDir,"GENERIC_failedToCreateGiraffeDir"},
  {DiagnosticId::GENERIC_failedToCreateGfsImageDir,"GENERIC_failedToCreateGfsImageDir"},
  {DiagnosticId::CONFIG_sectionNotFound,"CONFIG_sectionNotFound"},
  {DiagnosticId::CONFIG_settingNotFound,"CONFIG_settingNotFound"},
  {DiagnosticId::CONFIG_invalidJsonType,"CONFIG_invalidJsonType"},
  {DiagnosticId::CONFIG_invalidSettingValue,"CONFIG_invalidSettingValue"},
  {DiagnosticId::CONFIG_enumNotString,"CONFIG_enumNotString"},
  {DiagnosticId::CONFIG_keyNotFoundInEnum,"CONFIG_keyNotFoundInEnum"},
  {DiagnosticId::CONFIG_failedToSaveToPath,"CONFIG_failedToSaveToPath"},
  {DiagnosticId::CONFIG_failedToOpenConfig,"CONFIG_failedToOpenConfig"},
  {DiagnosticId::CONFIG_configFileDoesNotExist,"CONFIG_configFileDoesNotExist"},
  {DiagnosticId::CONFIG_failedToLoadSectionNotFound,"CONFIG_failedToLoadSectionNotFound"},
  {DiagnosticId::CONFIG_extensionsNotArray,"CONFIG_extensionsNotArray"},
  {DiagnosticId::CONFIG_extensionNotObject,"CONFIG_extensionNotObject"},
  {DiagnosticId::CONFIG_extensionNameAlreadyExists,"CONFIG_extensionNameAlreadyExists"},
  {DiagnosticId::CONFIG_extensionNameDoesNotExist,"CONFIG_extensionNameDoesNotExist"},
  {DiagnosticId::MODULE_startFailure,"MODULE_startFailure"},
  {DiagnosticId::MODULE_moduleAlreadyStarted,"MODULE_moduleAlreadyStarted"},
  {DiagnosticId::DATA_MODULE_statusDataPacketUnknownSource,"DATA_MODULE_statusDataPacketUnknownSource"},
  {DiagnosticId::DATA_MODULE_dataPacketUnknownType,"DATA_MODULE_dataPacketUnknownType"},
  {DiagnosticId::DATA_MODULE_cameraNewImagePacketInvalidPath,"DATA_MODULE_cameraNewImagePacketInvalidPath"},
  {DiagnosticId::DATA_MODULE_cameraNewImagePacketInvalidFields,"DATA_MODULE_cameraNewImagePacketInvalidFields"},
  {DiagnosticId::DATA_MODULE_unrecognizedCommand,"DATA_MODULE_unrecognizedCommand"},
  {DiagnosticId::DATA_MODULE_noGpsSource,"DATA_MODULE_noGpsSource"},
  {DiagnosticId::DATA_LOG_invalidDataframeStrategy,"DATA_LOG_invalidDataframeStrategy"},
  {DiagnosticId::DATA_LOG_archiveOldDataFiles,"DATA_LOG_archiveOldDataFiles"},
  {DiagnosticId::DATA_LOG_archiveOldLogFiles,"DATA_LOG_archiveOldLogFiles"},
  {DiagnosticId::DATA_LOG_createDataDirBstrst,"DATA_LOG_createDataDirBstrst"},
  {DiagnosticId::DATA_LOG_createDataDirStdexcept,"DATA_LOG_createDataDirStdexcept"},
  {DiagnosticId::DATA_LOG_dataDirDoesNotExist,"DATA_LOG_dataDirDoesNotExist"},
  {DiagnosticId::DATA_LOG_dataDirExistenceBstrst,"DATA_LOG_dataDirExistenceBstrst"},
  {DiagnosticId::DATA_LOG_dataDirExistenceStdexcept,"DATA_LOG_dataDirExistenceStdexcept"},
  {DiagnosticId::DATA_LOG_dataDirCreateFailed,"DATA_LOG_dataDirCreateFailed"},
  {DiagnosticId::DATA_LOG_createDataArchiveDirBstrst,"DATA_LOG_createDataArchiveDirBstrst"},
  {DiagnosticId::DATA_LOG_createDataArchiveDirStdexcept,"DATA_LOG_createDataArchiveDirStdexcept"},
  {DiagnosticId::DATA_LOG_dataArchiveDirDoesNotExist,"DATA_LOG_dataArchiveDirDoesNotExist"},
  {DiagnosticId::DATA_LOG_dataArchiveDirExistenceBstrst,"DATA_LOG_dataArchiveDirExistenceBstrst"},
  {DiagnosticId::DATA_LOG_dataArchiveDirExistenceStdexcept,"DATA_LOG_dataArchiveDirExistenceStdexcept"},
  {DiagnosticId::DATA_LOG_dataArchiveDirCreateFailed,"DATA_LOG_dataArchiveDirCreateFailed"},
  {DiagnosticId::DATA_LOG_createLogDirBstrst,"DATA_LOG_createLogDirBstrst"},
  {DiagnosticId::DATA_LOG_createLogDirStdexcept,"DATA_LOG_createLogDirStdexcept"},
  {DiagnosticId::DATA_LOG_logDirDoesNotExist,"DATA_LOG_logDirDoesNotExist"},
  {DiagnosticId::DATA_LOG_logDirExistenceBstrst,"DATA_LOG_logDirExistenceBstrst"},
  {DiagnosticId::DATA_LOG_logDirExistenceStdexcept,"DATA_LOG_logDirExistenceStdexcept"},
  {DiagnosticId::DATA_LOG_logDirCreateFailed,"DATA_LOG_logDirCreateFailed"},
  {DiagnosticId::DATA_LOG_createLogArchiveDirBstrst,"DATA_LOG_createLogArchiveDirBstrst"},
  {DiagnosticId::DATA_LOG_createLogArchiveDirStdexcept,"DATA_LOG_createLogArchiveDirStdexcept"},
  {DiagnosticId::DATA_LOG_logArchiveDirDoesNotExist,"DATA_LOG_logArchiveDirDoesNotExist"},
  {DiagnosticId::DATA_LOG_logArchiveDirExistenceBstrst,"DATA_LOG_logArchiveDirExistenceBstrst"},
  {DiagnosticId::DATA_LOG_logArchiveDirExistenceStdexcept,"DATA_LOG_logArchiveDirExistenceStdexcept"},
  {DiagnosticId::DATA_LOG_logArchiveDirCreateFailed,"DATA_LOG_logArchiveDirCreateFailed"},
  {DiagnosticId::DATA_LOG_createNewDataFileBstrst,"DATA_LOG_createNewDataFileBstrst"},
  {DiagnosticId::DATA_LOG_createNewDataFileStdexcept,"DATA_LOG_createNewDataFileStdexcept"},
  {DiagnosticId::DATA_LOG_createNewLogFileBstrst,"DATA_LOG_createNewLogFileBstrst"},
  {DiagnosticId::DATA_LOG_createNewLogFileStdexcept,"DATA_LOG_createNewLogFileStdexcept"},
  {DiagnosticId::DATA_LOG_appendToDataFile,"DATA_LOG_appendToDataFile"},
  {DiagnosticId::DATA_LOG_appendToLogFile,"DATA_LOG_appendToLogFile"},
  {DiagnosticId::DATA_LOG_dataFileSizeRead,"DATA_LOG_dataFileSizeRead"},
  {DiagnosticId::DATA_LOG_logFileSizeRead,"DATA_LOG_logFileSizeRead"},
  {DiagnosticId::DATA_LOG_dataArchiveDirSizeRead,"DATA_LOG_dataArchiveDirSizeRead"},
  {DiagnosticId::DATA_LOG_logArchiveDirSizeRead,"DATA_LOG_logArchiveDirSizeRead"},
  {DiagnosticId::DATA_LOG_archiveDataFile,"DATA_LOG_archiveDataFile"},
  {DiagnosticId::DATA_LOG_archiveLogFile,"DATA_LOG_archiveLogFile"},
  {DiagnosticId::DATA_LOG_rotateDataFile,"DATA_LOG_rotateDataFile"},
  {DiagnosticId::DATA_LOG_rotateLogFile,"DATA_LOG_rotateLogFile"},
  {DiagnosticId::DATA_LOG_fileListFail,"DATA_LOG_fileListFail"},
  {DiagnosticId::DATA_LOG_dataArchiveTrimmed,"DATA_LOG_dataArchiveTrimmed"},
  {DiagnosticId::DATA_LOG_dataArchiveTrimFailed,"DATA_LOG_dataArchiveTrimFailed"},
  {DiagnosticId::SYSTEM_MODULE_systemInfoReadFail,"SYSTEM_MODULE_systemInfoReadFail"},
  {DiagnosticId::SYSTEM_MODULE_cpuTempReadFail,"SYSTEM_MODULE_cpuTempReadFail"},
  {DiagnosticId::SYSTEM_MODULE_diskInfoReadFail,"SYSTEM_MODULE_diskInfoReadFail"},
  {DiagnosticId::SERVER_MODULE_invalidCommandString,"SERVER_MODULE_invalidCommandString"},
  {DiagnosticId::EXTENSION_MODULE_failedToCreate,"EXTENSION_MODULE_failedToCreate"},
  {DiagnosticId::EXTENSION_startCall,"EXTENSION_startCall"},
  {DiagnosticId::EXTENSION_stopTimeout,"EXTENSION_stopTimeout"},
  {DiagnosticId::EXTENSION_bme280TemperatureRange,"EXTENSION_bme280TemperatureRange"},
  {DiagnosticId::EXTENSION_bme280PressureRange,"EXTENSION_bme280PressureRange"},
  {DiagnosticId::EXTENSION_bme280HumidityRange,"EXTENSION_bme280HumidityRange"},
  {DiagnosticId::EXTENSION_bme280Div0,"EXTENSION_bme280Div0"},
  {DiagnosticId::EXTENSION_ds18b20TemperatureRange,"EXTENSION_ds18b20TemperatureRange"},
  {DiagnosticId::EXTENSION_samm8qReadWatchdog,"EXTENSION_samm8qReadWatchdog"},
  {DiagnosticId::EXTENSION_samm8qConfigureSetProtocol,"EXTENSION_samm8qConfigureSetProtocol"},
  {DiagnosticId::EXTENSION_samm8qConfigureSetMeasurementRate,"EXTENSION_samm8qConfigureSetMeasurementRate"},
  {DiagnosticId::EXTENSION_samm8qConfigureSetDynamicModel,"EXTENSION_samm8qConfigureSetDynamicModel"},
  {DiagnosticId::EXTENSION_samm8qReadTimeout,"EXTENSION_samm8qReadTimeout"},
  {DiagnosticId::EXTENSION_adcConfigFail,"EXTENSION_adcConfigFail"},
  {DiagnosticId::EXTENSION_piCameraCaptureFail,"EXTENSION_piCameraCaptureFail"},
  {DiagnosticId::FLIGHT_RUNNER_failedToRouteCommand,"FLIGHT_RUNNER_failedToRouteCommand"},
  {DiagnosticId::FLIGHT_RUNNER_commandDestinationNotFound,"FLIGHT_RUNNER_commandDestinationNotFound"},
  {DiagnosticId::FLIGHT_RUNNER_invalidCommand,"FLIGHT_RUNNER_invalidCommand"},
  {DiagnosticId::EXT_FAULT_none,"EXT_FAULT_none"},
  {DiagnosticId::EXT_FAULT_fatalConfig,"EXT_FAULT_fatalConfig"},
  {DiagnosticId::EXT_FAULT_bme280StartupI2CConnect,"EXT_FAULT_bme280StartupI2CConnect"},
  {DiagnosticId::EXT_FAULT_bme280StartupHandshake,"EXT_FAULT_bme280StartupHandshake"},
  {DiagnosticId::EXT_FAULT_bme280StartupConfigure,"EXT_FAULT_bme280StartupConfigure"},
  {DiagnosticId::EXT_FAULT_bme280StartupReadCompensation,"EXT_FAULT_bme280StartupReadCompensation"},
  {DiagnosticId::EXT_FAULT_bme280ReadTimeout,"EXT_FAULT_bme280ReadTimeout"},
  {DiagnosticId::EXT_FAULT_ds18b20IdInvalid,"EXT_FAULT_ds18b20IdInvalid"},
  {DiagnosticId::EXT_FAULT_ds18b20DeviceNotFound,"EXT_FAULT_ds18b20DeviceNotFound"},
  {DiagnosticId::EXT_FAULT_ds18b20ReadRaw,"EXT_FAULT_ds18b20ReadRaw"},
  {DiagnosticId::EXT_FAULT_ds18b20RawTempEmpty,"EXT_FAULT_ds18b20RawTempEmpty"},
  {DiagnosticId::EXT_FAULT_ds18b20RawTempStoi,"EXT_FAULT_ds18b20RawTempStoi"},
  {DiagnosticId::EXT_FAULT_ds18b20InitTimeout,"EXT_FAULT_ds18b20InitTimeout"},
  {DiagnosticId::EXT_FAULT_ds18b20ReadTimeout,"EXT_FAULT_ds18b20ReadTimeout"},
  {DiagnosticId::EXT_FAULT_samm8qI2CConnect,"EXT_FAULT_samm8qI2CConnect"},
  {DiagnosticId::EXT_FAULT_samm8qStartupHandshake,"EXT_FAULT_samm8qStartupHandshake"},
  {DiagnosticId::EXT_FAULT_samm8qResetFailure,"EXT_FAULT_samm8qResetFailure"},
  {DiagnosticId::EXT_FAULT_i2cBusConnectFault,"EXT_FAULT_i2cBusConnectFault"},
  {DiagnosticId::EXT_FAULT_i2cAddressSetFault,"EXT_FAULT_i2cAddressSetFault"},
  {DiagnosticId::EXT_FAULT_i2cUnknownFault,"EXT_FAULT_i2cUnknownFault"},
  {DiagnosticId::EXT_FAULT_i2cHandshakeTimeoutFault,"EXT_FAULT_i2cHandshakeTimeoutFault"},
  {DiagnosticId::EXT_FAULT_adcWatchdogTimeout,"EXT_FAULT_adcWatchdogTimeout"},
  {DiagnosticId::EXT_FAULT_bmi088AddressError,"EXT_FAULT_bmi088AddressError"},
  {DiagnosticId::EXT_FAULT_bmi088AccelHandshakeError,"EXT_FAULT_bmi088AccelHandshakeError"},
  {DiagnosticId::EXT_FAULT_bmi088GyroHandshakeError,"EXT_FAULT_bmi088GyroHandshakeError"},
  {DiagnosticId::EXT_FAULT_bmi088AccelOpenError,"EXT_FAULT_bmi088AccelOpenError"},
  {DiagnosticId::EXT_FAULT_bmi088GyroOpenError,"EXT_FAULT_bmi088GyroOpenError"},
  {DiagnosticId::EXT_FAULT_bmi088AccelConfigError,"EXT_FAULT_bmi088AccelConfigError"},
  {DiagnosticId::EXT_FAULT_bmi088GyroConfigError,"EXT_FAULT_bmi088GyroConfigError"},
  {DiagnosticId::EXT_FAULT_bmi088ReadTimeout,"EXT_FAULT_bmi088ReadTimeout"},
  {DiagnosticId::EXT_FAULT_rgbStatusLedExtraArgs,"EXT_FAULT_rgbStatusLedExtraArgs"},
  {DiagnosticId::EXT_FAULT_rgbStatusLedFailedToSetupPins,"EXT_FAULT_rgbStatusLedFailedToSetupPins"},
  {DiagnosticId::EXT_FAULT_rgbStatusLedShutdownFailed,"EXT_FAULT_rgbStatusLedShutdownFailed"},
  {DiagnosticId::EXT_FAULT_piCameraNotOnPi,"EXT_FAULT_piCameraNotOnPi"},
  {DiagnosticId::EXT_FAULT_piCameraNotDetected,"EXT_FAULT_piCameraNotDetected"},
  {DiagnosticId::EXT_FAULT_piCameraDetectFailed,"EXT_FAULT_piCameraDetectFailed"},
  {DiagnosticId::EXT_FAULT_piCameraInvalidImageDir,"EXT_FAULT_piCameraInvalidImageDir"},
  {DiagnosticId::EXT_FAULT_piCameraCaptureFailed,"EXT_FAULT_piCameraCaptureFailed"},
  {DiagnosticId::INTERFACE_invalidGpioPin,"INTERFACE_invalidGpioPin"},
  {DiagnosticId::INTERFACE_invalidGpioPinMode,"INTERFACE_invalidGpioPinMode"},
  {DiagnosticId::INTERFACE_initialStateInputPin,"INTERFACE_initialStateInputPin"},
  {DiagnosticId::INTERFACE_gpioFailedToInit,"INTERFACE_gpioFailedToInit"},
  {DiagnosticId::INTERFACE_gpioFailedToMapRegisters,"INTERFACE_gpioFailedToMapRegisters"},
  {DiagnosticId::INTERFACE_gpioNotInitialized,"INTERFACE_gpioNotInitialized"},
  {DiagnosticId::INTERFACE_gpioPinAlreadyReserved,"INTERFACE_gpioPinAlreadyReserved"},
  {DiagnosticId::INTERFACE_gpioNotOwner,"INTERFACE_gpioNotOwner"},
  {DiagnosticId::INTERFACE_gpioPinNotOutput,"INTERFACE_gpioPinNotOutput"},
  {DiagnosticId::INTERFACE_gpioNotReserverOrNotOwner,"INTERFACE_gpioNotReserverOrNotOwner"},
  {DiagnosticId::INTERFACE_gpioCanNotReadOutputPin,"INTERFACE_gpioCanNotReadOutputPin"},
  {DiagnosticId::GDL_invalidStartCall,"GDL_invalidStartCall"},
  {DiagnosticId::GDL_invalidStopCall,"GDL_invalidStopCall"},
  {DiagnosticId::GDL_invalidBroadcastCall,"GDL_invalidBroadcastCall"},
  {DiagnosticId::GDL_invalidExchangeCall,"GDL_invalidExchangeCall"},
  {DiagnosticId::TELEMETRY_invalidCommand,"TELEMETRY_invalidCommand"}
};



/**
 * @endverbatim
 * 
 * &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
 * AUTOMATICALLY GENERATED, DO NOT EDIT MANUALLY
 * &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
 */