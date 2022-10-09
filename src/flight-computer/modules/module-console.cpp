// FIX INCLUDES
#include <string>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <ctime>

#include "utility-configurables.h"
#include "utility-config-types.h"
#include "utility-data-stream.h"
#include "utility-data-snapshot.h"

#include "module-data.h"

#include "module-console.h"
/**
 * @todo Use data stream, not data module.
 * @todo console update interval should be a macro
 */
ConsoleModule::ConsoleModule(const ConfigData config_data, DataModule *data) {
    config_data_ = config_data;
    update_interval_ = config_data.debug.console_update_interval;
    data_ = data;
}

ConsoleModule::~ConsoleModule() {
    stop();
}

void ConsoleModule::start() {
    stop_flag_ = 0;
    runner_thread_ = std::thread(&ConsoleModule::runner, this);
}

void ConsoleModule::stop() {
    stop_flag_ = 1;
}

void ConsoleModule::runner() {
    while (!stop_flag_) {
        std::this_thread::sleep_for(std::chrono::milliseconds(update_interval_));
        clearScreen();
        printData();
    }
}

void ConsoleModule::clearScreen() {
    std::cout << "\033[2J\033[1;1H";
}

void ConsoleModule::printData() {
    std::cout << "GFC  -  Giraffe Flight System  -  V" + (std::string) GFC_VERSION << std::endl;
    std::time_t t = std::time(0);   // get time now
    std::tm* now = std::localtime(&t);
    std::cout << now->tm_hour << ":" << now->tm_min << ":" << now->tm_sec
        << "   "
        << (now->tm_year + 1900) << '-' << (now->tm_mon + 1) << '-'
        <<  now->tm_mday << std::endl;

    std::cout << 
    "====================================================================================" 
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
    std::cout << "    Starting Loop: ";
    switch (config_data_.general.starting_loop)
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

    std::cout << "Teletry - " << std::endl;
    std::cout << "Enabled: ";
    int telemetry_enabled = config_data_.telemetry.telemetry_enabled;
    if (telemetry_enabled) {
        std::cout << "Yes";
        /** @todo print telemetry data */
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
        case 3: std::cout << "i2c 0x" + extension.address;
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

    DataSnapshot snapshot = data_->getDataSnapshot();
    std::cout << "Data - " << snapshot.size() << std::endl;
    const int width = 2;
    int i = 1;
    for (auto& [key, value] : snapshot) {  
        std::cout << std::left << std::setw(20) << key << " = " << std::left << 
        std::setw(12) << value << "  |";
        if (i == width) {
            std::cout << std::endl;
            i = 0;
        }
        i++;
    }
}