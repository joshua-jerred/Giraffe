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


#include <filesystem>
#include "configuration.hpp"
#include "validation.hpp"


std::string cfg::General::getProjectName() const {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  return project_name_;
}

cfg::gEnum::MainBoard cfg::General::getMainBoard() const {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  return main_board_;
}

cfg::gEnum::ProcedureType cfg::General::getStartingProcedure() const {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  return starting_procedure_;
}

int cfg::General::getModuleStatusUpdateRate() const {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  return module_status_update_rate_;
}

void cfg::General::setProjectName(std::string val) {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  project_name_ = val;
}

void cfg::General::setMainBoard(cfg::gEnum::MainBoard val) {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  main_board_ = val;
}

void cfg::General::setStartingProcedure(cfg::gEnum::ProcedureType val) {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  starting_procedure_ = val;
}

void cfg::General::setModuleStatusUpdateRate(int val) {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  module_status_update_rate_ = val;
}

void cfg::General::setFromJson(const Json &json_data) {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  validation::setValidValue<std::string>(
        streams_.log,
        json_data,
        "general",
        "project_name",
        project_name_,
        1,
        20,
        "^(?!\\s)[a-zA-Z0-9_ -]{0,19}[^\\s]$"
  );
  validation::setValidEnum<cfg::gEnum::MainBoard>(
        streams_.log,
        json_data,
        "general",
        "main_board",
        main_board_,
        cfg::gEnum::K_STRING_TO_MAIN_BOARD_MAP
  );
  validation::setValidEnum<cfg::gEnum::ProcedureType>(
        streams_.log,
        json_data,
        "general",
        "starting_procedure",
        starting_procedure_,
        cfg::gEnum::K_STRING_TO_PROCEDURE_TYPE_MAP
  );
  validation::setValidValue<int>(
        streams_.log,
        json_data,
        "general",
        "module_status_update_rate",
        module_status_update_rate_,
        50,
        60000,
        ""
  );
}

Json cfg::General::getJson() const {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  return Json({
    {"project_name", project_name_},
    {"main_board", cfg::gEnum::K_MAIN_BOARD_TO_STRING_MAP.at(main_board_)},
    {"starting_procedure", cfg::gEnum::K_PROCEDURE_TYPE_TO_STRING_MAP.at(starting_procedure_)},
    {"module_status_update_rate", module_status_update_rate_}
  });
}
bool cfg::DataModuleData::getLogDataToFile() const {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  return log_data_to_file_;
}

int cfg::DataModuleData::getFileSystemCheckInterval() const {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  return file_system_check_interval_;
}

cfg::gEnum::LogStrategy cfg::DataModuleData::getLogStrategy() const {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  return log_strategy_;
}

cfg::gEnum::TimestampDetail cfg::DataModuleData::getTimestampDetail() const {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  return timestamp_detail_;
}

cfg::gEnum::TimestampTimezone cfg::DataModuleData::getTimestampTimezone() const {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  return timestamp_timezone_;
}

int cfg::DataModuleData::getLogIntervalMs() const {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  return log_interval_ms_;
}

int cfg::DataModuleData::getMaxDataLogFileSizeMb() const {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  return max_data_log_file_size_mb_;
}

int cfg::DataModuleData::getMaxDataArchiveSizeMb() const {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  return max_data_archive_size_mb_;
}

bool cfg::DataModuleData::getWarnOnTrim() const {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  return warn_on_trim_;
}

cfg::gEnum::ArchiveMethod cfg::DataModuleData::getArchiveMethod() const {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  return archive_method_;
}

std::string cfg::DataModuleData::getDataLogFileContents() const {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  return data_log_file_contents_;
}

void cfg::DataModuleData::setLogDataToFile(bool val) {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  log_data_to_file_ = val;
}

void cfg::DataModuleData::setFileSystemCheckInterval(int val) {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  file_system_check_interval_ = val;
}

void cfg::DataModuleData::setLogStrategy(cfg::gEnum::LogStrategy val) {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  log_strategy_ = val;
}

void cfg::DataModuleData::setTimestampDetail(cfg::gEnum::TimestampDetail val) {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  timestamp_detail_ = val;
}

void cfg::DataModuleData::setTimestampTimezone(cfg::gEnum::TimestampTimezone val) {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  timestamp_timezone_ = val;
}

void cfg::DataModuleData::setLogIntervalMs(int val) {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  log_interval_ms_ = val;
}

void cfg::DataModuleData::setMaxDataLogFileSizeMb(int val) {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  max_data_log_file_size_mb_ = val;
}

void cfg::DataModuleData::setMaxDataArchiveSizeMb(int val) {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  max_data_archive_size_mb_ = val;
}

void cfg::DataModuleData::setWarnOnTrim(bool val) {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  warn_on_trim_ = val;
}

void cfg::DataModuleData::setArchiveMethod(cfg::gEnum::ArchiveMethod val) {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  archive_method_ = val;
}

