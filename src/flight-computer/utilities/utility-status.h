/**
 * @file utility-status.h
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief Contains types for status of modules and extensions.
 * @date 2022-12-15
 * @copyright Copyright (c) 2022
 * @version 0.2
 */

#ifndef UTILITY_STATUS_H_
#define UTILITY_STATUS_H_

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
    STOPPING = 4
};

/**
 * @brief module_status is used by the individual modules. Used by FlightRunner.
 * @see FlightRunner
 */
enum class ModuleStatus {
    STOPPED=0,
    STARTING=1,
    RUNNING=2,
    STOPPING=3,
    ERROR_STATE=4 
};

#endif