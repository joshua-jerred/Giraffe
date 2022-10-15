/**
 * @file module-telemetry.cpp
 * @author Joshua Jerred (github.com/joshua-jerred)
 * @brief This fle implements the class TelemetryModule which is defined in
 * module-telemetry.h.
 * 
 * @version 0.0.9
 * @date 2022-10-11
 * @copyright Copyright (c) 2022
 */

#include "module-telemetry.h"

/**
 * @brief Construct a new TelemetryModule object. 
 * 
 * @param config_data All configuration data.
 * @param data_stream A pointer to the data stream.
 */
TelemetryModule::TelemetryModule(ConfigData config_data, DataStream *data_stream) {
    error_source_ = "M_TEL";

    config_data_ = config_data;
    ConfigData::Telemetry telemetry_config_ = config_data_.telemetry; 

    p_data_stream_ = data_stream;

    tx_number_ = 1;
}

/**
 * @brief Destroy the Telemetry Module:: Telemetry Module object
 * @todo implement this in a safe way.
 */
TelemetryModule::~TelemetryModule() {
}

/**
 * @brief This starts the telemetry module thread.
 * @todo not implemented yet.
 */
void TelemetryModule::start() {
}

void TelemetryModule::stop() {
}

/**
 * @brief Adds AX.25 encoded AFSK packets to the telemetry queue.
 * @param None
 * @return Void
 * @todo implement this.
 */
void TelemetryModule::sendDataPacket() {
    DataFrame data = p_data_stream_->getDataFrameCopy();
    std::string message;
    for (const auto & [ unit, packet ] : data) {
        message += unit + ":" + packet.value + ";";
    }
    Transmission newTX;
    newTX.type = Transmission::Type::AFSK;
    //newTX.wav_location = generateAFSK(telemetry_config_.callsign, 
    //    (std::string) TELEMETRY_WAV_LOCATION + "APRS" + 
    //    std::to_string(getNextTXNumber()) + ".wav", message);
    addToTXQueue(newTX);
}

/**
 * @brief Send a raw AFSK message (callsign is added).
 * @param message The message to send.
 * @return void
 */
void TelemetryModule::sendAFSK(std::string message){
    Transmission newTX;
    newTX.type = Transmission::Type::AFSK;
    //newTX.wav_location = generateAFSK(telemetry_config_.callsign, 
    //    (std::string) TELEMETRY_WAV_LOCATION + "AFSK" + 
    //    std::to_string(getNextTXNumber()) + ".wav", message);
    addToTXQueue(newTX);
}

/**
 * @brief Pulls data from the data snapshot and then
 * uses the AFSK utility to create an APRS packet.
 * @todo currently not implemented
 */
void TelemetryModule::sendAPRS() {
    DataFrame data = p_data_stream_->getDataFrameCopy();
    //std::string lat = data["GLAT"];
    //std::string lon = data["GLON"];
    //std::string alt = data["GALT"];
    Transmission newTX;
    newTX.type = Transmission::Type::APRS;
    //newTX.wav_location = generateAPRS();
    addToTXQueue(newTX);
}

/**
 * @brief Adds an SSTV image to the transmit queue.
 * It will use the path of the most recent picture from
 * the data snapshot, along with environmental data.
 */
void TelemetryModule::sendSSTVImage() {
    Transmission newTX;
    newTX.type = Transmission::Type::SSTV;
    //newTX.wav_location = generateSSTV();
    addToTXQueue(newTX);
}

/**
 * @brief Incremented the tx number and returns the new value.
 * @return int 
 */
int TelemetryModule::getNextTXNumber() {
    return ++tx_number_;
}

/**
 * @brief First checks the transmission for type, and verifies
 * that the wav file actually exists. If both are good,
 * it will acquire a lock on the transmission queue, add the
 * data, then unlock it.
 * @param transmission The transmission to add to the queue. 
 * @return void
 * @see Transmission
 * @todo Fully implement the queue
 */
void TelemetryModule::addToTXQueue(Transmission transmission) {
    if (transmission.type == Transmission::Type::ERROR) {
        p_data_stream_->addError("M_TEL", "BAD_TX_TYPE",
        "Attempted to add a transmission with an unknown type to the transmit"
        "queue.", 0);
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

/**
 * @brief Takes the callsign, output location, and the message to be encoded.
 * Generates an AX.25 encoded AFSK message that will be split into packets.
 * @todo Not Yet Implemented
 * @return File path to the generated wav file.
 */
std::string TelemetryModule::generateAFSK(std::string callsign, 
    std::string wav_location, std::string message) {
        return "not implemented";
}