void cfg::DataModuleData::setDataLogFileContents(std::string val) {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  data_log_file_contents_ = val;
}

void cfg::DataModuleData::setFromJson(const Json &json_data) {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  validation::setValidValue<bool>(
        streams_.log,
        json_data,
        "data_module_data",
        "log_data_to_file",
        log_data_to_file_,
        0,
        0,
        ""
  );
  validation::setValidValue<int>(
        streams_.log,
        json_data,
        "data_module_data",
        "file_system_check_interval",
        file_system_check_interval_,
        500,
        60000,
        ""
  );
  validation::setValidEnum<cfg::gEnum::LogStrategy>(
        streams_.log,
        json_data,
        "data_module_data",
        "log_strategy",
        log_strategy_,
        cfg::gEnum::K_STRING_TO_LOG_STRATEGY_MAP
  );
  validation::setValidEnum<cfg::gEnum::TimestampDetail>(
        streams_.log,
        json_data,
        "data_module_data",
        "timestamp_detail",
        timestamp_detail_,
        cfg::gEnum::K_STRING_TO_TIMESTAMP_DETAIL_MAP
  );
  validation::setValidEnum<cfg::gEnum::TimestampTimezone>(
        streams_.log,
        json_data,
        "data_module_data",
        "timestamp_timezone",
        timestamp_timezone_,
        cfg::gEnum::K_STRING_TO_TIMESTAMP_TIMEZONE_MAP
  );
  validation::setValidValue<int>(
        streams_.log,
        json_data,
        "data_module_data",
        "log_interval_ms",
        log_interval_ms_,
        100,
        600000,
        ""
  );
  validation::setValidValue<int>(
        streams_.log,
        json_data,
        "data_module_data",
        "max_data_log_file_size_mb",
        max_data_log_file_size_mb_,
        1,
        500,
        ""
  );
  validation::setValidValue<int>(
        streams_.log,
        json_data,
        "data_module_data",
        "max_data_archive_size_mb",
        max_data_archive_size_mb_,
        50,
        100000,
        ""
  );
  validation::setValidValue<bool>(
        streams_.log,
        json_data,
        "data_module_data",
        "warn_on_trim",
        warn_on_trim_,
        0,
        0,
        ""
  );
  validation::setValidEnum<cfg::gEnum::ArchiveMethod>(
        streams_.log,
        json_data,
        "data_module_data",
        "archive_method",
        archive_method_,
        cfg::gEnum::K_STRING_TO_ARCHIVE_METHOD_MAP
  );
  validation::setValidValue<std::string>(
        streams_.log,
        json_data,
        "data_module_data",
        "data_log_file_contents",
        data_log_file_contents_,
        0,
        50,
        ""
  );
}

Json cfg::DataModuleData::getJson() const {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  return Json({
    {"log_data_to_file", log_data_to_file_},
    {"file_system_check_interval", file_system_check_interval_},
    {"log_strategy", cfg::gEnum::K_LOG_STRATEGY_TO_STRING_MAP.at(log_strategy_)},
    {"timestamp_detail", cfg::gEnum::K_TIMESTAMP_DETAIL_TO_STRING_MAP.at(timestamp_detail_)},
    {"timestamp_timezone", cfg::gEnum::K_TIMESTAMP_TIMEZONE_TO_STRING_MAP.at(timestamp_timezone_)},
    {"log_interval_ms", log_interval_ms_},
    {"max_data_log_file_size_mb", max_data_log_file_size_mb_},
    {"max_data_archive_size_mb", max_data_archive_size_mb_},
    {"warn_on_trim", warn_on_trim_},
    {"archive_method", cfg::gEnum::K_ARCHIVE_METHOD_TO_STRING_MAP.at(archive_method_)},
    {"data_log_file_contents", data_log_file_contents_}
  });
}
bool cfg::DataModuleInfluxdb::getInfluxEnabled() const {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  return influx_enabled_;
}

bool cfg::DataModuleInfluxdb::getLogErrors() const {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  return log_errors_;
}

std::string cfg::DataModuleInfluxdb::getUrl() const {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  return url_;
}

std::string cfg::DataModuleInfluxdb::getToken() const {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  return token_;
}

std::string cfg::DataModuleInfluxdb::getOrganization() const {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  return organization_;
}

std::string cfg::DataModuleInfluxdb::getDataBucket() const {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  return data_bucket_;
}

std::string cfg::DataModuleInfluxdb::getErrorBucket() const {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  return error_bucket_;
}

cfg::gEnum::InfluxdbRetentionPolicy cfg::DataModuleInfluxdb::getRetentionPolicy() const {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  return retention_policy_;
}

std::string cfg::DataModuleInfluxdb::getContents() const {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  return contents_;
}

void cfg::DataModuleInfluxdb::setInfluxEnabled(bool val) {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  influx_enabled_ = val;
}

