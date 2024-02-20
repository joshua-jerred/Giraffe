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


#ifndef CONFIGURATION_HPP_
#define CONFIGURATION_HPP_

#include <string>
#include <fstream>
#include <mutex>
#include <unordered_map>
#include <vector>
#include "json.hpp"
#include "configuration_enums.hpp"
#include "shared_data.hpp"
#include "sections/cfg_section.hpp"
#include "sections/cfg_extensions.hpp"


namespace cfg {
class General : public cfg::CfgSection {
public:
  General(data::Streams &streams): cfg::CfgSection(streams){}

  std::string getProjectName() const;
  int getFlightIdentifierNumber() const;
  cfg::gEnum::MainBoard getMainBoard() const;
  cfg::gEnum::ProcedureType getStartingProcedure() const;
  int getModuleStatusUpdateRate() const;

  void setProjectName(std::string);
  void setFlightIdentifierNumber(int);
  void setMainBoard(cfg::gEnum::MainBoard);
  void setStartingProcedure(cfg::gEnum::ProcedureType);
  void setModuleStatusUpdateRate(int);

  void setFromJson(const Json&);
  Json getJson() const;

private:
  std::string project_name_ = "Giraffe Flight 1";
  int flight_identifier_number_ = 0;
  cfg::gEnum::MainBoard main_board_ = cfg::gEnum::MainBoard::OTHER;
  cfg::gEnum::ProcedureType starting_procedure_ = cfg::gEnum::ProcedureType::OTHER;
  int module_status_update_rate_ = 1000;
};

class DataModuleData : public cfg::CfgSection {
public:
  DataModuleData(data::Streams &streams): cfg::CfgSection(streams){}

  bool getLogDataToFile() const;
  int getFileSystemCheckInterval() const;
  cfg::gEnum::LogStrategy getLogStrategy() const;
  cfg::gEnum::TimestampDetail getTimestampDetail() const;
  cfg::gEnum::TimestampTimezone getTimestampTimezone() const;
  int getLogIntervalMs() const;
  int getMaxDataLogFileSizeMb() const;
  int getMaxDataArchiveSizeMb() const;
  bool getWarnOnTrim() const;
  cfg::gEnum::ArchiveMethod getArchiveMethod() const;
  std::string getDataLogFileContents() const;

  void setLogDataToFile(bool);
  void setFileSystemCheckInterval(int);
  void setLogStrategy(cfg::gEnum::LogStrategy);
  void setTimestampDetail(cfg::gEnum::TimestampDetail);
  void setTimestampTimezone(cfg::gEnum::TimestampTimezone);
  void setLogIntervalMs(int);
  void setMaxDataLogFileSizeMb(int);
  void setMaxDataArchiveSizeMb(int);
  void setWarnOnTrim(bool);
  void setArchiveMethod(cfg::gEnum::ArchiveMethod);
  void setDataLogFileContents(std::string);

  void setFromJson(const Json&);
  Json getJson() const;

private:
  bool log_data_to_file_ = true;
  int file_system_check_interval_ = 10000;
  cfg::gEnum::LogStrategy log_strategy_ = cfg::gEnum::LogStrategy::INTERVAL;
  cfg::gEnum::TimestampDetail timestamp_detail_ = cfg::gEnum::TimestampDetail::HHMMSS;
  cfg::gEnum::TimestampTimezone timestamp_timezone_ = cfg::gEnum::TimestampTimezone::SYSTEM;
  int log_interval_ms_ = 5000;
  int max_data_log_file_size_mb_ = 10;
  int max_data_archive_size_mb_ = 100;
  bool warn_on_trim_ = true;
  cfg::gEnum::ArchiveMethod archive_method_ = cfg::gEnum::ArchiveMethod::PLAIN_TEXT;
  std::string data_log_file_contents_ = "not implemented";
};

class DataModuleInfluxdb : public cfg::CfgSection {
public:
  DataModuleInfluxdb(data::Streams &streams): cfg::CfgSection(streams){}

  bool getInfluxEnabled() const;
  bool getLogErrors() const;
  std::string getUrl() const;
  std::string getToken() const;
  std::string getOrganization() const;
  std::string getDataBucket() const;
  std::string getErrorBucket() const;
  cfg::gEnum::InfluxdbRetentionPolicy getRetentionPolicy() const;
  std::string getContents() const;
  int getDataDumpInterval() const;

  void setInfluxEnabled(bool);
  void setLogErrors(bool);
  void setUrl(std::string);
  void setToken(std::string);
  void setOrganization(std::string);
  void setDataBucket(std::string);
  void setErrorBucket(std::string);
  void setRetentionPolicy(cfg::gEnum::InfluxdbRetentionPolicy);
  void setContents(std::string);
  void setDataDumpInterval(int);

