/**
 * @file flight-runner.cpp
 * @author Joshua Jerred (github.com/joshua-jerred)
 * @brief This file contains the high level flight computer code.
 * @details This file implemented the FlightRunner class.
 * 
 * @version 0.0.9
 * @date 2022-10-10
 * @copyright Copyright (c) 2022
 */

#include <chrono>
#include <iostream>
#include <csignal>


#include "utility-configurables.h"
#include "utility-config-types.h"
#include "utility-timer.h"
#include "utility-data-stream.h"

#include "module-configuration.h"
#include "module-extensions.h"
#include "module-data.h"
#include "module-telemetry.h"
/*
#include "module-server.h"
#include "module-telemetry.h"
*/

#include "flight-runner.h"

FlightRunner::FlightRunner() {
    current_flight_loop_type_ = FlightLoop::LoopType::FAILSAFE;
}

FlightRunner::~FlightRunner() {
    deconstruct();
}

int FlightRunner::start() {
    // ~~~ Create the Data Module ~~~ //
    p_data_module_ = new DataModule();
    // Do not start the DataModule yet because we need to add the config data

    // ~~~ Read The Config ~~~ //
    ConfigModule* config = new ConfigModule(p_data_module_->getDataStream());
    int status = config->load(CONFIG_LOCATION);
    if (status == -1) {
        std::cout << "Error: Could not load config file." << std::endl;
        return 1; /** @todo change to hardcoded failsafe */
    } else {
        config_data_ = config->getAll();
    }

    delete config; // The config module is not needed after loading config data
    
    p_data_module_->addConfigData(config_data_); // Add the config data to the DataModule

    p_data_module_->start(); // Start the DataModule
    
    // ~~~ Start the Extensions Module ~~~ //
    p_extension_module_ = new ExtensionsModule(config_data_, 
        p_data_module_->getDataStream()); // Enable Extensions

    p_extension_module_->start(); // Start Extensions

    // ~~~ Start the Console Module ~~~ //
    if (config_data_.debug.console_enabled) {
        p_console_module_ = new ConsoleModule(config_data_, 
            p_data_module_->getDataStream());
        p_console_module_->start();
    }

    // ~~~ Start the Telemetry Module ~~~ //
    if (config_data_.telemetry.telemetry_enabled) {
        p_telemetry_module_ = new TelemetryModule(config_data_, 
            p_data_module_->getDataStream());
        p_telemetry_module_->start();
    }

    // ~~~ Setup Done, Start the Flight Loop ~~~ //
    if (config_data_.general.starting_loop == FlightLoop::LoopType::TESTING) { // If user specified in config to use the testing loop, it's selected here.
        switchLoops(FlightLoop::LoopType::TESTING);
        std::cout << "Starting in Testing Loop" << std::endl;
    } else {
        healthCheck(); // Perform a health check to determine the flight loop type
    }
    
    return flightLoop(); // This will only return on shutdown
}

void FlightRunner::shutdown() {
    std::cout << "Shutting down..." << std::endl;
    shutdown_signal_ = 1;
}

int FlightRunner::flightLoop() {
    Timer tsl_data_log; // Refer to utility-timer.h
    Timer tsl_server; // tsl = time since last
    Timer tsl_photo;
    Timer tsl_data_packet;
    Timer tsl_APRS; 
    Timer tsl_SSTV_image; 
    Timer tsl_health_check;
    
    shutdown_signal_ = 0;

    /** @note FLIGHT LOOP */
    while (!shutdown_signal_) { // The endless loop where everything happens
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        if (tsl_data_log.elapsed() > current_intervals_.data_log) {
            p_data_module_->log();
            tsl_data_log.reset();
        }
        if (config_data_.telemetry.telemetry_enabled && 
        tsl_data_packet.elapsed() > current_intervals_.data_packet) {
            p_telemetry_module_->sendDataPacket();
        }
        //if (tslServer.elapsed() > current_intervals_.serverUpdate) {
        //    mpServerModule->update();
        //}
        //if (tslPhoto.elapsed() > current_intervals_.picture) {
        //    p_data_module_->capturePhoto();
        //}
        //if (tslAPRS.elapsed() > current_intervals_.aprs) {
        //    mpComModule->txAPRSPacket();
        //}
        //if (tslSSTVImage.elapsed() > current_intervals_.sstv) {
        //    mpComModule->txSSTVImage();
        //}
        //if (tslHealthCheck.elapsed() > current_intervals_.healthCheck) {
        //    healthCheck();
        //}
    }
    std::cout << "Shutdown signal received." << std::endl;
    deconstruct();

    return 0;
}

/**
 * @brief This function will switch the flight loop to a different loop type
 * if the values of extensions are within certain ranges.
 * @details This function is currently not implemented but it is responsible
 * for switching the flight loops if it's needed. This can be because flight
 * critical components are not working or to switch from standard to recovery.
 * @param None
 * @return void
 */
void FlightRunner::healthCheck() {
}

void FlightRunner::switchLoops(FlightLoop::LoopType loopType) {
    ConfigData::Loops loops = config_data_.flight_loops;
    switch (loopType)
    {
    case FlightLoop::LoopType::TESTING:
        current_intervals_ = loops.testing.intervals;
        break;
    default: // Default back to failsafe flight loop
        current_intervals_ = loops.failsafe.intervals;
        break;
    }
}

void FlightRunner::deconstruct() {
    std::cout << "Stopping Console Module ... ";
    if (p_console_module_ != nullptr) {
        //p_console_module_->stop();
        std::cout << "Stopped" << std::endl;
    } else {
        std::cout << "Not Running" << std::endl;
    }
    std::cout << "Stopping Telemetry Module ... ";
    if (p_telemetry_module_ != nullptr) {
        p_telemetry_module_->stop();
        std::cout << "Stopped" << std::endl;
    } else {
        std::cout << "Not Running" << std::endl;
    }
    std::cout << "Stopping Extension Module ... ";
    if (p_extension_module_ != nullptr) {
        p_extension_module_->stop();
        std::cout << "Stopped" << std::endl;
    } else {
        std::cout << "Not Running" << std::endl;
    }
    std::cout << "Stopping Data Module ... ";
    if (p_data_module_ != nullptr) {
        p_data_module_->stop();
        std::cout << "Stopped" << std::endl;
    } else {
        std::cout << "Not Running" << std::endl;
    }
}