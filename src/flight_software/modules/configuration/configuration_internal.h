/**
 * @file configuration_internal.h
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief Internal functionality for the configuration module.
 * @date 2023-05-13
 * @copyright Copyright (c) 2023
 */

#ifndef CONFIGURATION_INTERNAL_H_
#define CONFIGURATION_INTERNAL_H_

#include <string>

#include "configuration.h"

namespace cfg {

namespace file {
  bool saveConfiguration(const cfg::Configuration &config, const std::string &file_path);
}

namespace general {

namespace defaults {
const std::string project_name = "Giraffe 1";
inline constexpr cfg::General::MainBoard main_board = cfg::General::MainBoard::OTHER;
inline constexpr cfg::Procedure::Type starting_procedure = cfg::Procedure::Type::FAILSAFE;
}  // namespace defaults

namespace validators {
bool projectName(const std::string &project_name, std::string &error);
bool mainBoard(const std::string &main_board, std::string &error);
bool startingProcedure(const std::string &starting_procedure, std::string &error);
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
inline constexpr cfg::Aprs::SymbolTable symbol_table = cfg::Aprs::SymbolTable::PRIMARY;
inline constexpr char symbol = '/';
const std::string comment = "Giraffe Flight Software";
}  // namespace defaults

namespace validators {
bool ssid(const int ssid, std::string &error);
bool destinationAddress(const std::string &address, std::string &error);
bool symbol(const std::string &symbol, std::string &error);
bool comment(const std::string &comment, std::string &error); 
}
}  // namespace telemetry_aprs

namespace sstv {
namespace defaults {
inline constexpr bool enabled = false;
const std::string frequency = "145.5100";
inline constexpr cfg::Sstv::Mode mode = cfg::Sstv::Mode::ROBOT_36;
inline constexpr bool overlay_data = true;

}  // namespace defaults

namespace validators {
bool mode(const std::string &mode, std::string &error);
//bool dataOverlayContents(..., std::string &error);
}
}  // namespace telemetry_aprs

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
}
}  // namespace telemetry_aprs

};  // namespace cfg

#endif