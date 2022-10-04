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

#include "module.h"
#include "utility-data-stream.h"


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

    /**
     * @brief Get a pointer to the DataStream object.
     * 
     * @return DataStream*
     */
    DataStream* getDataStream();

    /**
     * @brief Get a Data Snapshot.
     */
    const DataSnapshot getDataSnapshot();


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
};
 /** @} */
#endif