  void setFromJson(const Json&);
  Json getJson() const;

private:
  bool influx_enabled_ = false;
  bool log_errors_ = false;
  std::string url_ = "http://localhost:8086/";
  std::string token_ = "none";
  std::string organization_ = "giraffe";
  std::string data_bucket_ = "gfs_data";
  std::string error_bucket_ = "gfs_errors";
  cfg::gEnum::InfluxdbRetentionPolicy retention_policy_ = cfg::gEnum::InfluxdbRetentionPolicy::INF;
  std::string contents_ = "not implemented";
  int data_dump_interval_ = 1000;
};

class DataModuleLog : public cfg::CfgSection {
public:
  DataModuleLog(data::Streams &streams): cfg::CfgSection(streams){}

  bool getLogToFile() const;
  int getMaxLogFileSizeMb() const;
  int getMaxLogArchiveSizeMb() const;
  cfg::gEnum::ArchiveMethod getErrorArchiveMethod() const;
  cfg::gEnum::LogLevel getLogLevel() const;
  cfg::gEnum::ErrorLogStrategy getErrorLogStrategy() const;
  int getErrorFrameLogInterval() const;

  void setLogToFile(bool);
  void setMaxLogFileSizeMb(int);
  void setMaxLogArchiveSizeMb(int);
  void setErrorArchiveMethod(cfg::gEnum::ArchiveMethod);
  void setLogLevel(cfg::gEnum::LogLevel);
  void setErrorLogStrategy(cfg::gEnum::ErrorLogStrategy);
  void setErrorFrameLogInterval(int);

  void setFromJson(const Json&);
  Json getJson() const;

private:
  bool log_to_file_ = true;
  int max_log_file_size_mb_ = 10;
  int max_log_archive_size_mb_ = 50;
  cfg::gEnum::ArchiveMethod error_archive_method_ = cfg::gEnum::ArchiveMethod::PLAIN_TEXT;
  cfg::gEnum::LogLevel log_level_ = cfg::gEnum::LogLevel::INFO;
  cfg::gEnum::ErrorLogStrategy error_log_strategy_ = cfg::gEnum::ErrorLogStrategy::ALL;
  int error_frame_log_interval_ = 60000;
};

class ConsoleModule : public cfg::CfgSection {
public:
  ConsoleModule(data::Streams &streams): cfg::CfgSection(streams){}

  bool getEnabled() const;
  int getUpdateInterval() const;

  void setEnabled(bool);
  void setUpdateInterval(int);

  void setFromJson(const Json&);
  Json getJson() const;

private:
  bool enabled_ = true;
  int update_interval_ = 1000;
};

class ServerModule : public cfg::CfgSection {
public:
  ServerModule(data::Streams &streams): cfg::CfgSection(streams){}

  bool getEnabled() const;
  int getTcpSocketPort() const;

  void setEnabled(bool);
  void setTcpSocketPort(int);

  void setFromJson(const Json&);
  Json getJson() const;

private:
  bool enabled_ = true;
  int tcp_socket_port_ = 7893;
};

class SystemModule : public cfg::CfgSection {
public:
  SystemModule(data::Streams &streams): cfg::CfgSection(streams){}

  bool getEnabled() const;
  int getSystemInfoPollRateMs() const;

  void setEnabled(bool);
  void setSystemInfoPollRateMs(int);

  void setFromJson(const Json&);
  Json getJson() const;

private:
  bool enabled_ = true;
  int system_info_poll_rate_ms_ = 1000;
};

class Telemetry : public cfg::CfgSection {
public:
  Telemetry(data::Streams &streams): cfg::CfgSection(streams){}

  bool getTelemetryEnabled() const;
  std::string getCallSign() const;
  bool getDataLinkEnabled() const;

  void setTelemetryEnabled(bool);
  void setCallSign(std::string);
  void setDataLinkEnabled(bool);

  void setFromJson(const Json&);
  Json getJson() const;

private:
  bool telemetry_enabled_ = false;
  std::string call_sign_ = "N0CALL";
  bool data_link_enabled_ = false;
};

class TelemetryAprs : public cfg::CfgSection {
public:
  TelemetryAprs(data::Streams &streams): cfg::CfgSection(streams){}

  bool getTelemetryPackets() const;
  bool getPositionPackets() const;
  int getPositionPacketInterval() const;
  std::string getFrequency() const;
  int getSsid() const;
  std::string getDestinationAddress() const;
  int getDestinationSsid() const;
  cfg::gEnum::AprsSymbolTable getSymbolTable() const;
  std::string getSymbol() const;
  std::string getComment() const;

  void setTelemetryPackets(bool);
  void setPositionPackets(bool);
  void setPositionPacketInterval(int);
  void setFrequency(std::string);
  void setSsid(int);
  void setDestinationAddress(std::string);
  void setDestinationSsid(int);
  void setSymbolTable(cfg::gEnum::AprsSymbolTable);
  void setSymbol(std::string);
  void setComment(std::string);

