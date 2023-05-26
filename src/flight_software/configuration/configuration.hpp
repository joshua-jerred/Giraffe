// * * * AUTOMATICALLY GENERATED WITH CMake/Python * * *

#ifndef CONFIGURATION_HPP_
#define CONFIGURATION_HPP_

#include <string>
#include <fstream>
#include <mutex>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include "shared_data.hpp"

using json = nlohmann::ordered_json;

namespace cfg {
namespace gEnum {

enum class MainBoard {
  OTHER,
  PI_ZERO_W2,
  PI_4
};
static std::unordered_map<std::string, cfg::gEnum::MainBoard> const KeyToMainBoard = {
  {"other", cfg::gEnum::MainBoard::OTHER},
  {"pi_zero_w2", cfg::gEnum::MainBoard::PI_ZERO_W2},
  {"pi_4", cfg::gEnum::MainBoard::PI_4}
};
constexpr const char* MainBoardToKey(cfg::gEnum::MainBoard val) throw() {
  switch (val) {
    case cfg::gEnum::MainBoard::OTHER: return "other";
    case cfg::gEnum::MainBoard::PI_ZERO_W2: return "pi_zero_w2";
    case cfg::gEnum::MainBoard::PI_4: return "pi_4";
  }
  __builtin_unreachable();
}

enum class ProcedureType {
  TESTING,
  PRE_LAUNCH,
  ASCENT,
  DESCENT,
  RECOVERY,
  FAILSAFE,
  OTHER
};
static std::unordered_map<std::string, cfg::gEnum::ProcedureType> const KeyToProcedureType = {
  {"testing", cfg::gEnum::ProcedureType::TESTING},
  {"pre_launch", cfg::gEnum::ProcedureType::PRE_LAUNCH},
  {"ascent", cfg::gEnum::ProcedureType::ASCENT},
  {"descent", cfg::gEnum::ProcedureType::DESCENT},
  {"recovery", cfg::gEnum::ProcedureType::RECOVERY},
  {"failsafe", cfg::gEnum::ProcedureType::FAILSAFE},
  {"other", cfg::gEnum::ProcedureType::OTHER}
};
constexpr const char* ProcedureTypeToKey(cfg::gEnum::ProcedureType val) throw() {
  switch (val) {
    case cfg::gEnum::ProcedureType::TESTING: return "testing";
    case cfg::gEnum::ProcedureType::PRE_LAUNCH: return "pre_launch";
    case cfg::gEnum::ProcedureType::ASCENT: return "ascent";
    case cfg::gEnum::ProcedureType::DESCENT: return "descent";
    case cfg::gEnum::ProcedureType::RECOVERY: return "recovery";
    case cfg::gEnum::ProcedureType::FAILSAFE: return "failsafe";
    case cfg::gEnum::ProcedureType::OTHER: return "other";
  }
  __builtin_unreachable();
}

enum class LogStrategy {
  INTERVAL,
  ALL,
  SELECTION_INTERVAL,
  SELECTION_ALL
};
static std::unordered_map<std::string, cfg::gEnum::LogStrategy> const KeyToLogStrategy = {
  {"interval", cfg::gEnum::LogStrategy::INTERVAL},
  {"all", cfg::gEnum::LogStrategy::ALL},
  {"selection_interval", cfg::gEnum::LogStrategy::SELECTION_INTERVAL},
  {"selection_all", cfg::gEnum::LogStrategy::SELECTION_ALL}
};
constexpr const char* LogStrategyToKey(cfg::gEnum::LogStrategy val) throw() {
  switch (val) {
    case cfg::gEnum::LogStrategy::INTERVAL: return "interval";
    case cfg::gEnum::LogStrategy::ALL: return "all";
    case cfg::gEnum::LogStrategy::SELECTION_INTERVAL: return "selection_interval";
    case cfg::gEnum::LogStrategy::SELECTION_ALL: return "selection_all";
  }
  __builtin_unreachable();
}

enum class LogDetail {
  FULL,
  PARTIAL
};
static std::unordered_map<std::string, cfg::gEnum::LogDetail> const KeyToLogDetail = {
  {"full", cfg::gEnum::LogDetail::FULL},
  {"partial", cfg::gEnum::LogDetail::PARTIAL}
};
constexpr const char* LogDetailToKey(cfg::gEnum::LogDetail val) throw() {
  switch (val) {
    case cfg::gEnum::LogDetail::FULL: return "full";
    case cfg::gEnum::LogDetail::PARTIAL: return "partial";
  }
  __builtin_unreachable();
}

enum class ArchiveMethod {
  PLAIN_TEXT
};
static std::unordered_map<std::string, cfg::gEnum::ArchiveMethod> const KeyToArchiveMethod = {
  {"plain_text", cfg::gEnum::ArchiveMethod::PLAIN_TEXT}
};
constexpr const char* ArchiveMethodToKey(cfg::gEnum::ArchiveMethod val) throw() {
  switch (val) {
    case cfg::gEnum::ArchiveMethod::PLAIN_TEXT: return "plain_text";
  }
  __builtin_unreachable();
}

enum class InfluxdbRetentionPolicy {
  HOUR,
  DAY,
  WEEK,
  MONTH,
  YEAR,
  INF
};
static std::unordered_map<std::string, cfg::gEnum::InfluxdbRetentionPolicy> const KeyToInfluxdbRetentionPolicy = {
  {"hour", cfg::gEnum::InfluxdbRetentionPolicy::HOUR},
  {"day", cfg::gEnum::InfluxdbRetentionPolicy::DAY},
  {"week", cfg::gEnum::InfluxdbRetentionPolicy::WEEK},
  {"month", cfg::gEnum::InfluxdbRetentionPolicy::MONTH},
  {"year", cfg::gEnum::InfluxdbRetentionPolicy::YEAR},
  {"inf", cfg::gEnum::InfluxdbRetentionPolicy::INF}
};
constexpr const char* InfluxdbRetentionPolicyToKey(cfg::gEnum::InfluxdbRetentionPolicy val) throw() {
  switch (val) {
    case cfg::gEnum::InfluxdbRetentionPolicy::HOUR: return "hour";
    case cfg::gEnum::InfluxdbRetentionPolicy::DAY: return "day";
    case cfg::gEnum::InfluxdbRetentionPolicy::WEEK: return "week";
    case cfg::gEnum::InfluxdbRetentionPolicy::MONTH: return "month";
    case cfg::gEnum::InfluxdbRetentionPolicy::YEAR: return "year";
    case cfg::gEnum::InfluxdbRetentionPolicy::INF: return "inf";
  }
  __builtin_unreachable();
}

enum class LogLevel {
  INFO,
  WARN,
  ERROR
};
static std::unordered_map<std::string, cfg::gEnum::LogLevel> const KeyToLogLevel = {
  {"info", cfg::gEnum::LogLevel::INFO},
  {"warn", cfg::gEnum::LogLevel::WARN},
  {"error", cfg::gEnum::LogLevel::ERROR}
};
constexpr const char* LogLevelToKey(cfg::gEnum::LogLevel val) throw() {
  switch (val) {
    case cfg::gEnum::LogLevel::INFO: return "info";
    case cfg::gEnum::LogLevel::WARN: return "warn";
    case cfg::gEnum::LogLevel::ERROR: return "error";
  }
  __builtin_unreachable();
}

enum class AprsSymbolTable {
  PRIMARY,
  ALTERNATE
};
static std::unordered_map<std::string, cfg::gEnum::AprsSymbolTable> const KeyToAprsSymbolTable = {
  {"primary", cfg::gEnum::AprsSymbolTable::PRIMARY},
  {"alternate", cfg::gEnum::AprsSymbolTable::ALTERNATE}
};
constexpr const char* AprsSymbolTableToKey(cfg::gEnum::AprsSymbolTable val) throw() {
  switch (val) {
    case cfg::gEnum::AprsSymbolTable::PRIMARY: return "primary";
    case cfg::gEnum::AprsSymbolTable::ALTERNATE: return "alternate";
  }
  __builtin_unreachable();
}

} // namespace gEnum

class CfgSection {
 public:
  CfgSection(data::Streams &streams): streams_(streams){}
  
