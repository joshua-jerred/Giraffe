/**
 * @file module-console.cpp
 * @author Joshua Jerred (https://joshuajer.red/)
 * @brief This file contains the Console Module class.
 * 
 * @version 0.0.9
 * @date 2022-10-10
 * @copyright Copyright (c) 2022
 */

#include <iomanip>

#include "modules.h"
using namespace modules;

/**
 * @brief The constructor for the Console Module.
 * @param config_data The configuration data for the system.
 * @param data A pointer to the data module.
 * @todo Error on update interval being less than 1
 */
ConsoleModule::ConsoleModule(const ConfigData config_data, DataStream *stream):
    Module(stream, MODULE_CONSOLE_PREFIX),
    config_data_(config_data),
    p_data_stream_(stream) {

    update_interval_ = config_data.debug.console_update_interval;
    if (update_interval_ < 1) {
        update_interval_ = 1;
        //error()
    }
}

/**
 * @brief The destructor for the Console Module.
 */
ConsoleModule::~ConsoleModule() {
    stop();
}

/**
 * @brief Starts the console module thread and begins printing data.
 * @details Sets the stop flag to 0, and starts the thread which runs the
 * 'runner' function.
 * @param None
 * @return void
 */
void ConsoleModule::start() {
    stop_flag_ = 0;
    runner_thread_ = std::thread(&ConsoleModule::runner, this);
}

/**
 * @brief Sets the stop flag to 1 which will stop the thread, then joins the
 * thread.
 * @details Because it joins the thread it can safely be started again after
 * stopping.
 * @param None
 * @return void
 */
void ConsoleModule::stop() {
    stop_flag_ = 1;
    if (runner_thread_.joinable()) {
        runner_thread_.join();
    }
}

/**
 * @brief The main function for the console module thread.
 * @details This function will run until the stop flag is set to 1. It will
 * print the data to the console, and then sleep for the update interval.
 * @param None
 * @return void
 */
void ConsoleModule::runner() {
    while (!stop_flag_) {
        clearScreen();
        printData();
        std::this_thread::sleep_for(
            std::chrono::milliseconds(update_interval_));
    }
}

/**
 * @brief Clears the screen
 * @param None
 * @return void
 */
void ConsoleModule::clearScreen() {
    std::cout << "\033[2J\033[1;1H";
}

/**
 * @brief Prints formatted data to the console.
 * @details This is a bit messy and probably should be redone with ncurses.
 * @param None
 * @return void
 * @todo Add telemetry data
 */
