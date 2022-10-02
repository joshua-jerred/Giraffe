/**
 * @file flight-runner.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-10-01
 * 
 * @copyright Copyright (c) 2022
 * @defgroup flight-computer
 * @addtogroup flight-computer
 */

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
    mActive = 1;
    mCurrentFlightLoop = FlightLoop::LoopType::kFailsafe;
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
        mConfigData = config->getAll();
    }
    
    delete config; // File is not needed after loading all config data

    mpDataModule = new DataModule(mConfigData); // Start Data Service
    
    mpExtensionsModule = new ExtensionsModule(mConfigData, mpDataModule->getDataStream()); // Enable Extensions
    mpExtensionsModule->start(); // Start Extensions

    //mpServerModule = new ServerModule(mConfigData, mpDataModule); // Start Web Server
    //mpComModule = new ComModule(mConfigData, mpDataModule, 
    //mpExtensionsModule); // Enable Radio communication

    if (mConfigData.general.starting_loop == FlightLoop::LoopType::kTesting) { // If user specified in config to use the testing loop, it's selected here.
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
    Timer tslServer; // tsl = time since last
    Timer tslDataLog; // Refer to timer.h
    Timer tslPhoto;
    Timer tslDataPacket;
    Timer tslAPRS; 
    Timer tslSSTVImage; 
    Timer tslHealthCheck;
    
    mActive = 1;

    /*! FLIGHT LOOP */
    while (mActive == 1) { // The endless loop where everything happens
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        //if (tslServer.elapsed() > mCurrentIntervals.serverUpdate) {
        //    mpServerModule->update();
        //}
        if (tslDataLog.elapsed() > mCurrentIntervals.data_log) {
            mpDataModule->log();
            tslDataLog.reset();
        }
        //if (tslPhoto.elapsed() > mCurrentIntervals.picture) {
        //    mpDataModule->capturePhoto();
        //}
        //if (tslDataPacket.elapsed() > mCurrentIntervals.dataPacket) {
        //    mpComModule->txDataPacket();
        //}
        //if (tslAPRS.elapsed() > mCurrentIntervals.aprs) {
        //    mpComModule->txAPRSPacket();
        //}
        //if (tslSSTVImage.elapsed() > mCurrentIntervals.sstv) {
        //    mpComModule->txSSTVImage();
        //}
        //if (tslHealthCheck.elapsed() > mCurrentIntervals.healthCheck) {
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
    ConfigData::Loops loops = mConfigData.flight_loops;
    switch (loopType)
    {
    case FlightLoop::LoopType::kTesting:
        mCurrentIntervals = loops.testing.intervals;
        break;
    default: // Default back to failsafe flight loop
        mCurrentIntervals = loops.failsafe.intervals;
        break;
    }
}