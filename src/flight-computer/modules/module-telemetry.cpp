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
    error_source_ = MODULE_TELEMETRY_PREFIX;

    config_data_ = config_data;

    p_data_stream_ = data_stream;

    tx_number_ = 1;

    call_sign_ = config_data_.telemetry.call_sign;
}

/**
 * @brief Destroy the Telemetry Module:: Telemetry Module object
 * @todo implement this in a safe way.
 */
TelemetryModule::~TelemetryModule() {
    stop();
}

/**
 * @brief This starts the telemetry module thread.
 * @todo not implemented yet.
 */
void TelemetryModule::start() {
    p_data_stream_->addData(MODULE_TELEMETRY_PREFIX,
        "TXQ_SZ", "0", 100);
    p_data_stream_->addData(MODULE_TELEMETRY_PREFIX, 
        "ACTIVE_TX",
        "NONE", 
        0);
    stop_flag_ = 0;
    tx_thread_ = std::thread(&TelemetryModule::runner, this);
}

void TelemetryModule::stop() {
    if (status() == ModuleStatus::STOPPED) {
        return;
    }
    stop_flag_ = 1;
    if (tx_thread_.joinable()) {
        tx_thread_.join();
    }
}

/**
 * @brief Currently implemented with PSK
 * @param None
 * @return Void
 * @todo implement this.
 */
void TelemetryModule::sendDataPacket() {
    DataFrame data = p_data_stream_->getDataFrameCopy();
    std::string message = call_sign_;
    for (const auto & [ unit, packet ] : data) {
        message += unit + ":" + packet.value + "\n";
    }
    Transmission newTX;
    newTX.type = Transmission::Type::PSK;
    newTX.wav_location = generatePSK(message);
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
    newTX.wav_location = generateAFSK(message);
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
    newTX.wav_location = generateAPRS();
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

void TelemetryModule::error(std::string error_code, std::string info) {
	p_data_stream_->addError(error_source_, error_code, info, 0);
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
        error("BAD_TX_TYPE", "");
        return;
    }
    // This will be enabled once file generation is implemented.
    std::ifstream fs(transmission.wav_location);
	if (!fs.good()) {
        p_data_stream_->addError("M_TEL", "BAD_TX_FILE",
        "Attempted to add a transmission with a bad wav file to the transmit"
        " queue: " + transmission.wav_location, 10);
        return;
    }

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
std::string TelemetryModule::generateAFSK(std::string message) {
    return "not-implemented.wav";
}

std::string TelemetryModule::generatePSK(std::string message) {
    std::string file_path = TELEMETRY_WAV_LOCATION + (std::string) "psk-" + std::to_string(getNextTXNumber()) + ".wav";
    
    PSK::Mode mode;
    PSK::SymbolRate symbol_rate;

    std::string requested_mode = config_data_.telemetry.psk_mode;

    if (requested_mode == "bpsk") {
        mode = PSK::BPSK;
    } else if (requested_mode == "qpsk") {
        mode = PSK::QPSK;
    } else {
        error("PSK_M", requested_mode);
        mode = PSK::BPSK; // Default to BPSK
    }

    std::string requested_symbol_rate = config_data_.telemetry.psk_symbol_rate;

    if (requested_symbol_rate == "125") {
        symbol_rate = PSK::S125;
    } else if (requested_symbol_rate == "250") {
        symbol_rate = PSK::S250;
    } else if (requested_symbol_rate == "500") {
        symbol_rate = PSK::S500;
    } else {
        error("PSK_S", requested_symbol_rate);
        symbol_rate = PSK::S125; // Default to 125
    }

    PSK psk = PSK(file_path, mode, symbol_rate);
    if (psk.encodeTextData(message)) {
        return file_path;
    } else {
        error("PSK_E", "Failed to encode PSK data.");
        return "";
    }
}

std::string TelemetryModule::generateAPRS() {
    return "not-implemented.wav";
}

std::string TelemetryModule::generateSSTV() {
    return "not-implemented.wav";
}

void TelemetryModule::runner() {
    while (!stop_flag_) {
        tx_queue_lock_.lock();
        int queue_size = tx_queue_.size();
        tx_queue_lock_.unlock();
        p_data_stream_->addData(MODULE_TELEMETRY_PREFIX,
        "TXQ_SZ", std::to_string(queue_size), 100);
        if (queue_size > 0) {
            tx_queue_lock_.lock();
            Transmission tx = tx_queue_.front();
            std::string command = "aplay " + tx.wav_location;
            tx_queue_.pop();
            tx_queue_lock_.unlock();
            switch (tx.type) {
                case Transmission::Type::AFSK:
                    std::cout << "AFSK TX" << std::endl;
                    p_data_stream_->addData(MODULE_TELEMETRY_PREFIX, 
                        "ACTIVE_TX",
                        "AFSK", 
                        100);
                    //txAFSK(tx.wav_location);
                    std::this_thread::sleep_for(std::chrono::seconds(2));
                    break;
                case Transmission::Type::APRS:
                    std::cout << "APRS TX" << std::endl;
                    p_data_stream_->addData(MODULE_TELEMETRY_PREFIX, 
                        "ACTIVE_TX",
                        "APRS", 
                        100);
                    std::this_thread::sleep_for(std::chrono::seconds(2));
                    //txAPRS(tx.wav_location);
                    break;
                case Transmission::Type::SSTV:
                    std::cout << "SSTV TX" << std::endl;
                    p_data_stream_->addData(MODULE_TELEMETRY_PREFIX, 
                        "ACTIVE_TX",
                        "SSTV", 
                        100);
                    std::this_thread::sleep_for(std::chrono::seconds(2));
                    //p_data_stream_->addLog("M_TEL", "SSTV_TX",
                    //    "Sent SSTV transmission.", 10);
                    //txSSTV(tx.wav_location);
                    break;
                case Transmission::Type::PSK:
                    std::cout << "PSK TX" << std::endl;
                    p_data_stream_->addData(MODULE_TELEMETRY_PREFIX, 
                        "ACTIVE_TX",
                        "PSK", 
                        100);
                    system(command.c_str());
                    //txPSK(tx.wav_location);
                    break;
                default:
                    p_data_stream_->addError(error_source_, "BAD_TX_TYPE",
                    "Attempted to play a transmission with an unknown type." + 
                    std::to_string((int) tx.type), 0);
                    break;
            }
            p_data_stream_->addData(MODULE_TELEMETRY_PREFIX, 
                "ACTIVE_TX",
                "NONE", 
                0);
        }
        std::this_thread::sleep_for(
            std::chrono::milliseconds(TELEMETRY_INTERVAL_MILI_SECONDS)
            );
    }
}