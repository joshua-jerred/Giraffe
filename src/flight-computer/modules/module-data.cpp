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
    data_stream_packet packet;
    for (int i = 0; i < packetCount; i++) {
        packet = mpDataStream->getNextDataPacket();
    }
}

void DataModule::log( ) {
    // Currently not implemented, but will be used to log data to a file.
    // This will be called by the main loop.
    // For now it will just print the data to the console.
    std::cout << "DataModule::log() called" << std::endl;
    for (const auto & [ key, value ] : mDataSnapshot)  {
        std::cout << key << ": " << value << std::endl;
    }
}