void cfg::DataModuleInfluxdb::setLogErrors(bool val) {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  log_errors_ = val;
}

void cfg::DataModuleInfluxdb::setUrl(std::string val) {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  url_ = val;
}

void cfg::DataModuleInfluxdb::setToken(std::string val) {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  token_ = val;
}

void cfg::DataModuleInfluxdb::setOrganization(std::string val) {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  organization_ = val;
}

void cfg::DataModuleInfluxdb::setDataBucket(std::string val) {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  data_bucket_ = val;
}

void cfg::DataModuleInfluxdb::setErrorBucket(std::string val) {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  error_bucket_ = val;
}

void cfg::DataModuleInfluxdb::setRetentionPolicy(cfg::gEnum::InfluxdbRetentionPolicy val) {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  retention_policy_ = val;
}

void cfg::DataModuleInfluxdb::setContents(std::string val) {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  contents_ = val;
}

void cfg::DataModuleInfluxdb::setFromJson(const Json &json_data) {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  validation::setValidValue<bool>(
        streams_.log,
        json_data,
        "data_module_influxdb",
        "influx_enabled",
        influx_enabled_,
        0,
        0,
        ""
  );
  validation::setValidValue<bool>(
        streams_.log,
        json_data,
        "data_module_influxdb",
        "log_errors",
        log_errors_,
        0,
        0,
        ""
  );
  validation::setValidValue<std::string>(
        streams_.log,
        json_data,
        "data_module_influxdb",
        "url",
        url_,
        0,
        50,
        ""
  );
  validation::setValidValue<std::string>(
        streams_.log,
        json_data,
        "data_module_influxdb",
        "token",
        token_,
        0,
        50,
        ""
  );
  validation::setValidValue<std::string>(
        streams_.log,
        json_data,
        "data_module_influxdb",
        "organization",
        organization_,
        0,
        50,
        ""
  );
  validation::setValidValue<std::string>(
        streams_.log,
        json_data,
        "data_module_influxdb",
        "data_bucket",
        data_bucket_,
        0,
        50,
        ""
  );
  validation::setValidValue<std::string>(
        streams_.log,
        json_data,
        "data_module_influxdb",
        "error_bucket",
        error_bucket_,
        0,
        50,
        ""
  );
  validation::setValidEnum<cfg::gEnum::InfluxdbRetentionPolicy>(
        streams_.log,
        json_data,
        "data_module_influxdb",
        "retention_policy",
        retention_policy_,
        cfg::gEnum::K_STRING_TO_INFLUXDB_RETENTION_POLICY_MAP
  );
  validation::setValidValue<std::string>(
        streams_.log,
        json_data,
        "data_module_influxdb",
        "contents",
        contents_,
        0,
        50,
        ""
  );
}

Json cfg::DataModuleInfluxdb::getJson() const {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  return Json({
    {"influx_enabled", influx_enabled_},
    {"log_errors", log_errors_},
    {"url", url_},
    {"token", token_},
    {"organization", organization_},
    {"data_bucket", data_bucket_},
    {"error_bucket", error_bucket_},
    {"retention_policy", cfg::gEnum::K_INFLUXDB_RETENTION_POLICY_TO_STRING_MAP.at(retention_policy_)},
    {"contents", contents_}
  });
}
bool cfg::DataModuleLog::getLogToFile() const {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  return log_to_file_;
}

int cfg::DataModuleLog::getMaxLogFileSizeMb() const {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  return max_log_file_size_mb_;
}

int cfg::DataModuleLog::getMaxLogArchiveSizeMb() const {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  return max_log_archive_size_mb_;
}

cfg::gEnum::ArchiveMethod cfg::DataModuleLog::getErrorArchiveMethod() const {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  return error_archive_method_;
}

cfg::gEnum::LogLevel cfg::DataModuleLog::getLogLevel() const {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  return log_level_;
}

cfg::gEnum::ErrorLogStrategy cfg::DataModuleLog::getErrorLogStrategy() const {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  return error_log_strategy_;
}

int cfg::DataModuleLog::getErrorFrameLogInterval() const {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  return error_frame_log_interval_;
}

void cfg::DataModuleLog::setLogToFile(bool val) {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  log_to_file_ = val;
}

void cfg::DataModuleLog::setMaxLogFileSizeMb(int val) {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  max_log_file_size_mb_ = val;
}

void cfg::DataModuleLog::setMaxLogArchiveSizeMb(int val) {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  max_log_archive_size_mb_ = val;
}

void cfg::DataModuleLog::setErrorArchiveMethod(cfg::gEnum::ArchiveMethod val) {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  error_archive_method_ = val;
}

void cfg::DataModuleLog::setLogLevel(cfg::gEnum::LogLevel val) {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  log_level_ = val;
}

