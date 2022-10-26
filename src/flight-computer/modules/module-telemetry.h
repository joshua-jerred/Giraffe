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

struct Transmission {
    enum class Type {
        ERROR = 0,
        APRS = 1,
        AFSK = 2,
        SSTV = 3,
    };
    Type type;
    std::string wav_location;
};


/**
 * @brief This class is responsible for handling the radio
 * and transmitting data upon request.
 * @todo This is hardly implemented.
 */
class TelemetryModule : public Module {
public:
    TelemetryModule(ConfigData config_data, DataStream *data_stream);
    ~TelemetryModule();

    void start();
    void stop();

    void sendDataPacket();
    void sendAFSK(std::string message);
    void sendAPRS();
    void sendSSTVImage();

private:
    int getNextTXNumber();

    void addToTXQueue(Transmission transmission);
    void getNextTXQueueItem();

    std::string generateAFSK(std::string message);
    std::string generateAPRS();
    std::string generateSSTV();

    void runner();
    
    int tx_number_;

    std::mutex tx_queue_lock_;
    std::queue<Transmission> tx_queue_;

    std::thread tx_thread_;
    std::atomic <int> stop_flag_;

    ConfigData config_data_;
    DataStream *p_data_stream_;
};

#endif