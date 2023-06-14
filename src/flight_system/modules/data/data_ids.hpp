// * * * * GENERATED AUTOMATICALLY, DO NOT MANUALLY MODIFY * * * *

#ifndef DATA_IDS_HPP_
#define DATA_IDS_HPP_

namespace data {
enum class DataId {
  GENERIC_unknown,
  GENERIC_test,
  MODULE_statusUpdate,
  MODULE_STATUS_data, // Data Module Status
  MODULE_STATUS_console, // Console Module Status
  MODULE_STATUS_server,
  MODULE_STATUS_system,
  SYSTEM_MODULE_cpu,
  DATA_MODULE_dsProcessDelay, // Data Stream Processing Delay
  DATA_MODULE_esPd, // Error Stream Processing Delay
  DATA_MODULE_loggingDataToFile,
  DATA_MODULE_loggingErrorsToFile,
  SERVER_MODULE_numTotalPacketsRecv,
  SERVER_MODULE_numTotalPacketsSent,
  SERVER_MODULE_numFailedPackets,
  SERVER_MODULE_clientAddress,
  SERVER_MODULE_connectionStatus,
  CONFIGURATION_configurationErrors,
  STREAMS_dataStreamCurrent,
  STREAMS_dataStreamTotal,
  STREAMS_errorStreamCurrent,
  STREAMS_errorStreamTotal,
  CRITICAL_flightPhase, // Flight Phase
  POSITION_GPS_lat,
  POSITION_GPS_lon,
  POSITION_IMU_xRoll,
  POSITION_IMU_yRoll,
  POSITION_IMU_zRoll,
  EXTENSION_statusUpdate
};
} // namespace data

#endif // DATA_IDS_HPP_

// * * * * GENERATED AUTOMATICALLY, DO NOT MANUALLY MODIFY * * * *