/**
 * @file module-data.h
 * @author Joshua Jerred (github.com/joshua-jerred)
 * @brief This file defines the DataService and DataStream objects.
 * @version 0.1
 * @date 2022-09-20
 * 
 * @copyright Copyright (c) 2022
 */
#ifndef MODULE_DATA_H_
#define MODULE_DATA_H_

#include "module.h"

typedef std::unordered_map<std::string, std::string> data_snapshot;

/**
 * @brief The DataService class is responsible for sensor data flow. It collects
 * all of the data into "frames". A frame is a snapshot of the most recent data
 * that has been pulled from the sensors. The DataService is also responsible
 * for the data log and error log files.
 * @addtogroup flight-computer-modules
 * @{
 */
class DataModule : public Module {
public:

    /**
     * @brief Construct a new Data Service object. Initializes the DataStream.
     */
    DataModule(ConfigData config_data);

    /**
     * @brief Stops the DataService and deconstruct the DataService object. 
     * Attempts safe shutdown of all services.
     */
    ~DataModule();

    /**
     * @brief Get a pointer to the DataStream object.
     * 
     * @return DataStream*
     */
    DataStream* getDataStream();

    /**
     * @brief Get a Data Snapshot.
     */
    data_snapshot getDataSnapshot();


    /**
     * @brief This function will log the data snapshot to the data log file.
     * @todo Implement this function.
     */
    void log();

private:

    /**
     * @brief This function will get the current number of
     * data packets before reading them all from the dataStream.
     * This means that if another thread manages to add a packet
     * to the dataStream while it's being parsed, it will not be
     * read until the next time this function is called.
     */
    void parseDataStream();

    void parseErrorStream();

    DataStream *mpDataStream;
    data_snapshot mDataSnapshot;
};
 /** @} */
#endif