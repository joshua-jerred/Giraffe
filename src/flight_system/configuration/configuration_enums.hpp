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


#ifndef CONFIGURATION_ENUMS_HPP_
#define CONFIGURATION_ENUMS_HPP_

#include <string>
#include <unordered_map>
#include <map>

namespace cfg::gEnum {

enum class MainBoard {
  OTHER,
  PI_ZERO_W2,
  PI_4
};
std::unordered_map<std::string, cfg::gEnum::MainBoard> const K_STRING_TO_MAIN_BOARD_MAP = {
  {"other", cfg::gEnum::MainBoard::OTHER},
  {"pi_zero_w2", cfg::gEnum::MainBoard::PI_ZERO_W2},
  {"pi_4", cfg::gEnum::MainBoard::PI_4}
};
std::map<cfg::gEnum::MainBoard, std::string> const K_MAIN_BOARD_TO_STRING_MAP = {
  {cfg::gEnum::MainBoard::OTHER, "other"},
  {cfg::gEnum::MainBoard::PI_ZERO_W2, "pi_zero_w2"},
  {cfg::gEnum::MainBoard::PI_4, "pi_4"}
};

enum class ProcedureType {
  TESTING,
  PRE_LAUNCH,
  ASCENT,
  DESCENT,
  RECOVERY,
  FAILSAFE,
  OTHER
};
std::unordered_map<std::string, cfg::gEnum::ProcedureType> const K_STRING_TO_PROCEDURE_TYPE_MAP = {
  {"testing", cfg::gEnum::ProcedureType::TESTING},
  {"pre_launch", cfg::gEnum::ProcedureType::PRE_LAUNCH},
  {"ascent", cfg::gEnum::ProcedureType::ASCENT},
  {"descent", cfg::gEnum::ProcedureType::DESCENT},
  {"recovery", cfg::gEnum::ProcedureType::RECOVERY},
  {"failsafe", cfg::gEnum::ProcedureType::FAILSAFE},
  {"other", cfg::gEnum::ProcedureType::OTHER}
};
std::map<cfg::gEnum::ProcedureType, std::string> const K_PROCEDURE_TYPE_TO_STRING_MAP = {
  {cfg::gEnum::ProcedureType::TESTING, "testing"},
  {cfg::gEnum::ProcedureType::PRE_LAUNCH, "pre_launch"},
  {cfg::gEnum::ProcedureType::ASCENT, "ascent"},
  {cfg::gEnum::ProcedureType::DESCENT, "descent"},
  {cfg::gEnum::ProcedureType::RECOVERY, "recovery"},
  {cfg::gEnum::ProcedureType::FAILSAFE, "failsafe"},
  {cfg::gEnum::ProcedureType::OTHER, "other"}
};

enum class LogStrategy {
  INTERVAL,
  ALL,
  SELECTION_INTERVAL,
  SELECTION_ALL
};
std::unordered_map<std::string, cfg::gEnum::LogStrategy> const K_STRING_TO_LOG_STRATEGY_MAP = {
  {"interval", cfg::gEnum::LogStrategy::INTERVAL},
  {"all", cfg::gEnum::LogStrategy::ALL},
  {"selection_interval", cfg::gEnum::LogStrategy::SELECTION_INTERVAL},
  {"selection_all", cfg::gEnum::LogStrategy::SELECTION_ALL}
};
std::map<cfg::gEnum::LogStrategy, std::string> const K_LOG_STRATEGY_TO_STRING_MAP = {
  {cfg::gEnum::LogStrategy::INTERVAL, "interval"},
  {cfg::gEnum::LogStrategy::ALL, "all"},
  {cfg::gEnum::LogStrategy::SELECTION_INTERVAL, "selection_interval"},
  {cfg::gEnum::LogStrategy::SELECTION_ALL, "selection_all"}
};

enum class TimestampDetail {
  HHMMSS,
  DATE_HHMMSS
};
std::unordered_map<std::string, cfg::gEnum::TimestampDetail> const K_STRING_TO_TIMESTAMP_DETAIL_MAP = {
  {"hhmmss", cfg::gEnum::TimestampDetail::HHMMSS},
  {"date_hhmmss", cfg::gEnum::TimestampDetail::DATE_HHMMSS}
};
std::map<cfg::gEnum::TimestampDetail, std::string> const K_TIMESTAMP_DETAIL_TO_STRING_MAP = {
  {cfg::gEnum::TimestampDetail::HHMMSS, "hhmmss"},
  {cfg::gEnum::TimestampDetail::DATE_HHMMSS, "date_hhmmss"}
};

enum class TimestampTimezone {
  SYSTEM,
  UTC
};
std::unordered_map<std::string, cfg::gEnum::TimestampTimezone> const K_STRING_TO_TIMESTAMP_TIMEZONE_MAP = {
  {"system", cfg::gEnum::TimestampTimezone::SYSTEM},
  {"utc", cfg::gEnum::TimestampTimezone::UTC}
};
std::map<cfg::gEnum::TimestampTimezone, std::string> const K_TIMESTAMP_TIMEZONE_TO_STRING_MAP = {
  {cfg::gEnum::TimestampTimezone::SYSTEM, "system"},
  {cfg::gEnum::TimestampTimezone::UTC, "utc"}
};

enum class ArchiveMethod {
  PLAIN_TEXT,
  BZIP2
};
std::unordered_map<std::string, cfg::gEnum::ArchiveMethod> const K_STRING_TO_ARCHIVE_METHOD_MAP = {
  {"plain_text", cfg::gEnum::ArchiveMethod::PLAIN_TEXT},
  {"bzip2", cfg::gEnum::ArchiveMethod::BZIP2}
};
std::map<cfg::gEnum::ArchiveMethod, std::string> const K_ARCHIVE_METHOD_TO_STRING_MAP = {
  {cfg::gEnum::ArchiveMethod::PLAIN_TEXT, "plain_text"},
  {cfg::gEnum::ArchiveMethod::BZIP2, "bzip2"}
};

enum class InfluxdbRetentionPolicy {
  HOUR,
  DAY,
  WEEK,
  MONTH,
  YEAR,
  INF
};
std::unordered_map<std::string, cfg::gEnum::InfluxdbRetentionPolicy> const K_STRING_TO_INFLUXDB_RETENTION_POLICY_MAP = {
  {"hour", cfg::gEnum::InfluxdbRetentionPolicy::HOUR},
  {"day", cfg::gEnum::InfluxdbRetentionPolicy::DAY},
  {"week", cfg::gEnum::InfluxdbRetentionPolicy::WEEK},
  {"month", cfg::gEnum::InfluxdbRetentionPolicy::MONTH},
  {"year", cfg::gEnum::InfluxdbRetentionPolicy::YEAR},
  {"inf", cfg::gEnum::InfluxdbRetentionPolicy::INF}
};
std::map<cfg::gEnum::InfluxdbRetentionPolicy, std::string> const K_INFLUXDB_RETENTION_POLICY_TO_STRING_MAP = {
  {cfg::gEnum::InfluxdbRetentionPolicy::HOUR, "hour"},
  {cfg::gEnum::InfluxdbRetentionPolicy::DAY, "day"},
  {cfg::gEnum::InfluxdbRetentionPolicy::WEEK, "week"},
  {cfg::gEnum::InfluxdbRetentionPolicy::MONTH, "month"},
  {cfg::gEnum::InfluxdbRetentionPolicy::YEAR, "year"},
  {cfg::gEnum::InfluxdbRetentionPolicy::INF, "inf"}
};

enum class LogLevel {
  INFO,
  WARN,
  ERROR
};
std::unordered_map<std::string, cfg::gEnum::LogLevel> const K_STRING_TO_LOG_LEVEL_MAP = {
  {"info", cfg::gEnum::LogLevel::INFO},
  {"warn", cfg::gEnum::LogLevel::WARN},
  {"error", cfg::gEnum::LogLevel::ERROR}
};
std::map<cfg::gEnum::LogLevel, std::string> const K_LOG_LEVEL_TO_STRING_MAP = {
  {cfg::gEnum::LogLevel::INFO, "info"},
  {cfg::gEnum::LogLevel::WARN, "warn"},
  {cfg::gEnum::LogLevel::ERROR, "error"}
};

enum class ErrorLogStrategy {
  ALL,
  ERROR_FRAME
};
std::unordered_map<std::string, cfg::gEnum::ErrorLogStrategy> const K_STRING_TO_ERROR_LOG_STRATEGY_MAP = {
  {"all", cfg::gEnum::ErrorLogStrategy::ALL},
  {"error_frame", cfg::gEnum::ErrorLogStrategy::ERROR_FRAME}
};
std::map<cfg::gEnum::ErrorLogStrategy, std::string> const K_ERROR_LOG_STRATEGY_TO_STRING_MAP = {
  {cfg::gEnum::ErrorLogStrategy::ALL, "all"},
  {cfg::gEnum::ErrorLogStrategy::ERROR_FRAME, "error_frame"}
};

enum class AprsSymbolTable {
  PRIMARY,
  ALTERNATE
};
std::unordered_map<std::string, cfg::gEnum::AprsSymbolTable> const K_STRING_TO_APRS_SYMBOL_TABLE_MAP = {
  {"primary", cfg::gEnum::AprsSymbolTable::PRIMARY},
  {"alternate", cfg::gEnum::AprsSymbolTable::ALTERNATE}
};
std::map<cfg::gEnum::AprsSymbolTable, std::string> const K_APRS_SYMBOL_TABLE_TO_STRING_MAP = {
  {cfg::gEnum::AprsSymbolTable::PRIMARY, "primary"},
  {cfg::gEnum::AprsSymbolTable::ALTERNATE, "alternate"}
};

enum class SstvMode {
  ROBOT36
};
std::unordered_map<std::string, cfg::gEnum::SstvMode> const K_STRING_TO_SSTV_MODE_MAP = {
  {"robot36", cfg::gEnum::SstvMode::ROBOT36}
};
std::map<cfg::gEnum::SstvMode, std::string> const K_SSTV_MODE_TO_STRING_MAP = {
  {cfg::gEnum::SstvMode::ROBOT36, "robot36"}
};

enum class DataPacketsMode {
  BPSK125,
  BPSK250,
  BPSK500,
  BPSK1000,
  QPSK125,
  QPSK250,
  QPSK500,
  AFSK_AX25
};
std::unordered_map<std::string, cfg::gEnum::DataPacketsMode> const K_STRING_TO_DATA_PACKETS_MODE_MAP = {
  {"bpsk125", cfg::gEnum::DataPacketsMode::BPSK125},
  {"bpsk250", cfg::gEnum::DataPacketsMode::BPSK250},
  {"bpsk500", cfg::gEnum::DataPacketsMode::BPSK500},
  {"bpsk1000", cfg::gEnum::DataPacketsMode::BPSK1000},
  {"qpsk125", cfg::gEnum::DataPacketsMode::QPSK125},
  {"qpsk250", cfg::gEnum::DataPacketsMode::QPSK250},
  {"qpsk500", cfg::gEnum::DataPacketsMode::QPSK500},
  {"afsk_ax25", cfg::gEnum::DataPacketsMode::AFSK_AX25}
};
std::map<cfg::gEnum::DataPacketsMode, std::string> const K_DATA_PACKETS_MODE_TO_STRING_MAP = {
  {cfg::gEnum::DataPacketsMode::BPSK125, "bpsk125"},
  {cfg::gEnum::DataPacketsMode::BPSK250, "bpsk250"},
  {cfg::gEnum::DataPacketsMode::BPSK500, "bpsk500"},
  {cfg::gEnum::DataPacketsMode::BPSK1000, "bpsk1000"},
  {cfg::gEnum::DataPacketsMode::QPSK125, "qpsk125"},
  {cfg::gEnum::DataPacketsMode::QPSK250, "qpsk250"},
  {cfg::gEnum::DataPacketsMode::QPSK500, "qpsk500"},
  {cfg::gEnum::DataPacketsMode::AFSK_AX25, "afsk_ax25"}
};

enum class ExtensionType {
  UNKNOWN,
  SIM_TEMP,
  SIM_PRES,
  SIM_HUM,
  SIM_GPS,
  SIM_BAT,
  SIM_IMU,
  SIM_ADC,
  BMP180,
  BME280,
  SAM_M8Q,
  DS18B20,
  RGB_LED,
  MCP3021,
  MAX11615,
  BMI088
};
std::unordered_map<std::string, cfg::gEnum::ExtensionType> const K_STRING_TO_EXTENSION_TYPE_MAP = {
  {"unknown", cfg::gEnum::ExtensionType::UNKNOWN},
  {"sim_temp", cfg::gEnum::ExtensionType::SIM_TEMP},
  {"sim_pres", cfg::gEnum::ExtensionType::SIM_PRES},
  {"sim_hum", cfg::gEnum::ExtensionType::SIM_HUM},
  {"sim_gps", cfg::gEnum::ExtensionType::SIM_GPS},
  {"sim_bat", cfg::gEnum::ExtensionType::SIM_BAT},
  {"sim_imu", cfg::gEnum::ExtensionType::SIM_IMU},
  {"sim_adc", cfg::gEnum::ExtensionType::SIM_ADC},
  {"bmp180", cfg::gEnum::ExtensionType::BMP180},
  {"bme280", cfg::gEnum::ExtensionType::BME280},
  {"sam_m8q", cfg::gEnum::ExtensionType::SAM_M8Q},
  {"ds18b20", cfg::gEnum::ExtensionType::DS18B20},
  {"rgb_led", cfg::gEnum::ExtensionType::RGB_LED},
  {"mcp3021", cfg::gEnum::ExtensionType::MCP3021},
  {"max11615", cfg::gEnum::ExtensionType::MAX11615},
  {"bmi088", cfg::gEnum::ExtensionType::BMI088}
};
std::map<cfg::gEnum::ExtensionType, std::string> const K_EXTENSION_TYPE_TO_STRING_MAP = {
  {cfg::gEnum::ExtensionType::UNKNOWN, "unknown"},
  {cfg::gEnum::ExtensionType::SIM_TEMP, "sim_temp"},
  {cfg::gEnum::ExtensionType::SIM_PRES, "sim_pres"},
  {cfg::gEnum::ExtensionType::SIM_HUM, "sim_hum"},
  {cfg::gEnum::ExtensionType::SIM_GPS, "sim_gps"},
  {cfg::gEnum::ExtensionType::SIM_BAT, "sim_bat"},
  {cfg::gEnum::ExtensionType::SIM_IMU, "sim_imu"},
  {cfg::gEnum::ExtensionType::SIM_ADC, "sim_adc"},
  {cfg::gEnum::ExtensionType::BMP180, "bmp180"},
  {cfg::gEnum::ExtensionType::BME280, "bme280"},
  {cfg::gEnum::ExtensionType::SAM_M8Q, "sam_m8q"},
  {cfg::gEnum::ExtensionType::DS18B20, "ds18b20"},
  {cfg::gEnum::ExtensionType::RGB_LED, "rgb_led"},
  {cfg::gEnum::ExtensionType::MCP3021, "mcp3021"},
  {cfg::gEnum::ExtensionType::MAX11615, "max11615"},
  {cfg::gEnum::ExtensionType::BMI088, "bmi088"}
};

enum class I2CBus {
  I2C_1
};
std::unordered_map<std::string, cfg::gEnum::I2CBus> const K_STRING_TO_I2C_BUS_MAP = {
  {"i2c_1", cfg::gEnum::I2CBus::I2C_1}
};
std::map<cfg::gEnum::I2CBus, std::string> const K_I2C_BUS_TO_STRING_MAP = {
  {cfg::gEnum::I2CBus::I2C_1, "i2c_1"}
};

} // namespace cfg::gEnum


#endif // CONFIGURATION_ENUMS_HPP_


/**
 * @endverbatim
 * 
 * &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
 * AUTOMATICALLY GENERATED, DO NOT EDIT MANUALLY
 * &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
 */