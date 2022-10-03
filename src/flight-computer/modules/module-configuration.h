/**
 * @file module-configuration.h
 * @author Joshua Jerred (github.com/joshua-jerred)
 * @brief This file defines the class ConfigModule and 
 * nlohmann json enumerations.
 * 
 * @version 0.1
 * @date 2022-09-29
 * 
 * @copyright Copyright (c) 2022
 */

#ifndef MODULE_CONFIGURATION_H_
#define MODULE_CONFIGURATION_H_

#include <nlohmann/json.hpp>

#include "utility-configurables.h"
#include "utility-config-types.h"

// Acts the same as the standard json object, but this
// maintains the same structure as the config file.
// Requires nlohmann json version 3.9 or higher.
using json = nlohmann::ordered_json; 

/**
 * @brief The following enumerations are used to link the values
 * in the configuration file with the values of structures
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
    {ConfigData::MainboardType::kError, "error"}, 
    {ConfigData::MainboardType::kOther, "other"},
    {ConfigData::MainboardType::kPi_zero, "pi_zero"},
    {ConfigData::MainboardType::kPi_zero_w, "pi_zero_w"},
    {ConfigData::MainboardType::kPi_2, "pi_2"},
    {ConfigData::MainboardType::kPi_3, "pi_3"},
    {ConfigData::MainboardType::kPi_4, "pi_4"}
})

NLOHMANN_JSON_SERIALIZE_ENUM( FlightLoop::LoopType, {
    {FlightLoop::LoopType::kError, "error"},
    {FlightLoop::LoopType::kTesting, "testing"},
    {FlightLoop::LoopType::kStandard, "standard"},
    {FlightLoop::LoopType::kRecovery, "recovery"},
    {FlightLoop::LoopType::kFailsafe, "failsafe"}
})

NLOHMANN_JSON_SERIALIZE_ENUM( ExtensionMetadata::Category, {
    {ExtensionMetadata::Category::kError, "error"},
    {ExtensionMetadata::Category::kOther, "other"},
    {ExtensionMetadata::Category::kRadio, "radio"},
    {ExtensionMetadata::Category::kGPS, "gps"},
    {ExtensionMetadata::Category::kCamera, "camera"},
    {ExtensionMetadata::Category::kInternalSensor, "internal_sensor"},
    {ExtensionMetadata::Category::kExternalSensor, "external_sensor"}
})

NLOHMANN_JSON_SERIALIZE_ENUM( ExtensionMetadata::Interface, {
    {ExtensionMetadata::Interface::kOther, "other"},
    {ExtensionMetadata::Interface::kInternal, "internal"},
    {ExtensionMetadata::Interface::kI2C, "i2c"},
    {ExtensionMetadata::Interface::kSerial, "serial"},
    {ExtensionMetadata::Interface::kOneWire, "oneWire"},
    {ExtensionMetadata::Interface::kUSB, "USB"},
    {ExtensionMetadata::Interface::kGPIO, "gpio"}
})

/**
 * @brief ConfigModule is used to load all of the configuration data from the 
 * config file. 
 * @test Needs testsing completed (test-01)
 * @details The data is then placed into a common configuration type that is
 * recognized system wide.
 */
class ConfigModule {
public:
    /**
     * @brief Constructs a new Config Module object
     * @param none
     */
    ConfigModule();

    /**
     * @brief Destroys the Config Module object
     */
    ~ConfigModule();

    /**
     * @brief Loads the configuration file at the specified path
     * @param filepath The path to the configuration file, relitive to executable root.
     * @return int - Returns 0 if successful, -1 if not
     * @test
     */
    int load(std::string filepath);

    /**
     * @brief Returns all of the configuration data.
     * @param none
     * @return ConfigData 
     * @test
     */
    ConfigData getAll();

    /**
     * @brief Returns a vector of strings containing the errors that were
     * detected when parsing the configuration file.
     * @param none
     * @return std::vector<std::string> - A vector of strings containing the 
     * errors 
     * @test
     */
    std::vector<std::string> getErrors();

    /**
     * @brief Returns a copy of the json data.
     * @param none
     * @return json - nlohmann json object
     * @test
     */
    json getAllJson();

private:
    /**
     * @brief Calls all of the parse functions to load the configuration data.
     * The return value is the number of errors detected.
     * @param none
     * @return int - The number of errors detected, 0 if none.
     * @test
     */
    int parseAll();
        
    /** @brief Prases General @test */
    void parseGeneral();

    /** @brief Prases Extensions @test */
    void parseExtensions();

    /** @brief Prases Server @test */
    void parseServer();

    /** @brief Prases Telemetry @test */
    void parseTelemetry();

    /** @brief Prases Data Types @test */
    void parseDataTypes();

    /** @brief Flight Loops @test */
    void parseFlightLoops();

    int unsaved_changes_;
    std::string config_file_path_;
    json json_buffer_;
    ConfigData config_data_;
    std::vector<std::string> errors_;
};

#endif