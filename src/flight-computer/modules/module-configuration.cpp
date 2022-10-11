/**
 * @file module-configuration.cpp
 * @author Joshua Jerred (github.com/joshua-jerred)
 * @brief This file implements the class ConfigModule which is defined
 * in module-configuration.h.
 * @version 0.1
 * @date 2022-10-03
 * @copyright Copyright (c) 2022
 */

#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <regex>

#include <nlohmann/json.hpp>

#include "module.h"

#include "module-configuration.h"

using json = nlohmann::ordered_json;

/**
 * @brief Construct a new ConfigModule::ConfigModule object
 * @param None
 */
ConfigModule::ConfigModule() {
}

/**
 * @brief DestroyS the ConfigModule::ConfigModule object
 */
ConfigModule::~ConfigModule() {
}

/**
 * @brief Load and parse a configuration file from a given path.
 * @details This function will load and parse a json file from a given path.
 * If the file fails to open or if it does not exists it will return -1.
 * If it does load, it will call parseAll()
 * @param file_path The local path to the configuration file.
 * @return int -1 if the file could not be opened, 0 if the file was loaded.
 * @see parseAll()
 */
int ConfigModule::load(std::string file_path) {
	config_file_path_ = file_path;
	std::ifstream fs(config_file_path_);
	if (fs.fail()) {
		return -1;
	}
	json_buffer_ = json::parse(fs);
	parseAll();
	return 0;
}

/**
 * @brief Returns all of the configuration data as a ConfigData object. All
 * errors will be included, refer to getErrors() for a list of errors.
 * @param None
 * @return ConfigData - All of the configuration data.
 */
ConfigData ConfigModule::getAll() {
	return config_data_;
}

/**
 * @brief getErrors returns a vector of strings containing the errors that were
 * detected when parsing the configuration file.
 * @details This function returns human readable errors.
 * @todo The errors need to be more similar to the errors in other modules that
 * report to the datastream.
 * @param None
 * @return std::vector<std::string> 
 */
std::vector<std::string> ConfigModule::getErrors() {
	return errors_;
}

/**
 * @brief Returns a copy of the json data.
 * @param None
 * @return json - nlohmann json object
 */
json ConfigModule::getAllJson() {
	return json_buffer_;
}

/**
 * @brief Calls all of the parse functions to load the configuration data.
 * @param None
 * @return void
 */
void ConfigModule::parseAll() {
	parseGeneral();
	parseExtensions();
	parseDebug();
	parseTelemetry();
	parseDataTypes();
	parseFlightLoops();
}

/**
 * @brief Parses the general section of the configuration file.
 * @details This function pulls out the project name, mainboard type, and
 * starting loop type. It will add errors to the errors_ vector if any of the
 * required fields contain errors.
 * @param None
 * @return void
 * @todo Character type limits on the project name.
 */
void ConfigModule::parseGeneral() { 
	std::string name = json_buffer_["general"]["name"].get<std::string>();
	if (name.length() < PROJECT_NAME_MIN_LENGTH || 
	name.length() > PROJECT_NAME_MAX_LENGTH) {
		errors_.push_back("Project name must be between " + 
		std::to_string(PROJECT_NAME_MIN_LENGTH) + " and " + 
		std::to_string(PROJECT_NAME_MAX_LENGTH) + " characters.");
	} else {
		config_data_.general.project_name = name;
	}

	ConfigData::MainboardType mbtype = 
	json_buffer_["general"]["main-board-type"].get<ConfigData::MainboardType>();
	if (mbtype == ConfigData::MainboardType::ERROR) {
		errors_.push_back("Invalid main board type.");
	} else {
		config_data_.general.main_board = mbtype;
	}
	
	FlightLoop::LoopType ltype = 
	json_buffer_["general"]["starting-loop"].get<FlightLoop::LoopType>();
	if (ltype == FlightLoop::LoopType::ERROR) {
		errors_.push_back("Invalid starting loop type.");
	} else {
		config_data_.general.starting_loop = ltype;
	}
}