void cfg::DataModuleLog::setErrorLogStrategy(cfg::gEnum::ErrorLogStrategy val) {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  error_log_strategy_ = val;
}

void cfg::DataModuleLog::setErrorFrameLogInterval(int val) {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  error_frame_log_interval_ = val;
}

void cfg::DataModuleLog::setFromJson(const Json &json_data) {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  validation::setValidValue<bool>(
        streams_.log,
        json_data,
        "data_module_log",
        "log_to_file",
        log_to_file_,
        0,
        0,
        ""
  );
  validation::setValidValue<int>(
        streams_.log,
        json_data,
        "data_module_log",
        "max_log_file_size_mb",
        max_log_file_size_mb_,
        1,
        500,
        ""
  );
  validation::setValidValue<int>(
        streams_.log,
        json_data,
        "data_module_log",
        "max_log_archive_size_mb",
        max_log_archive_size_mb_,
        50,
        100000,
        ""
  );
  validation::setValidEnum<cfg::gEnum::ArchiveMethod>(
        streams_.log,
        json_data,
        "data_module_log",
        "error_archive_method",
        error_archive_method_,
        cfg::gEnum::K_STRING_TO_ARCHIVE_METHOD_MAP
  );
  validation::setValidEnum<cfg::gEnum::LogLevel>(
        streams_.log,
        json_data,
        "data_module_log",
        "log_level",
        log_level_,
        cfg::gEnum::K_STRING_TO_LOG_LEVEL_MAP
  );
  validation::setValidEnum<cfg::gEnum::ErrorLogStrategy>(
        streams_.log,
        json_data,
        "data_module_log",
        "error_log_strategy",
        error_log_strategy_,
        cfg::gEnum::K_STRING_TO_ERROR_LOG_STRATEGY_MAP
  );
  validation::setValidValue<int>(
        streams_.log,
        json_data,
        "data_module_log",
        "error_frame_log_interval",
        error_frame_log_interval_,
        500,
        600000,
        ""
  );
}

Json cfg::DataModuleLog::getJson() const {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  return Json({
    {"log_to_file", log_to_file_},
    {"max_log_file_size_mb", max_log_file_size_mb_},
    {"max_log_archive_size_mb", max_log_archive_size_mb_},
    {"error_archive_method", cfg::gEnum::K_ARCHIVE_METHOD_TO_STRING_MAP.at(error_archive_method_)},
    {"log_level", cfg::gEnum::K_LOG_LEVEL_TO_STRING_MAP.at(log_level_)},
    {"error_log_strategy", cfg::gEnum::K_ERROR_LOG_STRATEGY_TO_STRING_MAP.at(error_log_strategy_)},
    {"error_frame_log_interval", error_frame_log_interval_}
  });
}
bool cfg::ConsoleModule::getEnabled() const {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  return enabled_;
}

int cfg::ConsoleModule::getUpdateInterval() const {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  return update_interval_;
}

void cfg::ConsoleModule::setEnabled(bool val) {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  enabled_ = val;
}

void cfg::ConsoleModule::setUpdateInterval(int val) {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  update_interval_ = val;
}

void cfg::ConsoleModule::setFromJson(const Json &json_data) {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  validation::setValidValue<bool>(
        streams_.log,
        json_data,
        "console_module",
        "enabled",
        enabled_,
        0,
        0,
        ""
  );
  validation::setValidValue<int>(
        streams_.log,
        json_data,
        "console_module",
        "update_interval",
        update_interval_,
        100,
        10000,
        ""
  );
}

Json cfg::ConsoleModule::getJson() const {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  return Json({
    {"enabled", enabled_},
    {"update_interval", update_interval_}
  });
}
bool cfg::ServerModule::getEnabled() const {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  return enabled_;
}

int cfg::ServerModule::getTcpSocketPort() const {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  return tcp_socket_port_;
}

void cfg::ServerModule::setEnabled(bool val) {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  enabled_ = val;
}

void cfg::ServerModule::setTcpSocketPort(int val) {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  tcp_socket_port_ = val;
}

void cfg::ServerModule::setFromJson(const Json &json_data) {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  validation::setValidValue<bool>(
        streams_.log,
        json_data,
        "server_module",
        "enabled",
        enabled_,
        0,
        0,
        ""
  );
  validation::setValidValue<int>(
        streams_.log,
        json_data,
        "server_module",
        "tcp_socket_port",
        tcp_socket_port_,
        1024,
        65535,
        ""
  );
}

Json cfg::ServerModule::getJson() const {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  return Json({
    {"enabled", enabled_},
    {"tcp_socket_port", tcp_socket_port_}
  });
}
bool cfg::SystemModule::getEnabled() const {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  return enabled_;
}

int cfg::SystemModule::getSystemInfoPollRateMs() const {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  return system_info_poll_rate_ms_;
}

void cfg::SystemModule::setEnabled(bool val) {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  enabled_ = val;
}

