/**
 * @file module-data.h
 * @author Joshua Jerred (github.com/joshua-jerred)
 * @brief This file declares the class DataModule.
 * @version 0.1
 * @date 2022-10-03
 * @copyright Copyright (c) 2022
 */
#ifndef MODULE_DATA_H_
#define MODULE_DATA_H_

#include <unordered_map>
#include <string>
#include <iostream>
#include <fstream>
#include <thread>
#include <mutex>
#include <chrono>
#include <ctime>
#include <atomic>

#include "utility-data-stream.h"
#include "utility-configurables.h"
#include "utility-config-types.h"

#include "modules.h"

/**
 * @brief The DataModule class is responsible for managing all data between
 * modules. It is responsible for creating safe structures that different
 * concurrently running threads can access. It is in charge of the
 * DataStream. The DataStream is a queue that all of the extensions have
 * access to. They can add data whenever they want. The data module is 
 * responsible for collecting this data from the stream and placing it into a
 * data frame which is then sent to the datastream so other modules can access 
 * it.
 * This module also continually goes through the DataFrame looking for stale
 * data. If it finds stale data, it will set it's value to 'NO_DATA'.
 * This is an indication of an error within an extension.
 * 
 * The DataModule is also responsible for logging the data in the dataframe
 * to a log file when requested to do so.
 * 
 * The DataModule also logs errors which are collected through the datastream
 * in the same way as the data.
 * 
 * The data and error log directories can be set inside of 
 * 'utility-configurables.h'.
 */
class DataModule : public Module {
public:
    DataModule();
    DataModule(const DataModule&) = delete; // No copy constructor
    DataModule& operator=(const DataModule&) = delete; // No copy assignment
    ~DataModule();

    void addConfigData(ConfigData config_data);

    void start();
    void stop();

    DataStream* getDataStream();
    DataFrame getSnapshot();
    void log();

private:
    void addDataTypeToFrame(ConfigData::DataTypes::ExtensionDataType data_type); // add a data type to the data frame
    
    void checkForStaleData(); // check for stale data in the data frame
    void parseDataStream();

    void checkForStaleErrors();
    void parseErrorStream();
    
    void runner();

    std::string data_log_file_path_ = "";
    std::string error_log_file_path_ = "";

    DataStream *p_data_stream_ = nullptr;

    DataFrame dataframe_ = DataFrame();
    ErrorFrame errorframe_ = ErrorFrame();

    std::atomic<int> shutdown_signal_ = 0;
    std::thread runner_thread_ = std::thread();
};
#endif // MODULE_DATA_H_
