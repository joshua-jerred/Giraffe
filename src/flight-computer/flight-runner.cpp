

#include <chrono>
#include <iostream>

#include "utility-configurables.h"
#include "utility-config-types.h"
#include "utility-timer.h"
#include "utility-data-stream.h"

#include "module-configuration.h"
#include "module-extensions.h"
#include "module-data.h"
/*
#include "module-server.h"
#include "module-telemetry.h"
*/

#include "flight-runner.h"

FlightRunner::FlightRunner() {
    current_flight_loop_type_ = FlightLoop::LoopType::kFailsafe;
}

FlightRunner::~FlightRunner() {
}

int FlightRunner::start() {
    // Read the Flight Module Config File
    ConfigModule* config = new ConfigModule();
    int status = config->load(CONFIG_LOCATION);
    if (status != 0) {
        std::cout << "Error: Could not load config file." << std::endl;
        return 1; /** @todo change to hardcoded failsafe */
    } else {
        config_data_ = config->getAll();
    }
    
    delete config; // File is not needed after loading all config data

    p_data_module_ = new DataModule(config_data_); // Start Data Service
    p_extension_module_ = new ExtensionsModule(config_data_, p_data_module_->getDataStream()); // Enable Extensions
    p_extension_module_->start(); // Start Extensions

    //mpServerModule = new ServerModule(config_data_, p_data_module_); // Start Web Server
    //mpComModule = new ComModule(config_data_, p_data_module_, 
    //p_extension_module_); // Enable Radio communication

    if (config_data_.general.starting_loop == FlightLoop::LoopType::kTesting) { // If user specified in config to use the testing loop, it's selected here.
        switchLoops(FlightLoop::LoopType::kTesting);
        std::cout << "Starting in Testing Loop" << std::endl;
    } else if (healthCheck() == 0) {
        switchLoops(FlightLoop::LoopType::kStandard);
        std::cout << "Starting in Standard Loop" << std::endl;
    } else {
        switchLoops(FlightLoop::LoopType::kFailsafe);
        std::cout << "Starting in Failsafe Loop" << std::endl;
    }
    
    return flightLoop(); // This will only return on shutdown
}

int FlightRunner::flightLoop() {
    Timer tsl_server; // tsl = time since last
    Timer tsl_data_log; // Refer to timer.h
    Timer tsl_photo;
    Timer tsl_data_packet;
    Timer tsl_APRS; 
    Timer tsl_SSTV_image; 
    Timer tsl_health_check;
    
    mActive = 1;

    /*! FLIGHT LOOP */
    while (mActive == 1) { // The endless loop where everything happens
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        //if (tslServer.elapsed() > current_intervals_.serverUpdate) {
        //    mpServerModule->update();
        //}
        if (tsl_data_log.elapsed() > current_intervals_.data_log) {
            p_data_module_->log();
            tsl_data_log.reset();
        }
        //if (tslPhoto.elapsed() > current_intervals_.picture) {
        //    p_data_module_->capturePhoto();
        //}
        //if (tslDataPacket.elapsed() > current_intervals_.dataPacket) {
        //    mpComModule->txDataPacket();
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
    return 0;
}

/**
 * @todo Determine what information to check
 * @warning Not Yet Implemented
 * @return int 
 */
int FlightRunner::healthCheck() {
    return 0;
}

void FlightRunner::switchLoops(FlightLoop::LoopType loopType) {
    ConfigData::Loops loops = config_data_.flight_loops;
    switch (loopType)
    {
    case FlightLoop::LoopType::kTesting:
        current_intervals_ = loops.testing.intervals;
        break;
    default: // Default back to failsafe flight loop
        current_intervals_ = loops.failsafe.intervals;
        break;
    }
}