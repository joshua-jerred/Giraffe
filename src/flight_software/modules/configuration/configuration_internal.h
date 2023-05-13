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
}

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

// namespace telemetry {
// bool callSign(const std::string &call_sign);
// bool frequency(const std::string &frequency);

// bool aprsSsid(const int ssid);
// bool aprsAddress(const int address);
// bool aprsSymbol(const std::string &symbol);
// bool aprsComment(const std::string &comment);

// bool sstvMode(const std::string &comment);
// };
};  // namespace cfg

#endif