/**
 * @brief Parses the extensions section of the configuration file.
 * @details generates a vector of extension metadata objects. It checks for
 * sequential IDs, a name length, extension type, etc.
 * @param None
 * @return void
 * @todo type does not check for valid types.
 * @todo additional arguments for i2c, serial, etc. This should be changed into
 * a vector of string arguments to send to the extension as it's most flexible.
 * with all type of extensions.
 */
void ConfigModule::parseExtensions() {
	int number_of_extensions = 0;
	for (const auto& item : json_buffer_["extensions"].items()) {
		ExtensionMetadata newExtension;
		
		int id = item.value()["id"].get<int>();
		if (id != number_of_extensions + 1) { 
			errors_.push_back("Extension IDs must be sequential, starting at 1.");
		} else {
			newExtension.id = id;
		}

		std::string name = item.value()["name"].get<std::string>();
		if (name.length() < EXTENSION_NAME_MIN_LENGTH ||
		name.length() >= EXTENSION_NAME_MAX_LENGTH) {
			errors_.push_back("Extension name " + name + " must be between " + 
			std::to_string(EXTENSION_NAME_MIN_LENGTH) + " and " + 
			std::to_string(EXTENSION_NAME_MAX_LENGTH) + " characters.");
		} else {
			newExtension.name = name;
		}

		std::string etype = item.value()["type"].get<std::string>();
		if (etype.length() < EXTENSION_NAME_MIN_LENGTH ||
		etype.length() > EXTENSION_NAME_MAX_LENGTH) {
			errors_.push_back("Extension name " + etype + " must be between " + 
			std::to_string(EXTENSION_NAME_MIN_LENGTH) + " and " + 
			std::to_string(EXTENSION_NAME_MAX_LENGTH) + " characters.");
		} else {
			newExtension.extension_type = etype;
		}

		ExtensionMetadata::Category category =
		item.value()["category"].get<ExtensionMetadata::Category>();
		if (category == ExtensionMetadata::Category::ERROR) {
			errors_.push_back("Invalid extension category.");
		} else {
			newExtension.category = category;
		}

		ExtensionMetadata::Interface interface =
		item.value()["interface"].get<ExtensionMetadata::Interface>();
		if (interface == ExtensionMetadata::Interface::ERROR) {
			errors_.push_back("Invalid extension interface.");
		} else {
			if (interface == ExtensionMetadata::Interface::ONEWIRE) {
				std::string address = item.value()["address"].get<std::string>();
				if (!std::regex_search(address, std::regex("28-[0-9&a-f]{12}"))) {
					errors_.push_back("OneWire address must match format."
									  " It currently is: " + address);
				} else {
					newExtension.address = address;
				}
			} else if (interface == ExtensionMetadata::Interface::I2C) {
				std::string address = item.value()["address"].get<std::string>();
				int address_num = -1;
				std::stringstream strs;
				strs << std::hex << address; // convert hex string to int
				strs >> address_num;

				if (address_num < 0 || address_num > 127) {
					errors_.push_back("I2C address must be between 0 and 127. "
									  "It must be in hex format without 0x. "
									  "It currently is: " + address);
				} else {
					newExtension.address = address;
				}
			}
			newExtension.interface = interface;
		}

		int interval = item.value()["update-interval"].get<int>();
		if (interval < EXTENSION_INTERVAL_MIN || 
		interval > EXTENSION_INTERVAL_MAX) {
			errors_.push_back("Extension interval must be between " + 
			std::to_string(EXTENSION_INTERVAL_MIN) + " and " + 
			std::to_string(EXTENSION_INTERVAL_MAX) + " ms.");
		} else {
			newExtension.update_interval = interval;
		}

		int flight_critical = item.value()["flight-critical"].get<int>();
		if (flight_critical != 0 && flight_critical != 1) {
			errors_.push_back("Extension flight-critical must be 0 or 1.");
		} else {
			newExtension.critical = flight_critical;
		}

		if (newExtension.category == ExtensionMetadata::Category::RADIO) {
			// Check for radio-specific fields
		}

		config_data_.extensions.extensions_list.push_back(newExtension);
		number_of_extensions++;
	}
}

