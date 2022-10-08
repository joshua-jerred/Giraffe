/**
 * @file utility-configurables.h
 * @author Joshua Jerred (github.com/joshua-jerred)
 * @brief All macro definitions for the flight control software will be in
 * this file. Macros should be avoided.
 * @version 0.1
 * @date 2022-10-02
 * @copyright Copyright (c) 2022
 */

#ifndef UTILITY_CONFIGURABLES_H
#define UTILITY_CONFIGURABLES_H

#define GFC_VERSION "0.1.1"

/** @brief The locaiton of the configuration file. It is safe to change 
 * this value. */
#define CONFIG_LOCATION "./config.json"

/** @brief The location of the data log directory. Must be relative. 
 * It is safe to change this value. */
#define DATA_LOG_LOCATION "./logs/data/" // These must end with a "/"

/** @brief The location of the error log directory. Must be relative. 
 * It is safe to change this value. */
#define ERROR_LOG_LOCATION "./logs/errors/"

#define TELEMETRY_WAV_LOCATION "./telem/"

// ------- Config File Range Limits ------- //
// Changing these results in undefined bevahior, it is not recommended.
#define PROJECT_NAME_MIN_LENGTH 3
#define PROJECT_NAME_MAX_LENGTH 20

#define EXTENSION_NAME_MIN_LENGTH 3
#define EXTENSION_NAME_MAX_LENGTH 15

#define EXTENSION_INTERVAL_MIN 0
#define EXTENSION_INTERVAL_MAX 3600
// ---------------------------------------- //

// ------- Error Prefixes ------- //
#define MODULE_TELEMETRY_ERROR_PREFIX "M_TEL"

// ------------------------------ //

// This is the interval, in seconds, that determines how often the data module
// will parse the data stream and update the data frame.
#define MODULE_DATA_FRAME_UPDATE_INTERVAL_MILI_SECONDS 500


#endif