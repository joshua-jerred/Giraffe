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
bool mainBoard(const std::string &main_board);
};  // namespace general
};  // namespace validator

};  // namespace cfg

#endif