/**
 * @brief Parses the debug section of the configuration file.
 * @param None
 * @return void
 * @todo Error checking
 */
void ConfigModule::parseDebug() {
	config_data_.debug.console_enabled = 
		json_buffer_["debugging"]["console-enabled"].get<bool>();

	config_data_.debug.console_update_interval = 
		json_buffer_["debugging"]["console-update-interval"].get<int>();

	config_data_.debug.web_server_enabled = 
		json_buffer_["debugging"]["web-server-enabled"].get<bool>();

	config_data_.debug.web_server_update_interval = 
		json_buffer_["debugging"]["web-server-update-interval"].get<int>();
}


/**
 * @brief Parses the telemetry section of the configuration file.
 * @details Currently not fully implemented. It will first check in the config
 * file to see if telemetry is enabled in the config. If it isn't it will 
 * immediately return. If it is enabled, it will read in the rest of the 
 * telemetry configuration. This is currently not fully implemented and will
 * only read the callsign.
 * @param None
 * @return void
 * @todo Error Checking
 * @todo Implement the rest of the telemetry configuration
 */
void ConfigModule::parseTelemetry() {
	int telem_enabled =
	json_buffer_["telemetry"]["telemetry-enabled"].get<bool>();
	if (telem_enabled == 0) {
		config_data_.telemetry.telemetry_enabled = 0;
		return;
	}
	
	config_data_.telemetry.telemetry_enabled = 1;

	std::string callsign = json_buffer_["telemetry"]["callsign"].get<std::string>();
	if (callsign == "" || callsign == "NOCALL") {
		errors_.push_back("Your callsign is invalid.");
	} else {
		config_data_.telemetry.callsign = callsign;
	}
}

/**
 * @brief Parses the data type section of the configuration file.
 * @details This will read in the data types from the configuration file. This
 * is what determines if it will be included in telemetry or not. Name is
 * used by the extensions, unit is used by the telemetry and should be a couple
 * characters in length.
 * @param None
 * @return void
 * @todo Update error checking.
 */
void ConfigModule::parseDataTypes() {
	for (const auto& item : json_buffer_["data-types"].items()) {
		ConfigData::DataTypes::ExtensionDataType newDataType;
		try 
		{
			newDataType.source = item.value()["source"].get<std::string>();
			newDataType.name = item.value()["name"].get<std::string>();
			newDataType.unit = item.value()["unit"].get<std::string>();
			newDataType.include_in_telemtry = 
			item.value()["include-in-telemetry"].get<bool>();

			config_data_.data_types.types.push_back(newDataType);
		} catch (const std::exception& e) {
			errors_.push_back("Error parsing data-types." + (std::string) e.what());
		}
	}
}

/**
 * @brief Parses the flight loop section of the configuration file.
 * @details This is the portion that contains the actual tasks of the flight.
 * @param None
 * @return void
 * @todo change 'flight loop' to 'flight mode'
 */
void ConfigModule::parseFlightLoops() {
	for (const auto& item : json_buffer_["flight-loops"].items()) {
		FlightLoop newFlightLoop;

		newFlightLoop.type = item.value()["type"].get<FlightLoop::LoopType>();

		if (newFlightLoop.type == FlightLoop::LoopType::ERROR) {
			errors_.push_back("Invalid flight loop type.");
		}

		newFlightLoop.enabled = item.value()["enabled"].get<bool>();

		newFlightLoop.intervals.data_log = 
		item.value()["intervals"]["data-log"].get<int>();

		if (newFlightLoop.type == FlightLoop::LoopType::TESTING) {
			config_data_.flight_loops.testing = newFlightLoop;
		} else if (newFlightLoop.type == FlightLoop::LoopType::STANDARD) {
			config_data_.flight_loops.standard = newFlightLoop;
		} else if (newFlightLoop.type == FlightLoop::LoopType::RECOVERY) {
			config_data_.flight_loops.recovery = newFlightLoop;
		}
	}
}