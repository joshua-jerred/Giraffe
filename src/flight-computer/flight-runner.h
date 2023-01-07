/**
 * @file flight-runner.h
 * @author Joshua Jerred (https://joshuajer.red/)
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

#include "config-types.h"
#include "timer.h"
#include "data-stream.h"

#include "modules.h"

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

    FlightRunner(const FlightRunner&) = delete; // No copy constructor
    FlightRunner& operator=(const FlightRunner&) = delete; // No copy assignment

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
     * 6. Perform a system wide test to determine flight proc configuration 
     *    (standard, failsafe, etc)
     * 7. Call flightLoop()
     * 
     * Start will only return upon the termination of the flight proc.
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
    void shutdown();

private:
    /**
     * @brief This is where everything actually happens. This is the endless
     * loop that utilizes timers to perform actions. These actions include
     * everything that this software was made to do. Communication, data
     * collection, tracking, etc. It takes in the proc configuration data from 
     * start(), creates timers for each action type, performs each action on
     * a user defined timer interval. It will also be responsible for triggering
     * a shutdown when it is commanded to do so. It is capable of switching 
     * interval configurations between one flight proc type and another. It
     * will switch either when commanded to or upon the results of a system
     * wide "health check".
     * 
     * @return int shutdown signal [0 = good shutdown, any other 
     * number = bad shutdown]
     */
    int flightLoop();

    void healthCheck();

    void switchLoops(FlightProcedure::ProcType procType);

    void deconstruct();

    ConfigData config_data_ = ConfigData();
    FlightProcedure::ProcType current_flight_procedure_type_;
    FlightProcedure::Intervals current_intervals_ = FlightProcedure::Intervals();
    
    DataStream data_stream_ = DataStream();

    modules::DataModule *p_data_module_ = nullptr;
    modules::ExtensionsModule *p_extension_module_ = nullptr;
    modules::ConsoleModule *p_console_module_ = nullptr;
    modules::ServerModule *p_server_module_ = nullptr;
    modules::TelemetryModule *p_telemetry_module_ = nullptr;
    //ComModule *mpComModule;
    int shutdown_signal_;

    FlightProcedure flt_proc_testing_ = FlightProcedure(FlightProcedure::ProcType::TESTING);
    FlightProcedure flt_proc_standard_ = FlightProcedure(FlightProcedure::ProcType::STANDARD);
    FlightProcedure flt_proc_descent_ = FlightProcedure(FlightProcedure::ProcType::DESCENT);
    FlightProcedure flt_proc_recovery_ = FlightProcedure(FlightProcedure::ProcType::RECOVERY);
    FlightProcedure flt_proc_failsafe_ = FlightProcedure(FlightProcedure::ProcType::FAILSAFE);
};

#endif