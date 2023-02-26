/**
 * @file configurables.h
 * @author Joshua Jerred (https://joshuajer.red/)
 * @brief All macro definitions for the flight control software will be in
 * this file. Macros should be avoided.
 * @version 0.1
 * @date 2022-10-02
 * @copyright Copyright (c) 2022
 */

#ifndef UTILITY_CONFIGURABLES_H
#define UTILITY_CONFIGURABLES_H

namespace configurables{
  namespace data_module {
    const float kMaxLogFileSizeMB = 50; // MB
  }
}

#define GFS_VERSION "0.4"

/** @brief The location of the configuration file. It is safe to change 
 * this value. */
#define CONFIG_LOCATION "./config.json"

#define TELEMETRY_WAV_LOCATION "./telem/"

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

// ------- Error Prefixes ------- //
#define MODULE_DATA_PREFIX "M_DA"
#define MODULE_CONFIG_PREFIX "M_CF"
#define MODULE_TELEMETRY_PREFIX "M_TL"
#define MODULE_SERVER_PREFIX "M_SV"
#define MODULE_EXTENSION_PREFIX "M_EX"
#define MODULE_CONSOLE_PREFIX "M_CO"

#define EXTENSION_PREFIX "EX_"

// ------------------------------ //

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

#define TELEMETRY_WAV_LOCATION "./telem/" /** @todo look into moving this to ram disk */

#define CALLSIGN_FAILSAFE "NO_CALLSIGN"

#endif