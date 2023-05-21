/**
 * @file configurables.h
 * @author Joshua Jerred (https://joshuajer.red/)
 * @brief Configurable constants.
 * @version 0.1
 * @date 2022-10-02
 * @copyright Copyright (c) 2022
 */

#ifndef CONFIGURABLES_H_
#define CONFIGURABLES_H_

#include <string>

namespace configurables{
  const std::string kGiraffeVersion = GIRAFFE_VERSION_NUMBER; // This is set by CMake
  const std::string kGiraffeVersionStage = GIRAFFE_VERSION_STAGE; // This is set by CMake

  namespace file_paths {
    static const std::string kConfigFilePath = "./gfs_config.json";
    static const std::string kDataLogLocation = "./data_logs/";
    static const std::string kErrorLogLocation = "./error_logs/";
    static const std::string kTelemetryWavLocation = "./telemetry/";
    static const std::string kImagesLocation = "./images/";
  }
}

// ------- Config File Range Limits ------- //
// Changing these results in undefined behavior, it is not recommended.
#define PROJECT_NAME_MIN_LENGTH 1
#define PROJECT_NAME_MAX_LENGTH 25

#define EXTENSION_ID_MIN 1
#define EXTENSION_ID_MAX 255

#define EXTENSION_NAME_MIN_LENGTH 2
#define EXTENSION_NAME_MAX_LENGTH 15

#define EXTENSION_INTERVAL_MIN 200 // In milliseconds
#define EXTENSION_INTERVAL_MAX 1800000 // In milliseconds
// ---------------------------------------- //

// This is the interval, in seconds, that determines how often the data module
// will parse the data stream and update the data frame.
#define MODULE_DATA_FRAME_UPDATE_INTERVAL_MILI_SECONDS 100
#define TELEMETRY_INTERVAL_MILI_SECONDS 500

#define MODULE_SERVER_PORT 8779

#define MODULE_SERVER_CHECK_COMMANDS_INTERVAL 1 // In seconds

#define I2C_BUS_NUMBER_LOW 1
#define I2C_BUS_NUMBER_HIGH 1
#define ADDRESS_LOW 0x04
#define ADDRESS_HIGH 0x77

#define ONE_WIRE_LOCATION "/sys/bus/w1/devices/"

#define CALLSIGN_FAILSAFE "NO_CALLSIGN"

#endif