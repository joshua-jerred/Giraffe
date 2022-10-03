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

#include "module.h"

#include "utility-config-types.h"

struct Transmission {
    enum class Type {
        kError = 0,
        kAPRS = 1,
        kAFSK = 2,
        kSSTV = 3,
        kAPRS = 4 
    };
    Type type;
    std::string wav_location;
};


/**
 * @brief This class is responsible for handling the radio
 * and transmitting data upon request.
 */
class TelemetryModule : public Module {
public:
    TelemetryModule(ConfigData config_data, DataStream *data_stream);
    ~TelemetryModule();

    void start();
    void stop();

    void txDataPacket();
    
    void txAFSK();
    void txAPRS();
    void txSSTVImage();
    void txSSDVImage();

private:
    void addToTXQueue(Transmission transmission);
    void getNextTXQueueItem();
    
    std::mutex tx_queue_lock_;
    std::queue<Transmission> tx_queue_;

    ConfigData config_data_;
    DataStream *data_stream_;
};

#endif