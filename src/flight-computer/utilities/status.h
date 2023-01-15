/**
 * @file status.h
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief Contains types for status enums for modules and extensions.
 * @date 2022-12-15
 * @copyright Copyright (c) 2022
 * @version 0.2
 */

#ifndef UTILITY_STATUS_H_
#define UTILITY_STATUS_H_

#include <unordered_map>
#include <string>

/**
 * @brief This is the enum used to communicate with the extension runner.
 * @details When an extension is constructed it's status will be set to stopped.
 * When Extension::start() is called the status will be set to starting.
 * The Extension::runner() (of the derived class) will be called and the status
 * should be changed to running. When the ExtensionModule calls 
 * Extension::stop() the status will change to 'STOPPING'. The 
 * Extension::runner() is then responsible for deconstructing and changing the
 * status to 'STOPPED' right before it exits.
 * 
 * If any errors occur the status will be set to 'ERROR' and there will be an
 * error message in the data stream.
 */
enum class ExtensionStatus { 
    ERROR = 0,
    STOPPED = 1, 
    STARTING = 2, 
    RUNNING = 3,
    STOPPING = 4,
    STOPPED_ERROR_STATE = 5
};

/**
 * @brief module_status is used by the individual modules. Used by FlightRunner.
 * @see FlightRunner
 */
enum class ModuleStatus {
    STARTING,
    RUNNING,
    STOPPING,
    STOPPED,
    ERROR_STATE 
};

static const std::unordered_map<ModuleStatus, std::string> module_status_to_string_map = {
    {ModuleStatus::STARTING, "STARTING"},
    {ModuleStatus::RUNNING, "RUNNING"},
    {ModuleStatus::STOPPING, "STOPPING"},
    {ModuleStatus::STOPPED, "STOPPED"},
    {ModuleStatus::ERROR_STATE, "ERROR_STATE"}
};

enum class I2C_STATUS {
    BUS_LOCK_CONFIG_ERROR, // Bus lock pointer is null
    NOT_CONNECTED,
    OK,
    CONFIG_ERROR_BUS,
    CONFIG_ERROR_ADDRESS,
    BUS_ERROR,
    ADDRESS_ERROR,
    READ_ERROR,
    WRITE_ERROR,
    UNKNOWN_ERROR
};

enum class ONEWIRE_STATUS {
    OK,
    NOT_FOUND,
    READ_ERROR,
    UNKNOWN_ERROR
};

#endif