void ConsoleModule::printData() {
    std::cout << "GFS  -  Giraffe Flight Software  -  V" + 
    (std::string) GFS_VERSION << std::endl;
    std::time_t t = std::time(0);   // get time now
    std::tm* now = std::localtime(&t);
    std::cout << now->tm_hour << ":" << now->tm_min << ":" << now->tm_sec
        << "   "
        << (now->tm_year + 1900) << '-' << (now->tm_mon + 1) << '-'
        <<  now->tm_mday << std::endl;

    std::cout << 
    "=========================================================================="
    "==========" 
    << std::endl << std::endl;

    std::cout << "General - " << std::endl;
    std::cout << "Project Name: " << config_data_.general.project_name <<
    "    Mainboard: ";
    switch (config_data_.general.main_board)
    {
    case 0:
        std::cout << "error";
        break;
    case 1:
        std::cout << "other";
        break;
    case 2:
        std::cout << "Pi Zero";
        break;
    case 3:
        std::cout << "Pi Zero W";
        break;
    case 4:
        std::cout << "Pi 2";
        break;
    case 5:
        std::cout << "Pi 3";
        break;
    case 6: std::cout << "Pi 4";
        break;
    default: std::cout << "Unknown";
        break;
    }
    std::cout << "    Starting Procedure: ";
    switch (config_data_.general.starting_proc)
    {
    case 0: std::cout << "error";
        break;
    case 1: std::cout << "Testing";
        break;
    case 2: std::cout << "Standard";
        break;
    case 3: std::cout << "Recovery";
        break;
    case 4: std::cout << "Failsafe";
        break;
    default:
        std::cout << "Unknown";
        break;
    }
    
    std::cout << std::endl << std::endl;

    FlightProcedure current_flt_proc = p_data_stream_->getFlightProcedureCopy();

    std::cout << "Flight Procedure- " << std::endl;
    std::cout << "Type: ";

    switch (current_flt_proc.type)
    {
    case 0: std::cout << "error";
        break;
    case 1: std::cout << "Testing";
        break;
    case 2: std::cout << "Standard";
        break;
    case 3: std::cout << "Recovery";
        break;
    case 4: std::cout << "Failsafe";
        break;
    default:
        std::cout << "Unknown";
        break;
    }

    std::cout << "   Intervals:" << std::endl;
    std::cout << "Data Log " << current_flt_proc.intervals.data_log << "s   ";
    std::cout << "Data Packet " << current_flt_proc.intervals.data_packet << "s   ";
    std::cout << "SSTV " << current_flt_proc.intervals.sstv << "s   ";
    std::cout << "APRS " << current_flt_proc.intervals.aprs << "s   ";
    std::cout << "Picture " << current_flt_proc.intervals.picture << "s   ";
    std::cout << "HealthCheck " << current_flt_proc.intervals.health_check << "s   ";
    std::cout << std::endl << std::endl;

    std::cout << "Telemetry - " << std::endl;
    std::cout << "Enabled: ";
    int telemetry_enabled = config_data_.telemetry.telemetry_enabled;
    if (telemetry_enabled) {
        std::cout << "Yes";
        std::cout << "    Callsign: " << config_data_.telemetry.call_sign;
        std::cout << std::endl;

        std::cout << "APRS Enabled: " << config_data_.telemetry.aprs_enabled;
        std::cout << "  Frequency: " << config_data_.telemetry.aprs_freq;
        std::cout << "  Key: " << config_data_.telemetry.aprs_key;
        std::cout << std::endl;
        std::cout << "SSID: " << config_data_.telemetry.aprs_ssid;
        std::cout << "  Symbol: " << config_data_.telemetry.aprs_symbol;
        std::cout << "  Memo: " << config_data_.telemetry.aprs_memo;
        std::cout << std::endl;
        
        std::cout << "AFSK Enabled: " << config_data_.telemetry.afsk_enabled;
        std::cout << "  Frequency: " << config_data_.telemetry.afsk_freq;
        std::cout << std::endl;
        
        std::cout << "SSTV Enabled: " << config_data_.telemetry.sstv_enabled;
        std::cout << "  Frequency: " << config_data_.telemetry.sstv_freq;
    } else {
        std::cout << "No";
    }
    std::cout << std::endl << std::endl;

    std::cout << "Extensions - " << std::endl;
    for (ExtensionMetadata extension : config_data_.extensions.extensions_list) {
        std::cout << std::setw(6) <<
        "ID: " + std::to_string(extension.id) << std::setw(15) <<
        "Name: " + extension.name << std::setw(20) <<
        "Type: " + extension.extension_type <<
        "Category: " << std::left << std::setw(10);
        switch ((int) extension.category)
        {
        case 0: std::cout << "error";
            break;
        case 1: std::cout << "other";
            break;
        case 2: std::cout << "radio";
            break;
        case 3: std::cout << "gps";
            break;
        case 4: std::cout << "camera";
            break;
        case 5: std::cout << "internal";
            break;
        case 6: std::cout << "external";
            break;
        default: std::cout << "unknown";
            break;
        }
        std::cout << "Interface: " << std::left << std::setw(10);
        switch ((int) extension.interface)
        {
        case 0: std::cout << "error";
            break;
        case 1: std::cout << "other";
            break;
        case 2: std::cout << "internal";
            break;
        case 3: std::cout << "i2c " + extension.extra_args.I2C_device_address;
            break;
        case 4: std::cout << "serial";
            break;
        case 5: std::cout << "one-wire";
            break;
        case 6: std::cout << "usb";
            break;
        case 7: std::cout << "gpio";
            break;
        default: std::cout << "unknown";
            break;
        }
        std::cout << "  " << std::setw(18) <<
        "Interval: " + std::to_string(extension.update_interval) <<
        "Critical: " << std::left << std::setw(10);
        if (extension.critical) {
            std::cout << "Yes";
        } else {
            std::cout << "No";
        }
        std::cout << std::endl;

    }
    std::cout << std::endl;

    std::cout << p_data_stream_->getTotalDataPackets() << " " 
    << p_data_stream_->getTotalErrorPackets() << std::endl;
    std::cout << p_data_stream_->getNumDataPackets() << " " 
    << p_data_stream_->getNumErrorPackets() << std::endl;

    DataFrame snapshot = p_data_stream_->getDataFrameCopy();
    std::cout << "Data - " << snapshot.size() << std::endl;
    const int width = 2;
    int i = 1;
    for (auto& [key, packet] : snapshot) {  
        std::cout << std::left << std::setw(20) << key << " = " << std::left << 
        std::setw(12) << packet.value << "  |";
        if (i == width) {
            std::cout << std::endl;
            i = 0;
        }
        i++;
    }

    std::cout << std::endl << std::endl;

    ErrorFrame error_snapshot = p_data_stream_->getErrorFrameCopy();
    std::cout << "Errors - " << error_snapshot.size() << std::endl;
    i = 1;
    for (auto& [key, packet] : error_snapshot) {  
        std::cout << packet.source << " - " << packet.error_code << " - ";
        std::cout << packet.info << std::endl;
    }
}