 protected:
  mutable std::mutex cfg_lock_ = std::mutex();
  data::Streams &streams_;
};

class General : public cfg::CfgSection {
public:
  General(data::Streams &streams): cfg::CfgSection(streams){}

  std::string getProjectName() const;
  cfg::gEnum::MainBoard getMainBoard() const;
  cfg::gEnum::ProcedureType getStartingProcedure() const;

  void setProjectName(std::string);
  void setMainBoard(cfg::gEnum::MainBoard);
  void setStartingProcedure(cfg::gEnum::ProcedureType);

  void setFromJson(const json&);
  json getJson() const;

private:
  std::string project_name_ = "Giraffe Flight 1";
  cfg::gEnum::MainBoard main_board_ = cfg::gEnum::MainBoard::OTHER;
  cfg::gEnum::ProcedureType starting_procedure_ = cfg::gEnum::ProcedureType::OTHER;
};

class DataModuleGeneral : public cfg::CfgSection {
public:
  DataModuleGeneral(data::Streams &streams): cfg::CfgSection(streams){}

  int getFramePurgeTime() const;

  void setFramePurgeTime(int);

  void setFromJson(const json&);
  json getJson() const;

private:
  int frame_purge_time_ = 30000;
};

class DataModuleDataLog : public cfg::CfgSection {
public:
  DataModuleDataLog(data::Streams &streams): cfg::CfgSection(streams){}