void cfg::SystemModule::setSystemInfoPollRateMs(int val) {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  system_info_poll_rate_ms_ = val;
}

void cfg::SystemModule::setFromJson(const Json &json_data) {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  validation::setValidValue<bool>(
        streams_.log,
        json_data,
        "system_module",
        "enabled",
        enabled_,
        0,
        0,
        ""
  );
  validation::setValidValue<int>(
        streams_.log,
        json_data,
        "system_module",
        "system_info_poll_rate_ms",
        system_info_poll_rate_ms_,
        100,
        600000,
        ""
  );
}

Json cfg::SystemModule::getJson() const {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  return Json({
    {"enabled", enabled_},
    {"system_info_poll_rate_ms", system_info_poll_rate_ms_}
  });
}
bool cfg::Telemetry::getTelemetryEnabled() const {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  return telemetry_enabled_;
}

std::string cfg::Telemetry::getCallSign() const {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  return call_sign_;
}

void cfg::Telemetry::setTelemetryEnabled(bool val) {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  telemetry_enabled_ = val;
}

void cfg::Telemetry::setCallSign(std::string val) {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  call_sign_ = val;
}

void cfg::Telemetry::setFromJson(const Json &json_data) {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  validation::setValidValue<bool>(
        streams_.log,
        json_data,
        "telemetry",
        "telemetry_enabled",
        telemetry_enabled_,
        0,
        0,
        ""
  );
  validation::setValidValue<std::string>(
        streams_.log,
        json_data,
        "telemetry",
        "call_sign",
        call_sign_,
        3,
        6,
        "[a-zA-Z0-9]{1,3}[0-9][a-zA-Z0-9]{0,3}[a-zA-Z]"
  );
}

Json cfg::Telemetry::getJson() const {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  return Json({
    {"telemetry_enabled", telemetry_enabled_},
    {"call_sign", call_sign_}
  });
}
bool cfg::TelemetryAprs::getTelemetryPackets() const {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  return telemetry_packets_;
}

bool cfg::TelemetryAprs::getPositionPackets() const {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  return position_packets_;
}

std::string cfg::TelemetryAprs::getFrequency() const {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  return frequency_;
}

int cfg::TelemetryAprs::getSsid() const {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  return ssid_;
}

std::string cfg::TelemetryAprs::getDestinationAddress() const {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  return destination_address_;
}

int cfg::TelemetryAprs::getDestinationSsid() const {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  return destination_ssid_;
}

cfg::gEnum::AprsSymbolTable cfg::TelemetryAprs::getSymbolTable() const {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  return symbol_table_;
}

std::string cfg::TelemetryAprs::getSymbol() const {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  return symbol_;
}

std::string cfg::TelemetryAprs::getComment() const {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  return comment_;
}

void cfg::TelemetryAprs::setTelemetryPackets(bool val) {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  telemetry_packets_ = val;
}

void cfg::TelemetryAprs::setPositionPackets(bool val) {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  position_packets_ = val;
}

void cfg::TelemetryAprs::setFrequency(std::string val) {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  frequency_ = val;
}

void cfg::TelemetryAprs::setSsid(int val) {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  ssid_ = val;
}

void cfg::TelemetryAprs::setDestinationAddress(std::string val) {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  destination_address_ = val;
}

void cfg::TelemetryAprs::setDestinationSsid(int val) {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  destination_ssid_ = val;
}

void cfg::TelemetryAprs::setSymbolTable(cfg::gEnum::AprsSymbolTable val) {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  symbol_table_ = val;
}

void cfg::TelemetryAprs::setSymbol(std::string val) {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  symbol_ = val;
}

void cfg::TelemetryAprs::setComment(std::string val) {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  comment_ = val;
}

void cfg::TelemetryAprs::setFromJson(const Json &json_data) {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  validation::setValidValue<bool>(
        streams_.log,
        json_data,
        "telemetry_aprs",
        "telemetry_packets",
        telemetry_packets_,
        0,
        0,
        ""
  );
  validation::setValidValue<bool>(
        streams_.log,
        json_data,
        "telemetry_aprs",
        "position_packets",
        position_packets_,
        0,
        0,
        ""
  );
  validation::setValidValue<std::string>(
        streams_.log,
        json_data,
        "telemetry_aprs",
        "frequency",
        frequency_,
        8,
        8,
        "[0-9]{3}.[0-9]{4}"
  );
  validation::setValidValue<int>(
        streams_.log,
        json_data,
        "telemetry_aprs",
        "ssid",
        ssid_,
        0,
        15,
        ""
  );
  validation::setValidValue<std::string>(
        streams_.log,
        json_data,
        "telemetry_aprs",
        "destination_address",
        destination_address_,
        3,
        6,
        ""
  );
  validation::setValidValue<int>(
        streams_.log,
        json_data,
        "telemetry_aprs",
        "destination_ssid",
        destination_ssid_,
        0,
        15,
        ""
  );
  validation::setValidEnum<cfg::gEnum::AprsSymbolTable>(
        streams_.log,
        json_data,
        "telemetry_aprs",
        "symbol_table",
        symbol_table_,
        cfg::gEnum::K_STRING_TO_APRS_SYMBOL_TABLE_MAP
  );
  validation::setValidValue<std::string>(
        streams_.log,
        json_data,
        "telemetry_aprs",
        "symbol",
        symbol_,
        1,
        1,
        ""
  );
  validation::setValidValue<std::string>(
        streams_.log,
        json_data,
        "telemetry_aprs",
        "comment",
        comment_,
        0,
        43,
        ""
  );
}

