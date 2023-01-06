/**
 * @file module-telemetry.cpp
 * @author Joshua Jerred (github.com/joshua-jerred)
 * @brief This fle implements the class TelemetryModule.
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
#include <filesystem>

#include "modules.h"
using namespace modules;

#include "mwav.h"

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

    tx_number_ = 0; // First tx id will be 1

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
 */
void TelemetryModule::sendDataPacket() {
    int tx_num = getNextTXNumber();

    DataFrame data = p_data_stream_->getDataFrameCopy();
    std::string message;
    message += "\n\n" + call_sign_ + "\n";
    message += "automated message - data to follow\n";
    message += "pkt_id:" + std::to_string(tx_num) + "\n";

    // Add data (specified in config) to the message that will be sent.
    std::string key = "";
    for (ConfigData::DataTypes::DataType type : config_data_.data_types.types) {
        if (type.include_in_telemetry) {
            key = type.source + ":" + type.unit;
            if (data.contains(key)) {
                message += type.telemetry_name + ":" + data[key].value + "\n";
            }
        }
    }

    //for (const auto & [ unit, packet ] : data) { 
    //    message += unit + ":" + packet.value + "\n";
    //}

    message += call_sign_ + "\n";
    message.push_back((char) 4); // End of Transmission character

    std::string message_lower;
    for(auto c : message)
        message_lower.push_back(std::tolower(c));

    Transmission newTX;
    newTX.type = Transmission::Type::PSK;
    newTX.tx_num = tx_num;
    newTX.wav_location = generatePSK(message_lower, newTX.tx_num);
    newTX.message = message;
    newTX.length = psk_length_;
    addToTXQueue(newTX); // If the TX failed to generate the error must be handled somewhere else.
}

/**
 * @brief Send a raw AFSK message (callsign is added).
 * @param message The message to send.
 * @return void
 */
void TelemetryModule::sendAFSK(std::string message){
    Transmission newTX;
    newTX.type = Transmission::Type::AFSK;
    newTX.tx_num = getNextTXNumber();
    newTX.wav_location = generateAFSK(message, newTX.tx_num);
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
std::string TelemetryModule::generateAFSK(const std::string &message, const int tx_number) {
    (void) message;
    (void) tx_number;
    return "not-implemented.wav";
}

std::string TelemetryModule::generatePSK(const std::string &message, const int tx_number) {
    std::string file_path = TELEMETRY_WAV_LOCATION + (std::string) "psk-" + std::to_string(tx_number) + ".wav";
    
    MWAVData::MODULATION mode = MWAVData::MODULATION::BPSK_125; // Default

    std::string requested_mode = config_data_.telemetry.psk_mode;
    if (requested_mode == "BPSK125") {
        mode = MWAVData::MODULATION::BPSK_125;
    } else if (requested_mode == "bpsk250") {
        mode = MWAVData::MODULATION::BPSK_250;
    } else if (requested_mode == "bpsk500") {
        mode = MWAVData::MODULATION::BPSK_500;
    } else if (requested_mode == "bpsk1000") {
        mode = MWAVData::MODULATION::BPSK_1000;
    } 

    bool res = MWAVData::encode(mode, message, file_path);
    
    if (res) {
        psk_length_ = 10; // TODO: Implement this
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


        data("TX_Q_SZ", queue_size, 5);


        // Report the info about the TX log
        DataStream::TXLogInfo lg = p_data_stream_->getTXLogInfo();
        data("TX_LG_SZ", lg.tx_log_size, 5);
        data("TX_LG_FRST", lg.first_tx_in_log, 5);
        data("TX_LG_LAST", lg.last_tx_in_log, 5);

        // Manage the TX queue
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

    // Delete the wav file, it's no longer needed as the data is in the log
    if (!std::filesystem::remove(wav_location)) {
        error("DEL_WAV", wav_location);
    }
}