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

class ConfigurationException : public GfsException {
 public:
  ConfigurationException(std::string error_code, std::string info)
      : GfsException("M_CFG", error_code, info) {
  }
};

namespace file {
void saveConfiguration(data::ErrorStream &es, cfg::Configuration &config,
                       const std::string &file_path, bool overwrite = true);
void loadConfiguration(data::ErrorStream &es, cfg::Configuration &config,
                       const std::string &file_path);
}  // namespace file

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

namespace defaults {
const std::string project_name = "Giraffe 1";
inline constexpr cfg::General::MainBoard main_board =
    cfg::General::MainBoard::OTHER;
inline constexpr cfg::Procedure::Type starting_procedure =
    cfg::Procedure::Type::FAILSAFE;
}  // namespace defaults

namespace validators {
bool projectName(const std::string &project_name, std::string &error);
bool mainBoard(const std::string &main_board, std::string &error);
bool startingProcedure(const std::string &starting_procedure,
                       std::string &error);
}  // namespace validators
};  // namespace general

namespace debug {
namespace defaults {
inline constexpr bool print_errors = false;
inline constexpr bool console_enabled = false;
inline constexpr int console_update_interval = 1000;
}  // namespace defaults

namespace validators {
bool consoleUpdateInterval(const int update_interval_ms, std::string &error);
}  // namespace validators
};  // namespace debug

namespace server {
namespace defaults {
inline constexpr int tcp_socket_port = 7893;
}
namespace validators {
bool tcpSocketPort(const int port_number, std::string &error);
}
}  // namespace server

namespace telemetry {
namespace defaults {
inline constexpr bool telemetry_enabled = false;
const std::string call_sign = "N0CALL";
}  // namespace defaults

namespace validators {
bool callSign(const std::string &call_sign, std::string &error);
bool frequency(const std::string &frequency, std::string &error);
}  // namespace validators
}  // namespace telemetry

namespace aprs {
namespace defaults {
inline constexpr bool telemetry_packets = false;
inline constexpr bool position_packets = false;
const std::string frequency = "144.3900";
inline constexpr int ssid = 0;
const std::string destination_address = "APRS";
inline constexpr int destination_ssid = 0;
inline constexpr cfg::Aprs::SymbolTable symbol_table =
    cfg::Aprs::SymbolTable::PRIMARY;
inline constexpr char symbol = '/';
const std::string comment = "Giraffe Flight Software";
}  // namespace defaults

namespace validators {
bool ssid(const int ssid, std::string &error);
bool destinationAddress(const std::string &address, std::string &error);
bool symbol(const std::string &symbol, std::string &error);
bool symbolTable(const std::string &symbol, std::string &error);
bool comment(const std::string &comment, std::string &error);
}  // namespace validators
}  // namespace aprs

namespace sstv {
namespace defaults {
inline constexpr bool enabled = false;
const std::string frequency = "145.5100";
inline constexpr cfg::Sstv::Mode mode = cfg::Sstv::Mode::ROBOT_36;
inline constexpr bool overlay_data = true;

}  // namespace defaults

namespace validators {
bool mode(const std::string &mode, std::string &error);
// bool dataOverlayContents(..., std::string &error);
}  // namespace validators
}  // namespace sstv

namespace data_packets {
namespace defaults {
inline constexpr bool enabled = false;
const std::string frequency = "145.5100";
inline constexpr cfg::DataPackets::Mode mode = cfg::DataPackets::Mode::BPSK250;
inline constexpr bool morse_call_sign = true;
const std::string comment = "Giraffe Flight Software";
}  // namespace defaults

namespace validators {
bool mode(const std::string &mode, std::string &error);
bool comment(const std::string &mode, std::string &error);
}  // namespace validators
}  // namespace data_packets

};  // namespace cfg

#endif