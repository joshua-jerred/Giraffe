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

namespace cfg {
namespace gEnum {

enum class MainBoard {
  OTHER,
  PI_ZERO_W2,
  PI_4
};
std::unordered_map<std::string, cfg::gEnum::MainBoard> const KeyToMainBoard = {
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
std::unordered_map<std::string, cfg::gEnum::ProcedureType> const KeyToProcedureType = {
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
std::unordered_map<std::string, cfg::gEnum::LogStrategy> const KeyToLogStrategy = {
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

enum class TimestampDetail {
  HHMMSS,
  DATE_HHMMSS
};
std::unordered_map<std::string, cfg::gEnum::TimestampDetail> const KeyToTimestampDetail = {
  {"hhmmss", cfg::gEnum::TimestampDetail::HHMMSS},
  {"date_hhmmss", cfg::gEnum::TimestampDetail::DATE_HHMMSS}
};
constexpr const char* TimestampDetailToKey(cfg::gEnum::TimestampDetail val) throw() {
  switch (val) {
    case cfg::gEnum::TimestampDetail::HHMMSS: return "hhmmss";
    case cfg::gEnum::TimestampDetail::DATE_HHMMSS: return "date_hhmmss";
  }
  __builtin_unreachable();
}

enum class TimestampTimezone {
  SYSTEM,
  UTC
};
std::unordered_map<std::string, cfg::gEnum::TimestampTimezone> const KeyToTimestampTimezone = {
  {"system", cfg::gEnum::TimestampTimezone::SYSTEM},
  {"utc", cfg::gEnum::TimestampTimezone::UTC}
};
constexpr const char* TimestampTimezoneToKey(cfg::gEnum::TimestampTimezone val) throw() {
  switch (val) {
    case cfg::gEnum::TimestampTimezone::SYSTEM: return "system";
    case cfg::gEnum::TimestampTimezone::UTC: return "utc";
  }
  __builtin_unreachable();
}

enum class ArchiveMethod {
  PLAIN_TEXT
};
std::unordered_map<std::string, cfg::gEnum::ArchiveMethod> const KeyToArchiveMethod = {
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
std::unordered_map<std::string, cfg::gEnum::InfluxdbRetentionPolicy> const KeyToInfluxdbRetentionPolicy = {
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
std::unordered_map<std::string, cfg::gEnum::LogLevel> const KeyToLogLevel = {
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

enum class ErrorLogStrategy {
  ALL,
  ERROR_FRAME
};
std::unordered_map<std::string, cfg::gEnum::ErrorLogStrategy> const KeyToErrorLogStrategy = {
  {"all", cfg::gEnum::ErrorLogStrategy::ALL},
  {"error_frame", cfg::gEnum::ErrorLogStrategy::ERROR_FRAME}
};
constexpr const char* ErrorLogStrategyToKey(cfg::gEnum::ErrorLogStrategy val) throw() {
  switch (val) {
    case cfg::gEnum::ErrorLogStrategy::ALL: return "all";
    case cfg::gEnum::ErrorLogStrategy::ERROR_FRAME: return "error_frame";
  }
  __builtin_unreachable();
}

enum class AprsSymbolTable {
  PRIMARY,
  ALTERNATE
};
std::unordered_map<std::string, cfg::gEnum::AprsSymbolTable> const KeyToAprsSymbolTable = {
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

enum class SstvMode {
  ROBOT36
};
std::unordered_map<std::string, cfg::gEnum::SstvMode> const KeyToSstvMode = {
  {"robot36", cfg::gEnum::SstvMode::ROBOT36}
};
constexpr const char* SstvModeToKey(cfg::gEnum::SstvMode val) throw() {
  switch (val) {
    case cfg::gEnum::SstvMode::ROBOT36: return "robot36";
  }
  __builtin_unreachable();
}

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
std::unordered_map<std::string, cfg::gEnum::DataPacketsMode> const KeyToDataPacketsMode = {
  {"bpsk125", cfg::gEnum::DataPacketsMode::BPSK125},
  {"bpsk250", cfg::gEnum::DataPacketsMode::BPSK250},
  {"bpsk500", cfg::gEnum::DataPacketsMode::BPSK500},
  {"bpsk1000", cfg::gEnum::DataPacketsMode::BPSK1000},
  {"qpsk125", cfg::gEnum::DataPacketsMode::QPSK125},
  {"qpsk250", cfg::gEnum::DataPacketsMode::QPSK250},
  {"qpsk500", cfg::gEnum::DataPacketsMode::QPSK500},
  {"afsk_ax25", cfg::gEnum::DataPacketsMode::AFSK_AX25}
};
constexpr const char* DataPacketsModeToKey(cfg::gEnum::DataPacketsMode val) throw() {
  switch (val) {
    case cfg::gEnum::DataPacketsMode::BPSK125: return "bpsk125";
    case cfg::gEnum::DataPacketsMode::BPSK250: return "bpsk250";
    case cfg::gEnum::DataPacketsMode::BPSK500: return "bpsk500";
    case cfg::gEnum::DataPacketsMode::BPSK1000: return "bpsk1000";
    case cfg::gEnum::DataPacketsMode::QPSK125: return "qpsk125";
    case cfg::gEnum::DataPacketsMode::QPSK250: return "qpsk250";
    case cfg::gEnum::DataPacketsMode::QPSK500: return "qpsk500";
    case cfg::gEnum::DataPacketsMode::AFSK_AX25: return "afsk_ax25";
  }
  __builtin_unreachable();
}

enum class ExtensionType {
  UNKNOWN,
  SIM_TEMP,
  SIM_PRES,
  SIM_HUM,
  SIM_GPS,
  SIM_BAT,
  SIM_IMU,
  BMP180,
  BME280,
  SAM_M8Q,
  DS18B20
};
std::unordered_map<std::string, cfg::gEnum::ExtensionType> const KeyToExtensionType = {
  {"unknown", cfg::gEnum::ExtensionType::UNKNOWN},
  {"sim_temp", cfg::gEnum::ExtensionType::SIM_TEMP},
  {"sim_pres", cfg::gEnum::ExtensionType::SIM_PRES},
  {"sim_hum", cfg::gEnum::ExtensionType::SIM_HUM},
  {"sim_gps", cfg::gEnum::ExtensionType::SIM_GPS},
  {"sim_bat", cfg::gEnum::ExtensionType::SIM_BAT},
  {"sim_imu", cfg::gEnum::ExtensionType::SIM_IMU},
  {"bmp180", cfg::gEnum::ExtensionType::BMP180},
  {"bme280", cfg::gEnum::ExtensionType::BME280},
  {"sam_m8q", cfg::gEnum::ExtensionType::SAM_M8Q},
  {"ds18b20", cfg::gEnum::ExtensionType::DS18B20}
};
constexpr const char* ExtensionTypeToKey(cfg::gEnum::ExtensionType val) throw() {
  switch (val) {
    case cfg::gEnum::ExtensionType::UNKNOWN: return "unknown";
    case cfg::gEnum::ExtensionType::SIM_TEMP: return "sim_temp";
    case cfg::gEnum::ExtensionType::SIM_PRES: return "sim_pres";
    case cfg::gEnum::ExtensionType::SIM_HUM: return "sim_hum";
    case cfg::gEnum::ExtensionType::SIM_GPS: return "sim_gps";
    case cfg::gEnum::ExtensionType::SIM_BAT: return "sim_bat";
    case cfg::gEnum::ExtensionType::SIM_IMU: return "sim_imu";
    case cfg::gEnum::ExtensionType::BMP180: return "bmp180";
    case cfg::gEnum::ExtensionType::BME280: return "bme280";
    case cfg::gEnum::ExtensionType::SAM_M8Q: return "sam_m8q";
    case cfg::gEnum::ExtensionType::DS18B20: return "ds18b20";
  }
  __builtin_unreachable();
}

enum class I2CBus {
  I2C_1
};
std::unordered_map<std::string, cfg::gEnum::I2CBus> const KeyToI2CBus = {
  {"i2c_1", cfg::gEnum::I2CBus::I2C_1}
};
constexpr const char* I2CBusToKey(cfg::gEnum::I2CBus val) throw() {
  switch (val) {
    case cfg::gEnum::I2CBus::I2C_1: return "i2c_1";
  }
  __builtin_unreachable();
}

} // namespace gEnum

} // namespace cfg


#endif // CONFIGURATION_ENUMS_HPP_


/**
 * @endverbatim
 * 
 * &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
 * AUTOMATICALLY GENERATED, DO NOT EDIT MANUALLY
 * &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
 */