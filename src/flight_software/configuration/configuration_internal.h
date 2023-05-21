/**
 * @file configuration_internal.h
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief Internal functionality for the configuration module.
 * @date 2023-05-13
 * @copyright Copyright (c) 2023
 */

#ifndef CONFIGURATION_INTERNAL_H_
#define CONFIGURATION_INTERNAL_H_

#include <nlohmann/json.hpp>
#include <string>

#include "configuration.h"
#include "gfs_exception.h"
#include "streams.h"

namespace cfg {

namespace json {
using json = nlohmann::ordered_json;

json allToJson(cfg::Configuration &configuration);
json generalToJson(const cfg::General &general_section);
json debugToJson(const cfg::Debug &debug_section);
json serverToJson(const cfg::Server &server_section);
json telemetryToJson(const cfg::Telemetry &telemetry_section);
json aprsToJson(const cfg::Aprs &aprs_section);
json sstvToJson(const cfg::Sstv &sstv_section);
json dataPacketsToJson(const cfg::DataPackets &data_packets_section);

void jsonToGeneral(const json &json_data, cfg::General &general,
                   data::ErrorStream &es, int &num_errors);

void jsonToDebug(const json &json_data, cfg::Debug &debug,
                 data::ErrorStream &es, int &num_errors);

void jsonToServer(const json &json_data, cfg::Server &server,
                 data::ErrorStream &es, int &num_errors);

void jsonToTelemetry(const json &json_data, cfg::Telemetry &telemetry,
                 data::ErrorStream &es, int &num_errors);

void jsonToAprs(const json &json_data, cfg::Aprs &aprs,
                 data::ErrorStream &es, int &num_errors);

void jsonToSstv(const json &json_data, cfg::Sstv &sstv,
                 data::ErrorStream &es, int &num_errors);

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

namespace debug {
cfg::Debug defaultDebug();

namespace validators {
bool consoleUpdateInterval(const int update_interval_ms, std::string &error);
}  // namespace validators
};  // namespace debug

namespace server {
cfg::Server defaultServer();

namespace validators {
bool tcpSocketPort(const int port_number, std::string &error);
}
}  // namespace server

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

};  // namespace cfg

#endif