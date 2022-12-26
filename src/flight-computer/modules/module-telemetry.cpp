/**
 * @file module-telemetry.cpp
 * @author Joshua Jerred (github.com/joshua-jerred)
 * @brief This fle implements the class TelemetryModule which is defined in
 * module-telemetry.h.
 * 
 * @version 0.1.0
 * @date 2022-10-11
 * @copyright Copyright (c) 2022
 */

#include <mutex>
#include <fstream>
#include <string>
#include <thread>
#include <vector>
#include <queue>
#include <atomic>
#include <iostream>

#include "modules.h"
using namespace modules;

#include "utility-psk.h" // TODO: Remove this and use MWAV

/**
 * @brief Construct a new TelemetryModule object. 
 * 
 * @param config_data All configuration data.
 * @param data_stream A pointer to the data stream.
 */
TelemetryModule::TelemetryModule(ConfigData config_data, DataStream *stream):
    Module(stream, MODULE_TELEMETRY_PREFIX),
    config_data_(config_data),
    p_data_stream_(stream) {

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
        module_status_ = ModuleStatus::STOPPED;
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
    std::string message;
    message += "\n\n" + call_sign_ + "\n";
    message += "automated message - data to follow\n";
    for (const auto & [ unit, packet ] : data) {
        message += unit + ":" + packet.value + "\n";
    }
    message += call_sign_ + "\n";
    message.push_back((char) 4); // End of Transmission character

    std::string message_lower;
    for(auto c : message)
        message_lower.push_back(std::tolower(c));

    Transmission newTX;
    newTX.type = Transmission::Type::PSK;
    newTX.wav_location = generatePSK(message_lower);
    newTX.length = psk_length_;
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
   // addToTXQueue(newTX);
}

/**
 * @brief Pulls data from the data snapshot and then
 * uses the AFSK utility to create an APRS packet.
 * @todo currently not implemented
 */
void TelemetryModule::sendAPRS() {
    DataFrame data = p_data_stream_->getDataFrameCopy();
    //std::string lat = data["LAT"];
    //std::string lon = data["LON"];
    //std::string alt = data["ALT"];
    Transmission newTX;
    newTX.type = Transmission::Type::APRS;
    newTX.wav_location = generateAPRS();
    //addToTXQueue(newTX);
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
    //addToTXQueue(newTX);
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
        transmission.wav_location, 10);
        return;
    }

    p_data_stream_->addToTxQueue(transmission);
}

/**
 * @brief Takes the callsign, output location, and the message to be encoded.
 * Generates an AX.25 encoded AFSK message that will be split into packets.
 * @todo Not Yet Implemented
 * @return File path to the generated wav file.
 */
std::string TelemetryModule::generateAFSK(std::string message) {
    (void) message;
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
        psk_length_ = psk.getLength();
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
    module_status_ = ModuleStatus::RUNNING;
    while (!stop_flag_) {
        int queue_size = p_data_stream_->getTXQueueSize();

        p_data_stream_->addData(MODULE_TELEMETRY_PREFIX,
        "TXQ_SZ", std::to_string(queue_size), 100);

        if (queue_size > 0) {

            Transmission tx = p_data_stream_->getNextTX();
            std::string tx_length = std::to_string(tx.length);

            switch (tx.type) {
                case Transmission::Type::AFSK:

                    std::this_thread::sleep_for(std::chrono::seconds(2)); // Will be removed once implemented
                    break;
                case Transmission::Type::APRS:
 
                    std::this_thread::sleep_for(std::chrono::seconds(2)); // Will be removed once implemented
                    break;
                case Transmission::Type::SSTV:

                    std::this_thread::sleep_for(std::chrono::seconds(2)); // Will be removed once implemented
                    break;
                case Transmission::Type::PSK:
                    playWav(tx.wav_location, "PSK", tx.length);
                    break;
                default:
                    error("BAD_TX_TYPE", std::to_string((int) tx.type));
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

void TelemetryModule::playWav(std::string wav_location, std::string tx_type, int tx_length) {
    std::string command = "aplay " + wav_location + " >nul 2>nul"; // command to play with aplay suppress output

    p_data_stream_->addData(MODULE_TELEMETRY_PREFIX,
        "ACTIVE_TX",
        tx_type + std::to_string(tx_length), 
        tx_length);

    /**
     * @details aplay seems to return 0 when it's done playing, 256 when
     * the file doesn't exist, and 2 when it was interrupted (ctrl+c).
     * 
     * This allows me to stop a transmission early if needed and provides
     * time to exit GFS with another ctrl+c.
     * 
     * This is definitely a bit of a hack but it gives the functionality
     * that I'm looking for. Eventually this could be replaced with popen
     * or a built in library for playing the wav files. I have had no issues
     * with this *so far*.
     */
    int exit_code = system(command.c_str());
    if (exit_code == 2) {
        std::cout << "Stopping Transmission Early - CTRL+C to exit GFS" << std::endl;
        std::this_thread::sleep_for(
            std::chrono::milliseconds(1000) // Delay for 1 second
        );
    } else if (exit_code != 0) {
        error("APLAY_EXIT_CODE_" + std::to_string(exit_code), wav_location);
    }
}