/**
 * @file module-data.cpp
 * @author Joshua Jerred
 * @brief github.com/joshua-jerred
 * @version 0.1 (testing)
 * @date 2022-08-05
 * @copyright Copyright (c) 2022
 * 
 * @brief Common object for passing data and errors between extensions and
 * the extension handler.
 * @details This code contains a DataStream class which has a two vectors that
 * hold data packets and error packets. A common DataStream object is created
 * and then a pointer to the object is sent to all attachments. Extensions will
 * add data and errors to the stream with the 'addData()' and 'addError()'.
 * These data packets and error packets can be grabbed by a common extension
 * handler with 'getNextDataPacket()' and 'getNextErrorPacket()'.
 */

#include <unordered_map> // used by mDataSnapshot
#include <string>
#include <iostream>

#include "module.h"
#include "utility-config-types.h"
#include "utility-data-stream.h"

#include "module-data.h"

DataModule::DataModule(ConfigData config_data) {
    // read data types from config and add them to the data_snapshot,
    // 
    for ( ConfigData::DataTypes::ExtensionDataType data_type : config_data.data_types.types ) {
        mDataSnapshot.insert({data_type.name, "NO-DATA"});
    }
    mpDataStream = new DataStream();
}

DataModule::~DataModule() {
    delete mpDataStream;
}

DataStream* DataModule::getDataStream() {
    return mpDataStream;
}

data_snapshot DataModule::getDataSnapshot() {
    return mDataSnapshot;
}

void DataModule::parseDataStream() {
    int packetCount = mpDataStream->getNumDataPackets();
    data_stream_packet dpacket;
    for (int i = 0; i < packetCount; i++) {
        dpacket = mpDataStream->getNextDataPacket();
        std::cout << dpacket.data_source << " - " << dpacket.data_name << 
            " - " << dpacket.data_value << std::endl;
    }
}

void DataModule::parseErrorStream() {
    int packetCount = mpDataStream->getNumErrorPackets();
    error_stream_packet epacket;
    for (int i = 0; i < packetCount; i++) {
        epacket = mpDataStream->getNextErrorPacket();
        std::cout << "Error: " << epacket.error_source << " - " << 
            epacket.error_name << " - " << epacket.error_info << std::endl;
    }
}

void DataModule::log() {
    std::cout << "DataModule::log() called" << std::endl;
    parseDataStream();
}