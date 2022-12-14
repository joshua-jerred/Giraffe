/**
 * @file flight-runner.cpp
 * @author Joshua Jerred (https://joshuajer.red/)
 * @brief This file contains the high level flight computer code.
 * @details This file implemented the FlightRunner class.
 * 
 * @version 0.0.9
 * @date 2022-10-10
 * @copyright Copyright (c) 2022
 */

#include "flight-runner.h"

FlightRunner::FlightRunner():
    current_flight_procedure_type_(FlightProcedure::ProcType::FAILSAFE),
    shutdown_signal_(false)
    {
}

FlightRunner::~FlightRunner() {
}

int FlightRunner::start() {
    // ~~~ Create the Data Module ~~~ //
    p_data_module_ = new modules::DataModule(data_stream_);
    // Do not start the DataModule yet because we need to add the config data

    // ~~~ Read The Config ~~~ //
    modules::ConfigModule* config = new modules::ConfigModule(&data_stream_);
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
    p_extension_module_ = new modules::ExtensionsModule(config_data_, 
        &data_stream_); // Enable Extensions

    p_extension_module_->start(); // Start Extensions

    // ~~~ Start the Console Module ~~~ //
    if (config_data_.debug.console_enabled) {
        p_console_module_ = new modules::ConsoleModule(config_data_, 
            &data_stream_);
        p_console_module_->start();
    }

    // ~~~ Start the Server Module ~~~ //
    if (config_data_.debug.web_server_enabled) {
        p_server_module_ = new modules::ServerModule(config_data_, 
            &data_stream_);
        p_server_module_->start();
    }

    // ~~~ Start the Telemetry Module ~~~ //
    if (config_data_.telemetry.telemetry_enabled) {
        p_telemetry_module_ = new modules::TelemetryModule(config_data_, 
            &data_stream_);
        p_telemetry_module_->start();
    }

    // ~~~ Setup Done, Start the Flight Loop ~~~ //
    if (config_data_.general.starting_proc == FlightProcedure::ProcType::TESTING) { // If user specified in config to use the testing proc, it's selected here.
        switchLoops(FlightProcedure::ProcType::TESTING);
        std::cout << "Starting in Testing Loop" << std::endl;
    } else {
        healthCheck(); // Perform a health check to determine the flight proc type
    }
    shutdown_signal_ = 0;
    return flightLoop(); // This will only return on shutdown
}

void FlightRunner::shutdown() {
    std::cout << "Shutting down" << std::endl;
    shutdown_signal_ = 1;
    return;
}

int FlightRunner::flightLoop() {
    std::cout << "Starting Flight Loop" << std::endl;
    Timer tsl_data_log; // Refer to timer.h
    Timer tsl_server; 
    Timer tsl_data_packet; // tsl = time since last
    Timer tsl_photo;
    Timer tsl_APRS; 
    Timer tsl_SSTV_image; 
    Timer tsl_health_check;
    
    GFSCommand command;

    /** @note FLIGHT LOOP */
    while (!shutdown_signal_) { // The endless loop where everything happens
        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        if (data_stream_.getNextCommand(command)) { // Check for commands
            switch (command.category) {
                case GFSCommand::CommandCategory::FLR:
                    
                    break;
                case GFSCommand::CommandCategory::TLM: // Telemetry Module
                    if (p_telemetry_module_ != nullptr) {
                        p_telemetry_module_->addCommand(command);
                    }
                    break;
                case GFSCommand::CommandCategory::MDL:
                    
                    break;
                case GFSCommand::CommandCategory::EXT:
                    
                    break;
                default:
                    std::cout << "unknown command category" << std::endl;
                    break;
            }
        }

        if (tsl_data_log.elapsed() > current_intervals_.data_log) {
            p_data_module_->log();
            tsl_data_log.reset();
        }

        if (config_data_.telemetry.telemetry_enabled) {
            if (config_data_.telemetry.psk_enabled &&
                tsl_data_packet.elapsed() > current_intervals_.data_packet) {
                p_telemetry_module_->sendDataPacket();
                tsl_data_packet.reset();
            }
            if (config_data_.telemetry.aprs_enabled &&
                tsl_APRS.elapsed() > current_intervals_.aprs) {
                p_telemetry_module_->sendAPRS();
                tsl_APRS.reset();
            }
            if (config_data_.telemetry.sstv_enabled &&
                tsl_SSTV_image.elapsed() > current_intervals_.sstv) {
                p_telemetry_module_->sendSSTVImage();
                tsl_SSTV_image.reset();
            }
        }
        
        if (config_data_.debug.web_server_enabled &&
            tsl_server.elapsed() > MODULE_SERVER_CHECK_COMMANDS_INTERVAL) {
            if (p_server_module_->checkShutdown()) {
                shutdown();
            }
        }
        //if (tslPhoto.elapsed() > current_intervals_.picture) {
        //    p_data_module_->capturePhoto();
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
 * @brief This function will switch the flight proc to a different proc type
 * if the values of extensions are within certain ranges.
 * @details This function is currently not implemented but it is responsible
 * for switching the flight procs if it's needed. This can be because flight
 * critical components are not working or to switch from standard to recovery.
 * @param None
 * @return void
 */
void FlightRunner::healthCheck() {
}

void FlightRunner::switchLoops(FlightProcedure::ProcType procType) {
    ConfigData::Procs procs = config_data_.flight_procs;
    FlightProcedure current_procedure; 
    switch (procType)
    {
    case FlightProcedure::ProcType::TESTING:

        current_intervals_ = procs.testing.intervals; // Set the intervals
        
        // Send the procedure to the data stream
        current_procedure.type = FlightProcedure::ProcType::TESTING;
        current_procedure.intervals = procs.testing.intervals;
        data_stream_.updateFlightProcedure(current_procedure);

        break;
    default: // Default back to failsafe flight proc

        current_intervals_ = procs.failsafe.intervals; // Set the intervals

        // Send the procedure to the data stream
        current_procedure.type = FlightProcedure::ProcType::FAILSAFE;
        current_procedure.intervals = procs.failsafe.intervals;
        data_stream_.updateFlightProcedure(current_procedure);
        break;
    }

}

void FlightRunner::deconstruct() {   
    std::cout << "Stopping Console Module ... ";
    if (p_console_module_ != nullptr) {
        p_console_module_->stop();
        delete p_console_module_;
        std::cout << "Stopped" << std::endl;
    } else {
        std::cout << "Not Running" << std::endl;
    }
    std::cout << "Stopping Server Module ... ";
    if (p_server_module_ != nullptr) {
        p_server_module_->stop();
        delete p_server_module_;
        std::cout << "Stopped" << std::endl;
    }
    std::cout << "Stopping Telemetry Module ... ";
    if (p_telemetry_module_ != nullptr) {
        p_telemetry_module_->stop();
        delete p_telemetry_module_;
        std::cout << "Stopped" << std::endl;
    } else {
        std::cout << "Not Running" << std::endl;
    }
    std::cout << "Stopping Extension Module ... ";
    if (p_extension_module_ != nullptr) {
        p_extension_module_->stop();
        delete p_extension_module_;
        std::cout << "Stopped" << std::endl;
    } else {
        std::cout << "Not Running" << std::endl;
    }
    std::cout << "Stopping Data Module ... ";
    if (p_data_module_ != nullptr) {
        p_data_module_->stop();
        delete p_data_module_;
        std::cout << "Stopped" << std::endl;
    } else {
        std::cout << "Not Running" << std::endl;
    }
}