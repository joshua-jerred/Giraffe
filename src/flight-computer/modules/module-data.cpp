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

#include "module.h"
#include "utility-config-types.h"
#include "utility-data-stream.h"

#include "module-data.h"

DataModule::DataModule(ConfigData config_data) {
    mpDataStream = new DataStream();

    for ( ConfigData::DataTypes::ExtensionDataType data_type : config_data.data_types.types ) {
        mDataSnapshot.insert({data_type.name, "NO-DATA"});
    }
}

DataModule::~DataModule() {
    delete mpDataStream;
}

DataStream* DataModule::getDataStream() {
    return mpDataStream;
}

void DataModule::parseDataStream() {
    int packetCount = mpDataStream->getNumDataPackets();
    DataStreamPacket dpacket;
    for (int i = 0; i < packetCount; i++) {
        dpacket = mpDataStream->getNextDataPacket();
        std::cout << dpacket.data_source << " - " << dpacket.data_name << 
            " - " << dpacket.data_value << std::endl;
    }
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

const DataSnapshot DataModule::getDataSnapshot() {
    return mpDataStream->getSnapshot();
}

void DataModule::log() {
    std::cout << "DataModule::log() called" << std::endl;
    parseDataStream();
}