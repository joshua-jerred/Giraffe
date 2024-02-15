/**
 * @file simulated-extensions.cpp
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief Simulated extensions for testing purposes.
 * @date 2023-01-29
 * @copyright Copyright (c) 2023
 * @version 0.1
 *
 * @todo Documentation
 * @todo Unit Tests
 */

#include <iostream>
#include <thread>
#include <atomic>

#include "extensions.h"

using namespace extension;

PRESS_SENSOR_SIM::PRESS_SENSOR_SIM(DataStream *p_data_stream, ExtensionMetadata extension_metadata)
    : Extension(p_data_stream, extension_metadata) {
    // Do nothing
    start_time_ = std::time(0);
}
PRESS_SENSOR_SIM::~PRESS_SENSOR_SIM() {
    // Do nothing
}
int PRESS_SENSOR_SIM::runner() {
    setStatus(ExtensionStatus::RUNNING);
    while (!stop_flag_) {
        // calculate time passed since start
        std::time_t now = std::time(0);
        std::time_t time_passed = now - start_time_;

        // calculate pressure
        if (time_passed < seconds_to_apogee_) { // Pressure Decreases
            pressure_ += pressure_delta_down_;
        } else { // Pressure Increases
            pressure_ += pressure_delta_up_;
        }

        // write pressure to data stream
        sendData("PRES_MBAR", pressure_);
		std::this_thread::sleep_for(
            std::chrono::milliseconds(getUpdateInterval())
        );
    }
    setStatus(ExtensionStatus::STOPPED);
    return 0;
}



TEMP_SENSOR_SIM::TEMP_SENSOR_SIM(DataStream *p_data_stream, ExtensionMetadata extension_metadata)
    : Extension(p_data_stream, extension_metadata) {
    // Do nothing
}
TEMP_SENSOR_SIM::~TEMP_SENSOR_SIM() {
    // Do nothing
}
int TEMP_SENSOR_SIM::runner() {
    setStatus(ExtensionStatus::RUNNING);
    while (!stop_flag_) {


		std::this_thread::sleep_for(
            std::chrono::milliseconds(getUpdateInterval())
        );
    }
    setStatus(ExtensionStatus::STOPPED);
    return 0;
}



RADIO_SIM::RADIO_SIM(DataStream *p_data_stream, ExtensionMetadata extension_metadata)
    : Extension(p_data_stream, extension_metadata) {
    // Do nothing
}
RADIO_SIM::~RADIO_SIM() {
    // Do nothing
}
int RADIO_SIM::runner() {
    setStatus(ExtensionStatus::RUNNING);
    while (!stop_flag_) {


		std::this_thread::sleep_for(
            std::chrono::milliseconds(getUpdateInterval())
        );
    }
    setStatus(ExtensionStatus::STOPPED);
    return 0;
}

GPS_SIM::GPS_SIM(DataStream *p_data_stream, ExtensionMetadata extension_metadata)
    : Extension(p_data_stream, extension_metadata) {
    // Do nothing
}
GPS_SIM::~GPS_SIM() {
    // Do nothing
}
int GPS_SIM::runner() {
    setStatus(ExtensionStatus::RUNNING);
    while (!stop_flag_) {
        GPSFrame gps_frame;

		std::this_thread::sleep_for(
            std::chrono::milliseconds(getUpdateInterval())
        );
    }
    setStatus(ExtensionStatus::STOPPED);
    return 0;
}

BATT_SENSOR_SIM::BATT_SENSOR_SIM(DataStream *p_data_stream, ExtensionMetadata extension_metadata)
    : Extension(p_data_stream, extension_metadata) {
    // Do nothing
}
BATT_SENSOR_SIM::~BATT_SENSOR_SIM() {
    // Do nothing
}
int BATT_SENSOR_SIM::runner() {
    setStatus(ExtensionStatus::RUNNING);
    while (!stop_flag_) {


		std::this_thread::sleep_for(
            std::chrono::milliseconds(getUpdateInterval())
        );
    }
    setStatus(ExtensionStatus::STOPPED);
    return 0;
}