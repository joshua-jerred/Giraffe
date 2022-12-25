/**
 * @file simulated-extensions.cpp
 * @author Joshua Jerred (github.com/joshua-jerred)
 * @brief This file contains the implementation of the simulated extensions.
 * @details Simulated extensions currently only send data to the data stream.
 * @todo Implement full simulation of the BMP180, DRA818V, DS18B20, and SAM-M8Q
 * This should include configuration identical to the real extensions.
 * @version 0.0.9
 * @date 2022-10-09
 * @copyright Copyright (c) 2022
 */

#include <iostream>
#include <thread>
#include <atomic>
#include <chrono> // For text extension

#include "extension.h"

// See simulated-extensions.h for documentation
TestExtension::TestExtension(DataStream *p_data_stream, 
                             ExtensionMetadata extension_metadata) :
                             Extension(p_data_stream, extension_metadata) {
}
TestExtension::~TestExtension() {
}
int TestExtension::runner() {
    setStatus(ExtensionStatus::RUNNING);
    while (!stop_flag_) {
        std::this_thread::sleep_for(
            std::chrono::milliseconds(getUpdateInterval())
            );
        auto timepoint =  static_cast<int>(duration_cast<std::chrono::milliseconds>(
		std::chrono::system_clock::now().time_since_epoch()).count());

        sendData("time", timepoint);
    }
    setStatus(ExtensionStatus::STOPPED);
    return 0;
}

// See simulated-extensions.h for documentation
BMP180_SIM::BMP180_SIM(DataStream *p_data_stream, ExtensionMetadata extension_metadata) :
    Extension(p_data_stream, extension_metadata) {
}
BMP180_SIM::~BMP180_SIM() {
}
int BMP180_SIM::runner() {
    setStatus(ExtensionStatus::RUNNING);
    int temp = 75;
    int pressure = 1018;
    int upOrDown = 1; // 1 = going up, 0 = going down
    while (!stop_flag_) {
        if (upOrDown == 1) {
            temp -= 1;
            pressure -= 7;
        } else {
            temp += 1;
            pressure += 1;
        }
        if (temp == -50) {
            upOrDown = 0;
        } else if (temp == 75) {
            upOrDown = 1;
        }
        std::this_thread::sleep_for(
            std::chrono::milliseconds(getUpdateInterval())
            );
        sendData("TEMP_F", temp);
        sendData("PRES_M", pressure);
    }
    setStatus(ExtensionStatus::STOPPED);
    return 0;
}

// See simulated-extensions.h for documentation
SAMM8Q_SIM::SAMM8Q_SIM(DataStream *p_data_stream, ExtensionMetadata extension_metadata) :
    Extension(p_data_stream, extension_metadata) {}
SAMM8Q_SIM::~SAMM8Q_SIM() {  
}
int SAMM8Q_SIM::runner() {
    setStatus(ExtensionStatus::RUNNING);
    float lat = 37.1010982;
    float lon = -113.5678354;
    int alt = 0;
    std::string quality = "2";
    int vertical_speed = 5;
    sendData("VERT_SPEED", vertical_speed);
    // ^This should test stale data
    while (!stop_flag_) {
        lat += 0.00001;
        lon += 0.00001;
        alt += 5;
        int horizontal_speed = 2 + (rand() % static_cast<int>(5 - 2 + 1));
        std::this_thread::sleep_for(
            std::chrono::milliseconds(getUpdateInterval())
        );
        sendData("GPS_LAT", lat);
        sendData("GPS_LON", lon);
        sendData("GPS_ALT_M", alt);
        sendData("GPS_QUAL", quality);
        sendData("HORZ_SPEED", horizontal_speed);
    }
    setStatus(ExtensionStatus::STOPPED);
    return 0;
}

// See simulated-extensions.h for documentation
DS18B20_SIM::DS18B20_SIM(DataStream *p_data_stream, ExtensionMetadata extension_metadata) :
    Extension(p_data_stream, extension_metadata) {
}
DS18B20_SIM::~DS18B20_SIM() {}
int DS18B20_SIM::runner() {
    setStatus(ExtensionStatus::RUNNING);
    int temp = 75;
    int upordown = 1; // 1 = going up, 0 = going down
    while (!stop_flag_) {
        std::this_thread::sleep_for(
            std::chrono::milliseconds(getUpdateInterval())
        );
        if (upordown == 1) {
            temp -= 1;
        } else {
            temp += 1;
        }
        if (temp == -50) {
            upordown = 0;
        } else if (temp == 75) {
            upordown = 1;
        }
        sendData("TEMP_C", temp);
    }
    setStatus(ExtensionStatus::STOPPED);
    return 0;
}

// See simulated-extensions.h for documentation
DRA818V_SIM::DRA818V_SIM(DataStream *p_data_stream, ExtensionMetadata extension_metadata) :
    Extension(p_data_stream, extension_metadata) {
}
DRA818V_SIM::~DRA818V_SIM() { 
}
int DRA818V_SIM::runner() {
    setStatus(ExtensionStatus::RUNNING);
    while (!stop_flag_) {
        std::this_thread::sleep_for(
            std::chrono::milliseconds(getUpdateInterval())
        );
    }
    setStatus(ExtensionStatus::STOPPED);
    return 0;
}