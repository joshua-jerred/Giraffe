/**
 * @file flight-runner.h
 * @author Joshua Jerred (github.com/joshua-jerred)
 * @brief This file defines the FlightRunner class.
 * @version 0.1
 * @date 2022-09-20
 * 
 * @todo Implement Flight Loop
 * 
 * @copyright Copyright (c) 2022
 */
#ifndef _FLIGHT_RUNNER_H_
#define _FLIGHT_RUNNER_H_

#include "utility-config-types.h"
#include "utility-timer.h"
#include "utility-data-stream.h"

#include "module-data.h"
#include "module-extensions.h"

/**
 * @brief The FlightRunner class is responsible for pulling everything together.
 * It manages the timing of each action that is defined in the other files.
 * It is triggered with the start() method and will only return upon program
 * termination.
 * 
 */
class FlightRunner {
public:
    /**
     * @brief Construct a new Flight Runner object.
     */
    FlightRunner();

    /**
     * @brief Deconstructs the flight runner object, called at program
     * termination; ideally called after shutdown().
     * @see shutdown()
     */
    ~FlightRunner();

    /**
     * @brief start() initializes the system.
     * start() will perform the following actions:
     * 1. Read and parse config file
     * 2. Start the Data Service
     * 3. Start the Web Server (if enabled)
     * 4. Start the Extension Handler
     * 5. Start the Radio Communication
     * 6. Perform a system wide test to determine flight loop configuration 
     *    (standard, failsafe, etc)
     * 7. Call flightLoop()
     * 
     * Start will only return upon the termination of the flight loop.
     * 
     * @return int shutdown signal to main [0 = good shutdown, any other number 
     * = bad shutdown] 
     */
    int start();

    /**
     * @brief Sends a shutdown signal to all modules (ExtensionHandler, 
     * RadioCommunication, etc). Called by flightLoop().
     * 
     * @return int shutdown signal to main [0 = good shutdown, any other 
     * number = bad shutdown]
     */
    int shutdown();

private:
    /**
     * @brief This is where everything actually happens. This is the endless
     * loop that utilizes timers to perform actions. These actions include
     * everything that this software was made to do. Communication, data
     * collection, tracking, etc. It takes in the loop configuration data from 
     * start(), creates timers for each action type, performs each action on
     * a user defined timer interval. It will also be responsible for triggering
     * a shutdown when it is commanded to do so. It is capable of switching 
     * interval configurations between one flight loop type and another. It
     * will switch either when commanded to or upon the results of a system
     * wide "health check".
     * 
     * @return int shutdown signal [0 = good shutdown, any other 
     * number = bad shutdown]
     */
    int flightLoop();

    /**
     * @brief 
     * 
     * @return int 
     */
    int healthCheck();

    void switchLoops(FlightLoop::LoopType loopType);

    ConfigData mConfigData;
    FlightLoop::LoopType mCurrentFlightLoop;
    FlightLoop::Intervals mCurrentIntervals;
    
    DataModule *mpDataModule;
    ExtensionsModule *mpExtensionsModule;
    //ServerModule *mpServerModule;
    //ComModule *mpComModule;
    int mActive;

    FlightLoop mTestingLoop;
    FlightLoop mStandardLoop;
    FlightLoop mDescentLoop;
    FlightLoop mRecoveryLoop;
    FlightLoop mFailsafeLoop;
};

#endif