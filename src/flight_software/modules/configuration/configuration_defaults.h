/**
 * @file configuration_defaults.h
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief Default configuration values.
 * @date 2023-05-10
 * @copyright Copyright (c) 2023
 */

#ifndef CONFIGURATION_DEFAULTS_H_
#define CONFIGURATION_DEFAULTS_H_

#include "configuration_structures.h"

namespace cfg {
namespace defaults {
namespace general {
const std::string project_name = "def name";
inline constexpr cfg::General::MainBoard main_board = cfg::General::MainBoard::OTHER;
};  // namespace general
};  // namespace defaults
};  // namespace cfg

#endif