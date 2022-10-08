/**
 * @file module-configuration.cpp
 * @author Joshua Jerred (github.com/joshua-jerred)
 * @brief This defines the class ConfigModule.
 * @version 0.1
 * @date 2022-10-03
 * @copyright Copyright (c) 2022
 */
#ifndef MODULE_DATA_H_
#define MODULE_DATA_H_

#include <thread>

#include "module.h"
#include "utility-data-stream.h"
#include "utility-data-snapshot.h"


/**
 * @brief The DataModule class is responsible for managing all data between
 * modules. It is responsible for creating safe structures that different
 * concurently running threads can access. It includes primarily a DataStream
 * and a DataSnapshot. The DataStream is a queue that all of the extensions have
 * access to. They can add data whenever they want. The data module is 
 * responsible for collecting this data from the stream. This data is then
 * either logged, or used to update the DataSnapshot. The DataSnapshot is a
 * map of containing the values and names of each 'data type' that the extensions
 * collect. The snapshot only contains a single value of each data type, the
 * most up to date value.
 * 
 * The DataModule is also reposonsible for logging the data in the snapshot when 
 * it is requested to do so.
 * 
 * The DataModule also logs errors.
 * 
 * The data and error directories can be set before congifuration in 
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

    void start();

    void stop();

    /**
     * @brief Get a pointer to the DataStream object.
     * 
     * @return DataStream*
     */
    DataStream* getDataStream();

    /**
     * @brief Get a Data Snapshot.
     */
    DataSnapshot getDataSnapshot();


    /**
     * @brief This function will log the data snapshot to the data log file.
     * @todo Implement this function.
     */
    void log();

private:

    void addDataTypeToFrame(ConfigData::DataTypes::ExtensionDataType data_type); // add a data type to the data frame

    void checkForStaleData(); // check for stale data in the data frame

    void parseDataStream();

    void parseErrorStream();

    void runner();

    DataStream *mpDataStream;

    // DataFrame format: <"source:unit", DataStreamPacket>
    typedef std::unordered_map<std::string, DataStreamPacket> DataFrame;
    std::mutex dataframe_mutex_;
    DataFrame dataframe_;

    std::thread runner_thread_;
};
 /** @} */
#endif