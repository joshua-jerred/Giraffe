/**
 * @file module-configuration.h
 * @author Joshua Jerred (github.com/joshua-jerred)
 * @brief This file defines the class ConfigModule and nlohmann json 
 * enumerations.
 * 
 * @version 0.1
 * @date 2022-09-29
 * @copyright Copyright (c) 2022
 */

#ifndef MODULE_CONFIGURATION_H_
#define MODULE_CONFIGURATION_H_

#include <nlohmann/json.hpp>

#include "utility-configurables.h"
#include "utility-config-types.h"

/**
 * @details
 * Acts the same as the standard json object, but this maintains the same 
 * structure as the config file upon loading and saving.
 * Requires nlohmann json version 3.9 or higher.
 */
using json = nlohmann::ordered_json; 

/**
 * @brief The following enumerations are used to link the values
 * in the configuration file with the values of enumerations
 * defined in utility-config-types.h
 * 
 * Important note from the nlohmann json documentation:
 * "When using get<ENUM_TYPE>(), undefined JSON values will default to the first 
 * pair specified in your map. Select this default pair carefully."
 * 
 * This is why the first pair is 'error' so the configuration module can
 * detect an incorrect value in the config file.
 * 
 * @see utility-config-types.h
 */
NLOHMANN_JSON_SERIALIZE_ENUM( ConfigData::MainboardType, {
    {ConfigData::MainboardType::ERROR, "error"}, 
    {ConfigData::MainboardType::OTHER, "other"},
    {ConfigData::MainboardType::PI_ZERO, "pi_zero"},
    {ConfigData::MainboardType::PI_ZERO_W, "pi_zero_w"},
    {ConfigData::MainboardType::PI_2, "pi_2"},
    {ConfigData::MainboardType::PI_3, "pi_3"},
    {ConfigData::MainboardType::PI_4, "pi_4"}
})

NLOHMANN_JSON_SERIALIZE_ENUM( FlightProcedure::ProcType, {
    {FlightProcedure::ProcType::ERROR, "error"},
    {FlightProcedure::ProcType::TESTING, "testing"},
    {FlightProcedure::ProcType::STANDARD, "standard"},
    {FlightProcedure::ProcType::RECOVERY, "recovery"},
    {FlightProcedure::ProcType::FAILSAFE, "failsafe"}
})

NLOHMANN_JSON_SERIALIZE_ENUM( ExtensionMetadata::Category, {
    {ExtensionMetadata::Category::ERROR, "error"},
    {ExtensionMetadata::Category::OTHER, "other"},
    {ExtensionMetadata::Category::RADIO, "radio"},
    {ExtensionMetadata::Category::GPS, "gps"},
    {ExtensionMetadata::Category::CAMERA, "camera"},
    {ExtensionMetadata::Category::INTERNAL_SENSOR, "internal_sensor"},
    {ExtensionMetadata::Category::EXTERNAL_SENSOR, "external_sensor"}
})

NLOHMANN_JSON_SERIALIZE_ENUM( ExtensionMetadata::Interface, {
    {ExtensionMetadata::Interface::OTHER, "other"},
    {ExtensionMetadata::Interface::INTERNAL, "internal"},
    {ExtensionMetadata::Interface::I2C, "i2c"},
    {ExtensionMetadata::Interface::SERIAL, "serial"},
    {ExtensionMetadata::Interface::ONEWIRE, "oneWire"},
    {ExtensionMetadata::Interface::USB, "USB"},
    {ExtensionMetadata::Interface::GPIO, "gpio"}
})

/**
 * @brief ConfigModule is used to load all of the configuration data from the 
 * config file. 
 * @details The configuration is read with Nlohmann JSON and then the data is 
 * placed into a common configuration type that is recognized system wide.
 * During parsing, the configuration module will check for errors and will
 * store them in a vector of strings. The errors can be retrieved with the
 * getErrors() function later. If everything was read and parsed correctly,
 * the getErrors() function will return an empty vector.
 * 
 * Basic usage looks like this:
 * ConfigModule config;
 * config.load("path/to/config.json");
 * config.getErrors(); // check for errors
 * config.getAll(); // get all of the configuration data
 */
class ConfigModule {
public:
    ConfigModule(DataStream *data_stream);
    ~ConfigModule();

    int load(std::string filepath);
    ConfigData getAll();
    json getAllJson();
    int getNumberOfErrors();

private:
    template <typename T>
    void error(std::string error_code, T info);
    void error(std::string error_code, std::string info);
    void error(std::string error_code);


    void parseAll();

    void parseGeneral();
    void parseExtensions();
    void parseDebug();
    void parseTelemetry();
    void parseDataTypes();
    void parseFlightProcedures();

    int number_of_errors_;

    DataStream *p_data_stream_;

    std::string config_file_path_;
    json json_buffer_;
    ConfigData config_data_;
};

#endif // MODULE_CONFIGURATION_H_