  bool getLogDataToFile() const;
  cfg::gEnum::LogStrategy getLogStrategy() const;
  cfg::gEnum::LogDetail getLogDetail() const;
  int getLogIntervalMs() const;
  int getMaxDataLogFileSizeMb() const;
  int getMaxDataArchiveSizeMb() const;
  cfg::gEnum::ArchiveMethod getArchiveMethod() const;

  void setLogDataToFile(bool);
  void setLogStrategy(cfg::gEnum::LogStrategy);
  void setLogDetail(cfg::gEnum::LogDetail);
  void setLogIntervalMs(int);
  void setMaxDataLogFileSizeMb(int);
  void setMaxDataArchiveSizeMb(int);
  void setArchiveMethod(cfg::gEnum::ArchiveMethod);

  void setFromJson(const json&);
  json getJson() const;

private:
  bool log_data_to_file_ = true;
  cfg::gEnum::LogStrategy log_strategy_ = cfg::gEnum::LogStrategy::INTERVAL;
  cfg::gEnum::LogDetail log_detail_ = cfg::gEnum::LogDetail::FULL;
  int log_interval_ms_ = 5000;
  int max_data_log_file_size_mb_ = 10;
  int max_data_archive_size_mb_ = 100;
  cfg::gEnum::ArchiveMethod archive_method_ = cfg::gEnum::ArchiveMethod::PLAIN_TEXT;
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

  void setInfluxEnabled(bool);
  void setLogErrors(bool);
  void setUrl(std::string);
  void setToken(std::string);
  void setOrganization(std::string);
  void setDataBucket(std::string);
  void setErrorBucket(std::string);
  void setRetentionPolicy(cfg::gEnum::InfluxdbRetentionPolicy);

  void setFromJson(const json&);
  json getJson() const;

private:
  bool influx_enabled_ = false;
  bool log_errors_ = false;
  std::string url_ = "localhost";
  std::string token_ = "none";
  std::string organization_ = "giraffe";
  std::string data_bucket_ = "gfs_data";
  std::string error_bucket_ = "gfs_errors";
  cfg::gEnum::InfluxdbRetentionPolicy retention_policy_ = cfg::gEnum::InfluxdbRetentionPolicy::INF;
};

class DataModuleErrorLog : public cfg::CfgSection {
public:
  DataModuleErrorLog(data::Streams &streams): cfg::CfgSection(streams){}

  bool getLogErrorsToFile() const;
  int getMaxErrorLogFileSizeMb() const;
  int getMaxErrorArchiveSizeMb() const;
  cfg::gEnum::ArchiveMethod getErrorArchiveMethod() const;

  void setLogErrorsToFile(bool);
  void setMaxErrorLogFileSizeMb(int);
  void setMaxErrorArchiveSizeMb(int);
  void setErrorArchiveMethod(cfg::gEnum::ArchiveMethod);

  void setFromJson(const json&);
  json getJson() const;

private:
  bool log_errors_to_file_ = true;
  int max_error_log_file_size_mb_ = 10;
  int max_error_archive_size_mb_ = 50;
  cfg::gEnum::ArchiveMethod error_archive_method_ = cfg::gEnum::ArchiveMethod::PLAIN_TEXT;
};

class DataModuleDebug : public cfg::CfgSection {
public:
  DataModuleDebug(data::Streams &streams): cfg::CfgSection(streams){}

  bool getEnabled() const;
  cfg::gEnum::LogLevel getLogLevel() const;