Json cfg::TelemetryAprs::getJson() const {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  return Json({
    {"telemetry_packets", telemetry_packets_},
    {"position_packets", position_packets_},
    {"frequency", frequency_},
    {"ssid", ssid_},
    {"destination_address", destination_address_},
    {"destination_ssid", destination_ssid_},
    {"symbol_table", cfg::gEnum::K_APRS_SYMBOL_TABLE_TO_STRING_MAP.at(symbol_table_)},
    {"symbol", symbol_},
    {"comment", comment_}
  });
}
bool cfg::TelemetrySstv::getEnabled() const {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  return enabled_;
}

std::string cfg::TelemetrySstv::getFrequency() const {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  return frequency_;
}

cfg::gEnum::SstvMode cfg::TelemetrySstv::getMode() const {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  return mode_;
}

bool cfg::TelemetrySstv::getOverlayData() const {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  return overlay_data_;
}

void cfg::TelemetrySstv::setEnabled(bool val) {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  enabled_ = val;
}

void cfg::TelemetrySstv::setFrequency(std::string val) {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  frequency_ = val;
}

void cfg::TelemetrySstv::setMode(cfg::gEnum::SstvMode val) {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  mode_ = val;
}

void cfg::TelemetrySstv::setOverlayData(bool val) {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  overlay_data_ = val;
}

void cfg::TelemetrySstv::setFromJson(const Json &json_data) {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  validation::setValidValue<bool>(
        streams_.log,
        json_data,
        "telemetry_sstv",
        "enabled",
        enabled_,
        0,
        0,
        ""
  );
  validation::setValidValue<std::string>(
        streams_.log,
        json_data,
        "telemetry_sstv",
        "frequency",
        frequency_,
        8,
        8,
        "[0-9]{3}.[0-9]{4}"
  );
  validation::setValidEnum<cfg::gEnum::SstvMode>(
        streams_.log,
        json_data,
        "telemetry_sstv",
        "mode",
        mode_,
        cfg::gEnum::K_STRING_TO_SSTV_MODE_MAP
  );
  validation::setValidValue<bool>(
        streams_.log,
        json_data,
        "telemetry_sstv",
        "overlay_data",
        overlay_data_,
        0,
        0,
        ""
  );
}

Json cfg::TelemetrySstv::getJson() const {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  return Json({
    {"enabled", enabled_},
    {"frequency", frequency_},
    {"mode", cfg::gEnum::K_SSTV_MODE_TO_STRING_MAP.at(mode_)},
    {"overlay_data", overlay_data_}
  });
}
bool cfg::TelemetryDataPackets::getEnabled() const {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  return enabled_;
}

std::string cfg::TelemetryDataPackets::getFrequency() const {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  return frequency_;
}

cfg::gEnum::DataPacketsMode cfg::TelemetryDataPackets::getMode() const {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  return mode_;
}

bool cfg::TelemetryDataPackets::getMorseCallSign() const {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  return morse_call_sign_;
}

std::string cfg::TelemetryDataPackets::getComment() const {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  return comment_;
}

void cfg::TelemetryDataPackets::setEnabled(bool val) {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  enabled_ = val;
}

void cfg::TelemetryDataPackets::setFrequency(std::string val) {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  frequency_ = val;
}

void cfg::TelemetryDataPackets::setMode(cfg::gEnum::DataPacketsMode val) {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  mode_ = val;
}

void cfg::TelemetryDataPackets::setMorseCallSign(bool val) {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  morse_call_sign_ = val;
}

void cfg::TelemetryDataPackets::setComment(std::string val) {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  comment_ = val;
}

void cfg::TelemetryDataPackets::setFromJson(const Json &json_data) {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  validation::setValidValue<bool>(
        streams_.log,
        json_data,
        "telemetry_data_packets",
        "enabled",
        enabled_,
        0,
        0,
        ""
  );
  validation::setValidValue<std::string>(
        streams_.log,
        json_data,
        "telemetry_data_packets",
        "frequency",
        frequency_,
        8,
        8,
        "[0-9]{3}.[0-9]{4}"
  );
  validation::setValidEnum<cfg::gEnum::DataPacketsMode>(
        streams_.log,
        json_data,
        "telemetry_data_packets",
        "mode",
        mode_,
        cfg::gEnum::K_STRING_TO_DATA_PACKETS_MODE_MAP
  );
  validation::setValidValue<bool>(
        streams_.log,
        json_data,
        "telemetry_data_packets",
        "morse_call_sign",
        morse_call_sign_,
        0,
        0,
        ""
  );
  validation::setValidValue<std::string>(
        streams_.log,
        json_data,
        "telemetry_data_packets",
        "comment",
        comment_,
        0,
        200,
        ""
  );
}

