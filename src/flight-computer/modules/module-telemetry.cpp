/**
 * @file module-telemetry.cpp
 * @author Joshua Jerred (github.com/joshua-jerred)
 * @brief This file implements the class TelemetryModule
 * which is defined in module-telemetry.h
 * @version 0.1
 * @date 2022-10-03
 * 
 * @copyright Copyright (c) 2022
 */
#include <mutex>
#include <fstream>

//#include "utility-afsk-encoder.h"
#include "utility-data-stream.h"

#include "module-telemetry.h"


/**
 * @brief Construct a new TelemetryModule object. 
 * 
 * @param config_data 
 */
TelemetryModule::TelemetryModule(ConfigData config_data, DataStream *data_stream) {
    config_data_ = config_data;
    data_stream_ = data_stream;
}

TelemetryModule::~TelemetryModule() {
}

void TelemetryModule::start() {
}

void TelemetryModule::stop() {
}

/**
 * @brief Adds AFSK encoded data packet, containing data
 * from the data snapshot, to the transmit queue.
 */
void TelemetryModule::txDataPacket() {
    DataSnapshot data = data_stream_->getSnapshot();

}

/**
 * @brief Pulls data from the data snapshot and then
 * uses the AFSK utility to create an AX.25 packet.
 */
void TelemetryModule::txAPRS() {
    
}

/**
 * @brief Adds an SSTV image to the transmit queue.
 * It will use the path of the most recent picture from
 * the data snapshot, along with environmental data from there.
 */
void TelemetryModule::txSSTVImage() {

}

/**
 * @todo Not yet implemented, low priority.
 */
void TelemetryModule::txSSDVImage() {

}

/**
 * @brief First checks the transmission for type, and verifies
 * that the wav file actually exists. If both are good,
 * it will aquire a lock on the transmission queue, add the
 * data, then unlock it.
 * @param transmission 
 * @return none
 */
void TelemetryModule::addToTXQueue(Transmission transmission) {
    if (transmission.type == Transmission::Type::kError) {
        data_stream_->addError("TELEMETRY", "BAD_TX_TYPE",
        "Attempted to add a transmission with an unknown type to the transmit"
        "queue.");
        return;
    }
    std::ifstream fs(transmission.wav_location);
	if (fs.fail()) {
        fs.close();
		return;
	}
    fs.close();

    tx_queue_lock_.lock();
    tx_queue_.push(transmission);
    tx_queue_lock_.unlock();
}