  void setEnabled(bool);
  void setLogLevel(cfg::gEnum::LogLevel);

  void setFromJson(const json&);
  json getJson() const;

private:
  bool enabled_ = false;
  cfg::gEnum::LogLevel log_level_ = cfg::gEnum::LogLevel::INFO;
};

class ConsoleModule : public cfg::CfgSection {
public:
  ConsoleModule(data::Streams &streams): cfg::CfgSection(streams){}

  bool getEnabled() const;
  int getUpdateInterval() const;

  void setEnabled(bool);
  void setUpdateInterval(int);

  void setFromJson(const json&);
  json getJson() const;

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

  void setFromJson(const json&);
  json getJson() const;

private:
  bool enabled_ = false;
  int tcp_socket_port_ = 7893;
};

class SystemModule : public cfg::CfgSection {
public:
  SystemModule(data::Streams &streams): cfg::CfgSection(streams){}

  bool getEnabled() const;
  int getSystemInfoPollRateMs() const;

  void setEnabled(bool);
  void setSystemInfoPollRateMs(int);

  void setFromJson(const json&);
  json getJson() const;

private:
  bool enabled_ = true;
  int system_info_poll_rate_ms_ = 1000;
};

class Telemetry : public cfg::CfgSection {
public:
  Telemetry(data::Streams &streams): cfg::CfgSection(streams){}

  bool getTelemetryEnabled() const;
  std::string getCallSign() const;

  void setTelemetryEnabled(bool);
  void setCallSign(std::string);

  void setFromJson(const json&);
  json getJson() const;

private:
  bool telemetry_enabled_ = false;
  std::string call_sign_ = "N0CALL";
};

class TelemetryAprs : public cfg::CfgSection {
public:
  TelemetryAprs(data::Streams &streams): cfg::CfgSection(streams){}

  bool getTelemetryPackets() const;
  bool getPositionPackets() const;
  std::string getFrequency() const;
  int getSsid() const;
  std::string getDestinationAddress() const;
  int getDestinationSsid() const;
  cfg::gEnum::AprsSymbolTable getSymbolTable() const;
  std::string getSymbol() const;
  std::string getComment() const;

  void setTelemetryPackets(bool);
  void setPositionPackets(bool);
  void setFrequency(std::string);
  void setSsid(int);
  void setDestinationAddress(std::string);
  void setDestinationSsid(int);
  void setSymbolTable(cfg::gEnum::AprsSymbolTable);
  void setSymbol(std::string);
  void setComment(std::string);

  void setFromJson(const json&);
  json getJson() const;

private:
  bool telemetry_packets_ = false;
  bool position_packets_ = false;
  std::string frequency_ = "144.3900";
  int ssid_ = 0;
  std::string destination_address_ = "APRS";
  int destination_ssid_ = 0;
  cfg::gEnum::AprsSymbolTable symbol_table_ = cfg::gEnum::AprsSymbolTable::PRIMARY;
  std::string symbol_ = "/";
  std::string comment_ = "Giraffe Flight Software";
};

class Configuration {
 public:
  Configuration(data::Streams &streams):
    general(streams),    data_module_general(streams),    data_module_data_log(streams),    data_module_influxdb(streams),    data_module_error_log(streams),    data_module_debug(streams),    console_module(streams),    server_module(streams),    system_module(streams),    telemetry(streams),    telemetry_aprs(streams),
    streams_(streams){}
    
    void save(std::string file_path = "");
    void load(std::string file_path = "");

  cfg::General general;
  cfg::DataModuleGeneral data_module_general;
  cfg::DataModuleDataLog data_module_data_log;
  cfg::DataModuleInfluxdb data_module_influxdb;
  cfg::DataModuleErrorLog data_module_error_log;
  cfg::DataModuleDebug data_module_debug;
  cfg::ConsoleModule console_module;
  cfg::ServerModule server_module;
  cfg::SystemModule system_module;
  cfg::Telemetry telemetry;
  cfg::TelemetryAprs telemetry_aprs;
 
 private:
  void error(data::logId error_code, std::string info = "") {
      streams_.log.error(node::Identification::CONFIGURATION, error_code, info);
  }
 
  data::Streams &streams_;
  std::mutex file_lock_ = std::mutex();
};

} // namespace cfg

#endif // CONFIGURATION_HPP_

// * * * AUTOMATICALLY GENERATED WITH CMake/Python * * *