Json cfg::TelemetryDataPackets::getJson() const {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  return Json({
    {"enabled", enabled_},
    {"frequency", frequency_},
    {"mode", cfg::gEnum::K_DATA_PACKETS_MODE_TO_STRING_MAP.at(mode_)},
    {"morse_call_sign", morse_call_sign_},
    {"comment", comment_}
  });
}
int cfg::ExtensionModule::getStatusPollingRate() const {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  return status_polling_rate_;
}

int cfg::ExtensionModule::getMaxRestartAttempts() const {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  return max_restart_attempts_;
}

int cfg::ExtensionModule::getRestartDelayMinimum() const {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  return restart_delay_minimum_;
}

int cfg::ExtensionModule::getStartTimeout() const {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  return start_timeout_;
}

int cfg::ExtensionModule::getMaxStartupAttempts() const {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  return max_startup_attempts_;
}

void cfg::ExtensionModule::setStatusPollingRate(int val) {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  status_polling_rate_ = val;
}

void cfg::ExtensionModule::setMaxRestartAttempts(int val) {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  max_restart_attempts_ = val;
}

void cfg::ExtensionModule::setRestartDelayMinimum(int val) {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  restart_delay_minimum_ = val;
}

void cfg::ExtensionModule::setStartTimeout(int val) {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  start_timeout_ = val;
}

void cfg::ExtensionModule::setMaxStartupAttempts(int val) {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  max_startup_attempts_ = val;
}

void cfg::ExtensionModule::setFromJson(const Json &json_data) {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  validation::setValidValue<int>(
        streams_.log,
        json_data,
        "extension_module",
        "status_polling_rate",
        status_polling_rate_,
        500,
        60000,
        ""
  );
  validation::setValidValue<int>(
        streams_.log,
        json_data,
        "extension_module",
        "max_restart_attempts",
        max_restart_attempts_,
        1,
        10,
        ""
  );
  validation::setValidValue<int>(
        streams_.log,
        json_data,
        "extension_module",
        "restart_delay_minimum",
        restart_delay_minimum_,
        500,
        60000,
        ""
  );
  validation::setValidValue<int>(
        streams_.log,
        json_data,
        "extension_module",
        "start_timeout",
        start_timeout_,
        500,
        60000,
        ""
  );
  validation::setValidValue<int>(
        streams_.log,
        json_data,
        "extension_module",
        "max_startup_attempts",
        max_startup_attempts_,
        1,
        10,
        ""
  );
}

Json cfg::ExtensionModule::getJson() const {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  return Json({
    {"status_polling_rate", status_polling_rate_},
    {"max_restart_attempts", max_restart_attempts_},
    {"restart_delay_minimum", restart_delay_minimum_},
    {"start_timeout", start_timeout_},
    {"max_startup_attempts", max_startup_attempts_}
  });
}
cfg::gEnum::I2CBus cfg::HardwareInterface::getI2CBus() const {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  return i2c_bus_;
}

void cfg::HardwareInterface::setI2CBus(cfg::gEnum::I2CBus val) {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  i2c_bus_ = val;
}

void cfg::HardwareInterface::setFromJson(const Json &json_data) {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  validation::setValidEnum<cfg::gEnum::I2CBus>(
        streams_.log,
        json_data,
        "hardware_interface",
        "i2c_bus",
        i2c_bus_,
        cfg::gEnum::K_STRING_TO_I2C_BUS_MAP
  );
}

Json cfg::HardwareInterface::getJson() const {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  return Json({
    {"i2c_bus", cfg::gEnum::K_I2C_BUS_TO_STRING_MAP.at(i2c_bus_)}
  });
}

void cfg::Configuration::getAllJson(Json &all_data) const {
  all_data = {
    {"general", general.getJson()}
,    {"data_module_data", data_module_data.getJson()}
,    {"data_module_influxdb", data_module_influxdb.getJson()}
,    {"data_module_log", data_module_log.getJson()}
,    {"console_module", console_module.getJson()}
,    {"server_module", server_module.getJson()}
,    {"system_module", system_module.getJson()}
,    {"telemetry", telemetry.getJson()}
,    {"telemetry_aprs", telemetry_aprs.getJson()}
,    {"telemetry_sstv", telemetry_sstv.getJson()}
,    {"telemetry_data_packets", telemetry_data_packets.getJson()}
,    {"extensions", extensions.getJson()}
,    {"extension_module", extension_module.getJson()}
,    {"hardware_interface", hardware_interface.getJson()}
  };
}
  
