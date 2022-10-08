/**
 * @file module-data.cpp
 * @author Joshua Jerred (github.com/joshua-jerred)
 * @brief This file implements the class DataModule which is defined
 * in module-data.h.
 * @version 0.1
 * @date 2022-10-03
 * @copyright Copyright (c) 2022
 */

#include <unordered_map> // used by mDataSnapshot
#include <string>
#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>

#include "module.h"

#include "utility-configurables.h"
#include "utility-config-types.h"
#include "utility-data-stream.h"

#include "module-data.h"

DataModule::DataModule(ConfigData config_data) {
    mpDataStream = new DataStream();

    dataframe_mutex_.lock();
    for ( ConfigData::DataTypes::ExtensionDataType data_type : 
        config_data.data_types.types ) { // for each data type in the config file
        addDataTypeToFrame(data_type);
    }
    dataframe_mutex_.unlock();
}

DataModule::~DataModule() {
    delete mpDataStream;
}

void DataModule::start() {
    runner_thread_ = std::thread(&DataModule::runner, this);
}

void DataModule::stop() {

}

DataStream* DataModule::getDataStream() {
    return mpDataStream;
}

/**
 * @todo Implement this function.
 */
DataSnapshot DataModule::getDataSnapshot() {
    DataSnapshot snapshot;
    for (auto& [source_and_unit, packet] : dataframe_) {  
        snapshot.insert_or_assign(source_and_unit, packet.value);
    }
    return snapshot;
}

/**
 * @todo Implement this function.
 */
void DataModule::log() {
    
}


void DataModule::addDataTypeToFrame(ConfigData::DataTypes::ExtensionDataType data_type) {
    DataStreamPacket packet;
    packet.source = data_type.source;
    packet.unit = data_type.unit;
    packet.value = "NODATA";
    packet.expiration_time = 0;
    dataframe_.insert_or_assign(data_type.source + ":" + data_type.unit, packet);
}


void DataModule::parseDataStream() {
    int packetCount = mpDataStream->getNumDataPackets();
    DataStreamPacket dpacket;
    dataframe_mutex_.lock();
    for (int i = 0; i < packetCount; i++) {
        /** @todo Check to see if it exists in the dataframe first, if not 
         * add an error.
          */
        dpacket = mpDataStream->getNextDataPacket();
        dataframe_.insert_or_assign(dpacket.source + ":" + dpacket.unit, dpacket);
    }
    dataframe_mutex_.unlock();
}

void DataModule::parseErrorStream() {
    int packetCount = mpDataStream->getNumErrorPackets();
    ErrorStreamPacket epacket;
    for (int i = 0; i < packetCount; i++) {
        epacket = mpDataStream->getNextErrorPacket();
        std::cout << "Error: " << epacket.error_source << " - " << 
            epacket.error_name << " - " << epacket.error_info << std::endl;
    }
}

void DataModule::checkForStaleData() {
    //std::time_t now = std::time(NULL);
    /** @todo Implement this. */
}

void DataModule::runner() {
    while (true) {
        std::this_thread::sleep_for(
            std::chrono::milliseconds(MODULE_DATA_FRAME_UPDATE_INTERVAL_MILI_SECONDS)
        );
        parseDataStream();
        parseErrorStream();
        checkForStaleData();
        sleep(1);
    }
}