  void setFromJson(const Json&);
  Json getJson() const;

private:
  bool telemetry_packets_ = false;
  bool position_packets_ = false;
  int position_packet_interval_ = 600;
  std::string frequency_ = "144.3900";
  int ssid_ = 0;
  std::string destination_address_ = "APRS";
  int destination_ssid_ = 0;
  cfg::gEnum::AprsSymbolTable symbol_table_ = cfg::gEnum::AprsSymbolTable::PRIMARY;
  std::string symbol_ = "/";
  std::string comment_ = "Giraffe Flight Software";
};

class TelemetrySstv : public cfg::CfgSection {
public:
  TelemetrySstv(data::Streams &streams): cfg::CfgSection(streams){}

  bool getEnabled() const;
  std::string getFrequency() const;
  cfg::gEnum::SstvMode getMode() const;
  bool getOverlayData() const;

  void setEnabled(bool);
  void setFrequency(std::string);
  void setMode(cfg::gEnum::SstvMode);
  void setOverlayData(bool);

  void setFromJson(const Json&);
  Json getJson() const;

private:
  bool enabled_ = false;
  std::string frequency_ = "145.5100";
  cfg::gEnum::SstvMode mode_ = cfg::gEnum::SstvMode::ROBOT36;
  bool overlay_data_ = true;
};

class TelemetryDataPackets : public cfg::CfgSection {
public:
  TelemetryDataPackets(data::Streams &streams): cfg::CfgSection(streams){}

  bool getEnabled() const;
  std::string getFrequency() const;
  cfg::gEnum::DataPacketsMode getMode() const;
  bool getMorseCallSign() const;
  std::string getComment() const;

  void setEnabled(bool);
  void setFrequency(std::string);
  void setMode(cfg::gEnum::DataPacketsMode);
  void setMorseCallSign(bool);
  void setComment(std::string);

  void setFromJson(const Json&);
  Json getJson() const;

private:
  bool enabled_ = false;
  std::string frequency_ = "145.5100";
  cfg::gEnum::DataPacketsMode mode_ = cfg::gEnum::DataPacketsMode::BPSK250;
  bool morse_call_sign_ = true;
  std::string comment_ = "Giraffe Flight Software";
};

class ExtensionModule : public cfg::CfgSection {
public:
  ExtensionModule(data::Streams &streams): cfg::CfgSection(streams){}

  int getStatusPollingRate() const;
  int getMaxRestartAttempts() const;
  int getRestartDelayMinimum() const;
  int getStartTimeout() const;
  int getMaxStartupAttempts() const;

  void setStatusPollingRate(int);
  void setMaxRestartAttempts(int);
  void setRestartDelayMinimum(int);
  void setStartTimeout(int);
  void setMaxStartupAttempts(int);

  void setFromJson(const Json&);
  Json getJson() const;

private:
  int status_polling_rate_ = 500;
  int max_restart_attempts_ = 5;
  int restart_delay_minimum_ = 1000;
  int start_timeout_ = 10000;
  int max_startup_attempts_ = 5;
};

class HardwareInterface : public cfg::CfgSection {
public:
  HardwareInterface(data::Streams &streams): cfg::CfgSection(streams){}

  cfg::gEnum::I2CBus getI2CBus() const;

  void setI2CBus(cfg::gEnum::I2CBus);

  void setFromJson(const Json&);
  Json getJson() const;

private:
  cfg::gEnum::I2CBus i2c_bus_ = cfg::gEnum::I2CBus::I2C_1;
};

class Configuration {
 public:
  Configuration(data::Streams &streams):
    
    general(streams),    
    data_module_data(streams),    
    data_module_influxdb(streams),    
    data_module_log(streams),    
    console_module(streams),    
    server_module(streams),    
    system_module(streams),    
    telemetry(streams),    
    telemetry_aprs(streams),    
    telemetry_sstv(streams),    
    telemetry_data_packets(streams),    
    extensions(streams),    
    extension_module(streams),    
    hardware_interface(streams),
    streams_(streams){}
    
    void getAllJson(Json &all_data) const;
    
    void save(std::string file_path = "");
    void load(std::string file_path = "");

  cfg::General general;
  cfg::DataModuleData data_module_data;
  cfg::DataModuleInfluxdb data_module_influxdb;
  cfg::DataModuleLog data_module_log;
  cfg::ConsoleModule console_module;
  cfg::ServerModule server_module;
  cfg::SystemModule system_module;
  cfg::Telemetry telemetry;
  cfg::TelemetryAprs telemetry_aprs;
  cfg::TelemetrySstv telemetry_sstv;
  cfg::TelemetryDataPackets telemetry_data_packets;
  cfg::Extensions extensions;
  cfg::ExtensionModule extension_module;
  cfg::HardwareInterface hardware_interface;
 
 private:
  void error(DiagnosticId error_code, std::string info = "") {
      streams_.log.error(node::Identification::CONFIGURATION, error_code, info);
  }
 
  data::Streams &streams_;
  std::mutex file_lock_ = std::mutex();
};

} // namespace cfg

#endif // CONFIGURATION_HPP_


/**
 * @endverbatim
 * 
 * &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
 * AUTOMATICALLY GENERATED, DO NOT EDIT MANUALLY
 * &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
 */