void cfg::Configuration::save(std::string file_path) {
  const std::lock_guard<std::mutex> lock(file_lock_);
  
  std::ofstream out(file_path);
  
  if (out.fail()) {
    error(DiagnosticId::CONFIG_failedToSaveToPath, file_path);
    return;
  }
  
  Json config_json;
  getAllJson(config_json);

  constexpr int json_indent = 2;
  std::string data = config_json.dump(json_indent);
  out << data;
}

inline bool sectionExists(const Json &all_data, const std::string &section_key) {
  return all_data.contains(section_key);
}

void cfg::Configuration::load(std::string file_path) {
  const std::lock_guard<std::mutex> lock(file_lock_);

  if (!std::filesystem::exists(file_path)) {
    error(DiagnosticId::CONFIG_configFileDoesNotExist, file_path);
    return; 
  }

  std::ifstream in(file_path);
  
  if (in.fail()) {
    error(DiagnosticId::CONFIG_failedToOpenConfig, file_path);
    return;
  }
  
  Json parsed;
  try {
    parsed = Json::parse(in);
  } catch (Json::parse_error &e) {
    return;
  }
  
  if (sectionExists(parsed, "general")) {
    general.setFromJson(parsed["general"]);
  } else {
    error(DiagnosticId::CONFIG_failedToLoadSectionNotFound, "general");
  }

  if (sectionExists(parsed, "data_module_data")) {
    data_module_data.setFromJson(parsed["data_module_data"]);
  } else {
    error(DiagnosticId::CONFIG_failedToLoadSectionNotFound, "data_module_data");
  }

  if (sectionExists(parsed, "data_module_influxdb")) {
    data_module_influxdb.setFromJson(parsed["data_module_influxdb"]);
  } else {
    error(DiagnosticId::CONFIG_failedToLoadSectionNotFound, "data_module_influxdb");
  }

  if (sectionExists(parsed, "data_module_log")) {
    data_module_log.setFromJson(parsed["data_module_log"]);
  } else {
    error(DiagnosticId::CONFIG_failedToLoadSectionNotFound, "data_module_log");
  }

  if (sectionExists(parsed, "console_module")) {
    console_module.setFromJson(parsed["console_module"]);
  } else {
    error(DiagnosticId::CONFIG_failedToLoadSectionNotFound, "console_module");
  }

  if (sectionExists(parsed, "server_module")) {
    server_module.setFromJson(parsed["server_module"]);
  } else {
    error(DiagnosticId::CONFIG_failedToLoadSectionNotFound, "server_module");
  }

  if (sectionExists(parsed, "system_module")) {
    system_module.setFromJson(parsed["system_module"]);
  } else {
    error(DiagnosticId::CONFIG_failedToLoadSectionNotFound, "system_module");
  }

  if (sectionExists(parsed, "telemetry")) {
    telemetry.setFromJson(parsed["telemetry"]);
  } else {
    error(DiagnosticId::CONFIG_failedToLoadSectionNotFound, "telemetry");
  }

  if (sectionExists(parsed, "telemetry_aprs")) {
    telemetry_aprs.setFromJson(parsed["telemetry_aprs"]);
  } else {
    error(DiagnosticId::CONFIG_failedToLoadSectionNotFound, "telemetry_aprs");
  }

  if (sectionExists(parsed, "telemetry_sstv")) {
    telemetry_sstv.setFromJson(parsed["telemetry_sstv"]);
  } else {
    error(DiagnosticId::CONFIG_failedToLoadSectionNotFound, "telemetry_sstv");
  }

  if (sectionExists(parsed, "telemetry_data_packets")) {
    telemetry_data_packets.setFromJson(parsed["telemetry_data_packets"]);
  } else {
    error(DiagnosticId::CONFIG_failedToLoadSectionNotFound, "telemetry_data_packets");
  }

  if (sectionExists(parsed, "extensions")) {
    extensions.setFromJson(parsed["extensions"]);
  } else {
    error(DiagnosticId::CONFIG_failedToLoadSectionNotFound, "extensions");
  }

  if (sectionExists(parsed, "extension_module")) {
    extension_module.setFromJson(parsed["extension_module"]);
  } else {
    error(DiagnosticId::CONFIG_failedToLoadSectionNotFound, "extension_module");
  }

  if (sectionExists(parsed, "hardware_interface")) {
    hardware_interface.setFromJson(parsed["hardware_interface"]);
  } else {
    error(DiagnosticId::CONFIG_failedToLoadSectionNotFound, "hardware_interface");
  }

}


/**
 * @endverbatim
 * 
 * &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
 * AUTOMATICALLY GENERATED, DO NOT EDIT MANUALLY
 * &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
 */