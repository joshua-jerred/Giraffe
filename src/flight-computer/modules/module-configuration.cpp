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

ConfigModule::ConfigModule() {
	unsaved_changes_ = 0;
}

ConfigModule::~ConfigModule() {
}

int ConfigModule::load(std::string filePath) {
	config_file_path_ = filePath;
	std::ifstream fs(config_file_path_);
	if (fs.fail()) {
		return -1;
	}
	json_buffer_ = json::parse(fs);
	parseAll();
	return 0;
}

ConfigData ConfigModule::getAll() {
	return config_data_;
}

std::vector<std::string> ConfigModule::getErrors() {
	return errors_;
}

json ConfigModule::getAllJson() {
	return json_buffer_;
}

int ConfigModule::parseAll() {
	parseGeneral();
	parseExtensions();
	parseServer();
	parseTelemetry();
	parseDataTypes();
	parseFlightLoops();

	if (errors_.size() > 0) {
		return -1;
	}
	return 0;
}

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
	if (mbtype == ConfigData::MainboardType::kError) {
		errors_.push_back("Invalid main board type.");
	} else {
		config_data_.general.main_board = mbtype;
	}
	
	FlightLoop::LoopType ltype = 
	json_buffer_["general"]["starting-loop"].get<FlightLoop::LoopType>();
	if (ltype == FlightLoop::LoopType::kError) {
		errors_.push_back("Invalid starting loop type.");
	} else {
		config_data_.general.starting_loop = ltype;
	}
}

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
		name.length() > EXTENSION_NAME_MAX_LENGTH) {
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
		if (category == ExtensionMetadata::Category::kError) {
			errors_.push_back("Invalid extension category.");
		} else {
			newExtension.category = category;
		}

		ExtensionMetadata::Interface interface =
		item.value()["interface"].get<ExtensionMetadata::Interface>();
		if (interface == ExtensionMetadata::Interface::kError) {
			errors_.push_back("Invalid extension interface.");
		} else {
			if (interface == ExtensionMetadata::Interface::kOneWire) {
				std::string address = item.value()["address"].get<std::string>();
				if (!std::regex_search(address, std::regex("28-[0-9&a-f]{12}"))) {
					errors_.push_back("OneWire address must match format."
									  " It currently is: " + address);
				} else {
					newExtension.address = address;
				}
			} else if (interface == ExtensionMetadata::Interface::kI2C) {
				std::string address = item.value()["address"].get<std::string>();
				int address_num = -1;
				std::stringstream strs;
				strs << std::hex << item.value()["i2c-address"].get<std::string>(); // convert hex string to int
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

		if (newExtension.category == ExtensionMetadata::Category::kRadio) {
			// Check for radio-specific fields
		}

		config_data_.extensions.extensions_list.push_back(newExtension);
		number_of_extensions++;
	}
}


void ConfigModule::parseServer() {
	config_data_.server.web_server_enabled = json_buffer_["server"]["server-enabled"].get<bool>();
}


/**
 * @todo Needs to be completed.
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

void ConfigModule::parseDataTypes() {
	for (const auto& item : json_buffer_["data-types"].items()) {
		ConfigData::DataTypes::ExtensionDataType newDataType;
		try 
		{
			newDataType.name = item.value()["name"];
			newDataType.unit = item.value()["unit"];
			config_data_.data_types.types.push_back(newDataType);
		} catch (const std::exception& e) {
			errors_.push_back("Error parsing data-types." + (std::string) e.what());
		}
	}
}

void ConfigModule::parseFlightLoops() {
	for (const auto& item : json_buffer_["flight-loops"].items()) {
		FlightLoop newFlightLoop;

		newFlightLoop.type = item.value()["type"].get<FlightLoop::LoopType>();

		newFlightLoop.intervals.data_log = 
		item.value()["intervals"]["data-log"].get<int>();

		newFlightLoop.intervals.server_update = 
		item.value()["intervals"]["server-update"].get<int>();

		if (newFlightLoop.type == FlightLoop::LoopType::kTesting) {
		config_data_.flight_loops.testing = newFlightLoop;
		}
	}
}