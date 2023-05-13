/**
 * @file config_structure.cpp
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief Header for the 'Configuration' class.
 * @date 2023-05-09
 * @copyright Copyright (c) 2023
 */

#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_

#include <string>

namespace cfg {

namespace validator {
namespace general {
bool projectName(const std::string &project_name, std::string &error);
bool mainBoard(const std::string &main_board, std::string &error);
bool startingProcedure(const std::string &starting_procedure, std::string &error);
};  // namespace general
namespace interface {
bool consoleUpdateInterval(const int update_interval_ms, std::string &error);
bool webSocketPort(const int port_number, std::string &error);
};  // namespace interface
// namespace telemetry {
// bool callSign(const std::string &call_sign);
// bool frequency(const std::string &frequency);

// bool aprsSsid(const int ssid);
// bool aprsAddress(const int address);
// bool aprsSymbol(const std::string &symbol);
// bool aprsComment(const std::string &comment);

// bool sstvMode(const std::string &comment);
// };
};  // namespace validator
};  // namespace cfg

#endif