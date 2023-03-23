/**
 * @file configurables.h
 * @author Joshua Jerred (https://joshuajer.red/)
 * @brief Configurable constants.
 * @version 0.1
 * @date 2022-10-02
 * @copyright Copyright (c) 2022
 */

#ifndef UTILITY_CONFIGURABLES_H
#define UTILITY_CONFIGURABLES_H

#include <string>

namespace configurables{
  const std::string kGiraffeVersion = GIRAFFE_VERSION_NUMBER; // This is set by CMake
  const std::string kGiraffeVersionStage = GIRAFFE_VERSION_STAGE; // This is set by CMake
  namespace config_defaults {
    static const std::string kAprsFrequency = "144.390";
    static const uint8_t kAprsSourceSSID = 0;
    static const bool kAprsPositionPacketEnabled = true;
    static const bool kAprsTelemetryPacketEnabled = false;
    static const std::string kAprsDestinationAddress = "APZGFS";
    static const uint8_t kAprsDestinationSSID = 0;
    static const char kAprsSymbol = '/'; // '/' is a dot

    static const std::string kSstvFrequency = "145.550";
    static const std::string kSstvMode = "robot36";
    static const bool kSstvSaveImages = false;
    static const bool kSstvOverlayData = true;

    static const std::string kDataPacketsFrequency = "145.550";
    static const std::string kDataPacketsMode = "bpsk500";
  }
  namespace config_limits {
    static const int kAprsMemoMaxSize = 30;
    static const int kSstvCommentMaxSize = 15;
  }
  namespace data_module {
    static const float kMaxLogFileSizeMB = 50; // MB
  }
  namespace telemetry_module {
    
        
  }
  namespace file_paths {
    static const std::string kConfigFilePath = "./gfs-configuration.json";
    static const std::string kDataLogLocation = "./data_logs/";
    static const std::string kErrorLogLocation = "./error_logs/";
    static const std::string kTelemetryWavLocation = "./telemetry/";
    static const std::string kImagesLocation = "./images/";
  }
  namespace prefix {
    static const std::string kDataModule = "M_DA";
    static const std::string kConfigModule = "M_CF";
    static const std::string kTelemetryModule = "M_TL";
    static const std::string kServerModule = "M_SV";
    static const std::string kExtensionModule = "M_EX";
    static const std::string kConsoleModule = "M_CO";

    static const std::string kExtension = "EX_";
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