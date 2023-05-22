/**
 * @file configuration_internal.h
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief Internal functionality for the configuration module.
 * @date 2023-05-13
 * @copyright Copyright (c) 2023
 */

#ifndef CONFIGURATION_INTERNAL_H_
#define CONFIGURATION_INTERNAL_H_

#include <map>
#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>

#include "configuration.h"
#include "gfs_exception.h"
#include "streams.h"

namespace cfg {

namespace json {
using json = nlohmann::ordered_json;

json allToJson(cfg::Configuration &);
json generalToJson(const cfg::General &);

json dataModuleGeneralToJson(const cfg::DataModuleGeneral &);
json dataModuleDataLogToJson(const cfg::DataModuleDataLog &);
json dataModuleInfluxDbToJson(const cfg::DataModuleInfluxDb &);
json dataModuleErrorLogToJson(const cfg::DataModuleErrorLog &);
json dataModuleDebugToJson(const cfg::DataModuleDebug &);

json consoleModuleToJson(const cfg::ConsoleModule &);
json serverModuleToJson(const cfg::ServerModule &);
json systemModuleToJson(const cfg::SystemModule &);

json telemetryToJson(const cfg::Telemetry &telemetry_section);
json aprsToJson(const cfg::Aprs &aprs_section);
json sstvToJson(const cfg::Sstv &sstv_section);
json dataPacketsToJson(const cfg::DataPackets &data_packets_section);

void jsonToGeneral(const json &json_data, cfg::General &general,
                   data::ErrorStream &es, int &num_errors);

void jsonToDataModuleGeneral(const json &json_data,
                             cfg::DataModuleGeneral &data_module_general,
                             data::ErrorStream &es, int &num_errors);

void jsonToDataModuleDataLog(const json &json_data,
                             cfg::DataModuleDataLog &data_module_data_log,
                             data::ErrorStream &es, int &num_errors);

void jsonToDataModuleInfluxDb(const json &json_data,
                              cfg::DataModuleInfluxDb &data_module_influx_db,
                              data::ErrorStream &es, int &num_errors);

void jsonToDataModuleErrorLog(const json &json_data,
                              cfg::DataModuleErrorLog &data_module_error_log,
                              data::ErrorStream &es, int &num_errors);

void jsonToDataModuleDebug(const json &json_data,
                           cfg::DataModuleDebug &data_module_debug,
                           data::ErrorStream &es, int &num_errors);

void jsonToConsoleModule(const json &json_data, cfg::ConsoleModule &console,
                         data::ErrorStream &es, int &num_errors);

void jsonToServerModule(const json &json_data, cfg::ServerModule &server,
                        data::ErrorStream &es, int &num_errors);

void jsonToSystemModule(const json &json_data, cfg::SystemModule &system,
                        data::ErrorStream &es, int &num_errors);

void jsonToTelemetry(const json &json_data, cfg::Telemetry &telemetry,
                     data::ErrorStream &es, int &num_errors);

void jsonToAprs(const json &json_data, cfg::Aprs &aprs, data::ErrorStream &es,
                int &num_errors);

void jsonToSstv(const json &json_data, cfg::Sstv &sstv, data::ErrorStream &es,
                int &num_errors);

void jsonToDataPackets(const json &json_data, cfg::DataPackets &data_packets,
                       data::ErrorStream &es, int &num_errors);

}  // namespace json

namespace general {
cfg::General defaultGeneral();

namespace validators {
bool projectName(const std::string &project_name, std::string &error);
bool mainBoard(const std::string &main_board, std::string &error);
bool startingProcedure(const std::string &starting_procedure,
                       std::string &error);
}  // namespace validators
};  // namespace general

namespace dm_general {
namespace validators {
bool framePurgeTime(const int, std::string &error);
}  // namespace validators
};  // namespace data_module_general

namespace dm_data_log {
namespace validators {
bool logStrategy(const std::string &, std::string &error);
bool logDetail(const std::string &, std::string &error);
bool logInterval(const int, std::string &error);
bool maxFileSize(const int, std::string &error);
bool maxArchiveSize(const int, std::string &error);
bool archiveMethod(const std::string &, std::string &error);
}  // namespace validators
}  // namespace data_module_data_log

namespace dm_influx_db {
namespace validators {
bool url(const std::string &, std::string &error);
bool token(const std::string &, std::string &error);
bool org(const std::string &, std::string &error);
bool bucket(const std::string &, std::string &error);
bool retentionPolicy(const std::string &, std::string &error);
}  // namespace validators
}  // namespace data_module_influx_db

namespace dm_debug {
namespace validators {
bool debugLevel(const std::string &, std::string &error);
}
}  // namespace data_module_debug

namespace console_module {
namespace validators {
bool updateInterval(const int, std::string &error);
}
}  // namespace console_module

namespace server_module {
namespace validators {
bool tcpSocketPort(const int, std::string &error);
}
}  // namespace server_module

namespace system_module {
namespace validators {
bool systemInfoPollRate(const int, std::string &error);
}
}  // namespace system_module

namespace telemetry {
cfg::Telemetry defaultTelemetry();

namespace validators {
bool callSign(const std::string &call_sign, std::string &error);
bool frequency(const std::string &frequency, std::string &error);
}  // namespace validators
}  // namespace telemetry

namespace aprs {
cfg::Aprs defaultAprs();

namespace validators {
bool ssid(const int ssid, std::string &error);
bool destinationAddress(const std::string &address, std::string &error);
bool symbol(const std::string &symbol, std::string &error);
bool symbolTable(const std::string &symbol, std::string &error);
bool comment(const std::string &comment, std::string &error);
}  // namespace validators
}  // namespace aprs

namespace sstv {
cfg::Sstv defaultSstv();

namespace validators {
bool mode(const std::string &mode, std::string &error);
// bool dataOverlayContents(..., std::string &error);
}  // namespace validators
}  // namespace sstv

namespace data_packets {
cfg::DataPackets defaultDataPackets();

namespace validators {
bool mode(const std::string &mode, std::string &error);
bool comment(const std::string &mode, std::string &error);
}  // namespace validators
}  // namespace data_packets

const std::map<cfg::Procedure::Type, std::string> kProcedureTypeToString = {
    {cfg::Procedure::Type::TESTING, "testing"},
    {cfg::Procedure::Type::ASCENT, "ascent"},
    {cfg::Procedure::Type::DESCENT, "descent"},
    {cfg::Procedure::Type::RECOVERY, "recovery"},
    {cfg::Procedure::Type::FAILSAFE, "failsafe"}};

const std::unordered_map<std::string, cfg::Procedure::Type>
    kStringToProcedureType = {{"testing", cfg::Procedure::Type::TESTING},
                              {"ascent", cfg::Procedure::Type::ASCENT},
                              {"descent", cfg::Procedure::Type::DESCENT},
                              {"recovery", cfg::Procedure::Type::RECOVERY},
                              {"failsafe", cfg::Procedure::Type::FAILSAFE}};

const std::map<cfg::General::MainBoard, std::string> kMainBoardToString = {
    {cfg::General::MainBoard::OTHER, "other"},
    {cfg::General::MainBoard::PI_4, "pi_4"},
    {cfg::General::MainBoard::PI_ZERO_W_2, "pi_zero_w_2"}};

const std::unordered_map<std::string, cfg::General::MainBoard>
    kStringToMainBoard = {
        {"other", cfg::General::MainBoard::OTHER},
        {"pi_4", cfg::General::MainBoard::PI_4},
        {"pi_zero_w_2", cfg::General::MainBoard::PI_ZERO_W_2}};

const std::map<cfg::DataModuleDataLog::LogStrategy, std::string>
    kLogStrategyToString = {
        {cfg::DataModuleDataLog::LogStrategy::INTERVAL, "interval"},
        {cfg::DataModuleDataLog::LogStrategy::ALL, "all"},
        {cfg::DataModuleDataLog::LogStrategy::SELECTION_INTERVAL,
         "selection_interval"},
        {cfg::DataModuleDataLog::LogStrategy::SELECTION_ALL, "selection_all"}};


const std::unordered_map<std::string, cfg::DataModuleDataLog::LogStrategy>
    kStringToLogStrategy = {
        {"interval", cfg::DataModuleDataLog::LogStrategy::INTERVAL},
        {"all", cfg::DataModuleDataLog::LogStrategy::ALL},
        {"selection_interval",
         cfg::DataModuleDataLog::LogStrategy::SELECTION_INTERVAL},
        {"selection_all", cfg::DataModuleDataLog::LogStrategy::SELECTION_ALL}};

const std::map<cfg::DataModuleDataLog::LogDetail, std::string>
    kLogDetailToString = {
        {cfg::DataModuleDataLog::LogDetail::FULL, "full"},
        {cfg::DataModuleDataLog::LogDetail::PARTIAL, "partial"}};

const std::unordered_map<std::string, cfg::DataModuleDataLog::LogDetail>
    kStringToLogDetail = {
        {"full", cfg::DataModuleDataLog::LogDetail::FULL},
        {"partial", cfg::DataModuleDataLog::LogDetail::PARTIAL}};

const std::map<cfg::ArchivalMethod, std::string> kArchivalMethodToString = {
    {cfg::ArchivalMethod::PLAIN_TEXT, "plain_text"},
};

const std::unordered_map<std::string, cfg::ArchivalMethod>
    kStringToArchivalMethod = {
        {"plain_text", cfg::ArchivalMethod::PLAIN_TEXT},
};

const std::map<cfg::DataModuleInfluxDb::RetentionPolicy, std::string>
    kRetentionPolicyToString = {
        {cfg::DataModuleInfluxDb::RetentionPolicy::HOUR, "1 Hour"},
        {cfg::DataModuleInfluxDb::RetentionPolicy::DAY, "1 Day"},
        {cfg::DataModuleInfluxDb::RetentionPolicy::WEEK, "1 Week"},
        {cfg::DataModuleInfluxDb::RetentionPolicy::MONTH, "1 Month"},
        {cfg::DataModuleInfluxDb::RetentionPolicy::YEAR, "1 Year"},
        {cfg::DataModuleInfluxDb::RetentionPolicy::INF, "INF"}};

const std::unordered_map<std::string, cfg::DataModuleInfluxDb::RetentionPolicy>
    kStringToRetentionPolicy = {
        {"1 Hour", cfg::DataModuleInfluxDb::RetentionPolicy::HOUR},
        {"1 Day", cfg::DataModuleInfluxDb::RetentionPolicy::DAY},
        {"1 Week", cfg::DataModuleInfluxDb::RetentionPolicy::WEEK},
        {"1 Month", cfg::DataModuleInfluxDb::RetentionPolicy::MONTH},
        {"1 Year", cfg::DataModuleInfluxDb::RetentionPolicy::YEAR},
        {"INF", cfg::DataModuleInfluxDb::RetentionPolicy::INF}};

const std::map<cfg::DataModuleDebug::LogLevel, std::string> kLogLevelToString =
    {{cfg::DataModuleDebug::LogLevel::INFO, "info"},
     {cfg::DataModuleDebug::LogLevel::WARN, "warn"},
     {cfg::DataModuleDebug::LogLevel::ERROR, "error"}};

const std::unordered_map<std::string, cfg::DataModuleDebug::LogLevel>
    kStringToLogLevel = {{"info", cfg::DataModuleDebug::LogLevel::INFO},
                         {"warn", cfg::DataModuleDebug::LogLevel::WARN},
                         {"error", cfg::DataModuleDebug::LogLevel::ERROR}};

const std::map<cfg::Aprs::SymbolTable, std::string> kAprsSymbolTableToString = {
    {cfg::Aprs::SymbolTable::PRIMARY, "primary"},
    {cfg::Aprs::SymbolTable::ALTERNATE, "alternate"}};

const std::unordered_map<std::string, cfg::Aprs::SymbolTable>
    kStringToAprsSymbolTable = {
        {"primary", cfg::Aprs::SymbolTable::PRIMARY},
        {"alternate", cfg::Aprs::SymbolTable::ALTERNATE}};

const std::map<cfg::Sstv::Mode, std::string> kSstvModeToString = {
    {cfg::Sstv::Mode::ROBOT_36, "robot36"}};

const std::unordered_map<std::string, cfg::Sstv::Mode> kStringToSstvMode = {
    {"robot36", cfg::Sstv::Mode::ROBOT_36}};

const std::map<cfg::DataPackets::Mode, std::string> kDataPacketsModeToString = {
    {cfg::DataPackets::Mode::BPSK125, "bpsk125"},
    {cfg::DataPackets::Mode::BPSK250, "bpsk250"},
    {cfg::DataPackets::Mode::BPSK500, "bpsk500"},
    {cfg::DataPackets::Mode::BPSK1000, "bpsk1000"},
    {cfg::DataPackets::Mode::QPSK125, "qpsk125"},
    {cfg::DataPackets::Mode::QPSK250, "qpsk250"},
    {cfg::DataPackets::Mode::QPSK500, "qpsk500"},
    {cfg::DataPackets::Mode::AFSK_AX25, "afsk_ax25"}};

const std::unordered_map<std::string, cfg::DataPackets::Mode>
    kStringToDataPacketsMode = {
        {"bpsk125", cfg::DataPackets::Mode::BPSK125},
        {"bpsk250", cfg::DataPackets::Mode::BPSK250},
        {"bpsk500", cfg::DataPackets::Mode::BPSK500},
        {"bpsk1000", cfg::DataPackets::Mode::BPSK1000},
        {"qpsk125", cfg::DataPackets::Mode::QPSK125},
        {"qpsk250", cfg::DataPackets::Mode::QPSK250},
        {"qpsk500", cfg::DataPackets::Mode::QPSK500},
        {"afsk_ax25", cfg::DataPackets::Mode::AFSK_AX25}};

inline void reportError(data::ErrorStream &es, const std::string code,
                        const std::string info) {
  es.addError(node::Identification::CONFIGURATION, code, info);
}
};  // namespace cfg

#endif