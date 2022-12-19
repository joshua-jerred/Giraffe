/**
 * @file module-telemetry.h
 * @author Joshua Jerred (github.com/joshua-jerred)
 * @brief This file defines the class TelemetryModule
 * @version 0.1
 * @date 2022-10-03
 * @copyright Copyright (c) 2022
 */

#ifndef MODULE_TELEMETRY_H_
#define MODULE_TELEMETRY_H_

#include <mutex>
#include <fstream>
#include <string>
#include <thread>
#include <vector>
#include <queue>
#include <atomic>
#include <iostream>

#include "module.h"

#include "utility-data-stream.h"
#include "utility-configurables.h"
#include "utility-config-types.h"

// Modulation Modes
#include "utility-psk.h"

/**
 * @brief This class is responsible for handling the radio
 * and transmitting data upon request. Currently only supports PSK.
 * @todo Two way communication, AFSK, APRS, etc.
 */
class TelemetryModule : public Module {
public:
    TelemetryModule(ConfigData config_data, DataStream *data_stream);
    TelemetryModule(const TelemetryModule &other) = delete; // No copy constructor
    TelemetryModule &operator=(const TelemetryModule &other) = delete; // No copy assignment
    ~TelemetryModule();

    void start();
    void stop();

    void sendDataPacket();
    void sendAFSK(std::string message);
    void sendPSK(std::string message);
    void sendAPRS();
    void sendSSTVImage();

private:
    int getNextTXNumber();

    void addToTXQueue(Transmission transmission);

    std::string generateAFSK(std::string message);
    std::string generatePSK(std::string message);
    std::string generateAPRS();
    std::string generateSSTV();

    void runner();
    void playWav(std::string wav_location, std::string tx_type, int tx_length);
    FILE *aplay_fp_ = nullptr;


    int tx_number_ = 1;
    std::string call_sign_ = CALLSIGN_FAILSAFE;

    std::thread tx_thread_ = std::thread();
    std::atomic<int> stop_flag_ = 0;

    ConfigData config_data_;
    DataStream *p_data_stream_;

    int psk_length_ = 0;
};

#endif