/**
 * @file configurables.h
 * @author Joshua Jerred (https://joshuajer.red/)
 * @brief Configurable constants.
 * @version 0.1
 * @date 2022-10-02
 * @copyright Copyright (c) 2022
 */

#ifndef CONFIGURABLES_HPP_
#define CONFIGURABLES_HPP_

#include <string>

namespace configurables {

namespace file_paths {
static const std::string kDataLogLocation = "./data_logs/";
static const std::string kErrorLogLocation = "./error_logs/";
static const std::string kTelemetryWavLocation = "./telemetry/";
static const std::string kImagesLocation = "./images/";
} // namespace file_paths
} // namespace configurables

#endif