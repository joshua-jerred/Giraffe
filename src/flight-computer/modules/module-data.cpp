/**
 * @file module-data.cpp
 * @author Joshua Jerred (github.com/joshua-jerred)
 * @brief This file implements the class DataModule which is defined
 * in module-data.h.
 * @version 0.1
 * @date 2022-10-03
 * @copyright Copyright (c) 2022
 */

#include "module.h"

#include "utility-configurables.h"
#include "utility-config-types.h"

#include "module-data.h"

DataModule::DataModule(ConfigData config_data) {
    std::time_t t = std::time(0);
    std::tm* now = std::localtime(&t);
    
    data_log_file_path_ = DATA_LOG_LOCATION +
    std::to_string(now->tm_year + 1900) + "-" +
    std::to_string(now->tm_mon + 1) + "-" +
    std::to_string(now->tm_mday) + "-" +
    std::to_string(now->tm_hour) + "-" +
    std::to_string(now->tm_min) + "-" +
    std::to_string(now->tm_sec) + ".csv";
    mpDataStream = new DataStream();

    error_log_file_path_ = ERROR_LOG_LOCATION +
    std::to_string(now->tm_year + 1900) + "-" +
    std::to_string(now->tm_mon + 1) + "-" +
    std::to_string(now->tm_mday) + "-" +
    std::to_string(now->tm_hour) + "-" +
    std::to_string(now->tm_min) + "-" +
    std::to_string(now->tm_sec) + ".csv";
    mpDataStream = new DataStream();

    for ( ConfigData::DataTypes::ExtensionDataType data_type : 
        config_data.data_types.types ) { // for each data type in the config file
        addDataTypeToFrame(data_type);
    }
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
void DataModule::log() {
    std::ofstream logfile;
    logfile.open(data_log_file_path_, std::ios_base::app);

    DataFrame dataframe_copy(mpDataStream->getDataFrameCopy());
    
    std::time_t t = std::time(0);
    std::tm* now = std::localtime(&t);

    for (auto& [source_and_unit, packet] : dataframe_copy) {
        logfile << now->tm_hour << ":" << now->tm_min << ":" << now->tm_sec << 
        ", " << packet.source << ", " << packet.unit << ", " << packet.value 
        << std::endl;
    }
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
    for (int i = 0; i < packetCount; i++) {
        /** @todo Check to see if it exists in the dataframe first, if not 
         * add an error.
          */
        dpacket = mpDataStream->getNextDataPacket();
        dataframe_.insert_or_assign(dpacket.source + ":" + dpacket.unit, dpacket);
    }
    mpDataStream->updateDataFrame(dataframe_);
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
    std::time_t now = std::time(NULL);
    for (auto& [source_and_unit, packet] : dataframe_) {  
        if ((int) packet.expiration_time < (int) now) {
            packet.value = "NO-DATA";
        }
    }
}

void DataModule::runner() {
    while (true) {
        std::this_thread::sleep_for(
            std::chrono::milliseconds(MODULE_DATA_FRAME_UPDATE_INTERVAL_MILI_SECONDS)
        );
        parseDataStream();
        parseErrorStream();
        checkForStaleData();
    }
}