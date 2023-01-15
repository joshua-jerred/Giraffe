/**
 * @file module-configuration.cpp
 * @author Joshua Jerred (https://joshuajer.red/)
 * @brief This file implements the class configuration module.
 * 
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

#include "data-stream.h"

#include "modules.h"
using namespace modules;

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
 * defined in config-types.h
 * 
 * Important note from the nlohmann json documentation:
 * "When using get<ENUM_TYPE>(), undefined JSON values will default to the first 
 * pair specified in your map. Select this default pair carefully."
 * 
 * This is why the first pair is 'error' so the configuration module can
 * detect an incorrect value in the config file.
 * 
 * @see config-types.h
 */
NLOHMANN_JSON_SERIALIZE_ENUM( Data::Mainboard, {
    {Data::Mainboard::ERROR, "error"}, 
    {Data::Mainboard::OTHER, "other"},
    {Data::Mainboard::PI_ZERO_W_2, "pi_zero_w_2"},
    {Data::Mainboard::PI_4, "pi_4"}
})

NLOHMANN_JSON_SERIALIZE_ENUM( FlightProcedure::Type, {
    {FlightProcedure::Type::ERROR, "error"},
    {FlightProcedure::Type::TESTING, "testing"},
    {FlightProcedure::Type::STANDARD, "standard"},
    {FlightProcedure::Type::RECOVERY, "recovery"},
    {FlightProcedure::Type::FAILSAFE, "failsafe"}
})

NLOHMANN_JSON_SERIALIZE_ENUM( ExtensionMetadata::Category, {
    {ExtensionMetadata::Category::ERROR, "error"},
    {ExtensionMetadata::Category::OTHER, "other"},
    {ExtensionMetadata::Category::RADIO, "radio"},
    {ExtensionMetadata::Category::GPS, "gps"},
    {ExtensionMetadata::Category::CAMERA, "camera"},
	{ExtensionMetadata::Category::BATTERY, "battery"},
	{ExtensionMetadata::Category::SYSTEM, "system"},
    {ExtensionMetadata::Category::INTERNAL_SENSOR, "internal_sensor"},
    {ExtensionMetadata::Category::EXTERNAL_SENSOR, "external_sensor"}
})

NLOHMANN_JSON_SERIALIZE_ENUM( ExtensionMetadata::Interface, {
    {ExtensionMetadata::Interface::OTHER, "other"},
    {ExtensionMetadata::Interface::INTERNAL, "internal"},
    {ExtensionMetadata::Interface::I2C, "i2c"},
    {ExtensionMetadata::Interface::UART, "uart"},
    {ExtensionMetadata::Interface::ONEWIRE, "onewire"},
    {ExtensionMetadata::Interface::USB, "usb"},
    {ExtensionMetadata::Interface::GPIO, "gpio"}
})
/**
 * @brief Construct a new ConfigModule::ConfigModule object
 * @param None
 */
ConfigModule::ConfigModule(DataStream &stream):
	data_stream_(stream) {
	number_of_errors_ = 0;
	config_data_.start_time = time(0);
}

/**
 * @brief Deconstructs the ConfigModule object
 */
ConfigModule::~ConfigModule() {
}

/**
 * @brief Load and parse a configuration file from a given path.
 * @details This function will load and parse a json file from a given path.
 * If the file fails to open or if it does not exists it will return -1.
 * If it does load, it will call parseAll()
 * @param file_path The local path to the configuration file.
 * @return int 0 if successfully loaded and parsed, -1 if the file failed to
 * open, -2 if there were any errors parsing the file.
 * @see parseAll()
 */
int ConfigModule::load(std::string file_path) {
	data_stream_.updateModuleStatus("config", ModuleStatus::RUNNING);
	config_file_path_ = file_path;
	
	std::ifstream fs(config_file_path_); // open file
	if (fs.fail()) {
		data_stream_.updateModuleStatus("config", ModuleStatus::ERROR_STATE);
		return -1; // file failed to open
	}
	
	json_buffer_ = json::parse(fs);
	parseAll();

	//if (getErrors().size() > 0) {
	//	return -2; // one or more errors parsing the file
	//}
	data_stream_.updateModuleStatus("config", ModuleStatus::STOPPED);
	return 0;
}

/**
 * @brief Returns all of the configuration data as a ConfigData object. All
 * errors will be included, refer to getErrors() for a list of errors.
 * @param None
 * @return ConfigData - All of the configuration data.
 */
Data ConfigModule::getAll() {
	return config_data_;
}

/**
 * @brief Returns a copy of the json data.
 * @param None
 * @return json - nlohmann json object
json ConfigModule::getAllJson() {
	return json_buffer_;
}
*/

int ConfigModule::getNumberOfErrors() {
	return number_of_errors_;
}

/**
 * @brief Simple override for adding errors to the data stream.
 * @param error_code The error code
 * @param info Additional information about the error (optional)
 */
template <typename T>
void ConfigModule::error(std::string error_code, T info) {
	number_of_errors_++;
	data_stream_.addError(MODULE_CONFIG_PREFIX, error_code, 
		std::to_string(info), 0);
}

void ConfigModule::error(std::string error_code, std::string info) {
	number_of_errors_++;
	data_stream_.addError(MODULE_CONFIG_PREFIX, error_code, info, 0);
}

void ConfigModule::error(std::string error_code) {
	number_of_errors_++;
	data_stream_.addError(MODULE_CONFIG_PREFIX, error_code, "", 0);
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
	parseFlightProcedures();
}

/**
 * @brief Parses the general section of the configuration file.
 * @details This function pulls out the project name, mainboard type, and
 * starting proc type. It will add errors to the errors_ vector if any of the
 * required fields contain errors.
 * @param None
 * @return void
 * */
void ConfigModule::parseGeneral() { 
	if (!json_buffer_.contains("general")) {
		error("GEN_NF"); // General section does not exist in config
		return;
	}

	if (!json_buffer_["general"].contains("project-name")) {
		error("GEN_PN_NF"); // Project name does not exist in config
		config_data_.general.project_name = "INVALID";
	} else {

		std::string name = 
		json_buffer_["general"]["project-name"].get<std::string>();

		if (name.length() < PROJECT_NAME_MIN_LENGTH || 
		name.length() > PROJECT_NAME_MAX_LENGTH) {
			error("GEN_PN_R", name);
			config_data_.general.project_name = "INVALID";
		} else if (!std::regex_search(name, std::regex("^[a-zA-Z_ 0-9-]*$"))) { 
			error("GEN_PN_I", name);
		} else {
			config_data_.general.project_name = name;
		}
	}

	if (!json_buffer_["general"].contains("main-board-type")) {
		error("GEN_MB_NF"); // Mainboard type does not exist in config
		config_data_.general.main_board = Data::Mainboard::ERROR;
	} else {
		Data::Mainboard mbtype = 
		json_buffer_["general"]["main-board-type"].get<Data::Mainboard>();
		if (mbtype == Data::Mainboard::ERROR) {
			error("GEN_MB_I");
		} else {
			config_data_.general.main_board = mbtype;
		}
	}

	if (!json_buffer_["general"].contains("starting-procedure")) {
		error("GEN_SP_NF"); // Starting procedure does not exist in config
		config_data_.general.starting_proc = FlightProcedure::Type::ERROR;
	} else {
		FlightProcedure::Type ptype = 
		json_buffer_["general"]["starting-procedure"].get<FlightProcedure::Type>();
		if (ptype == FlightProcedure::Type::ERROR) {
			error("GEN_SP_I");
		} else {
			config_data_.general.starting_proc = ptype;
		}
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
		try {
			int id = item.value()["id"].get<int>();
			if (id != number_of_extensions + 1) { 
				error("EXT_ID_R", id);
			} else if (id < EXTENSION_ID_MIN || id > EXTENSION_ID_MAX) {
				error("EXT_ID_S", id);
			} else {
				newExtension.id = id;
			}
	
			std::string name = item.value()["name"].get<std::string>();
			if (name.length() < EXTENSION_NAME_MIN_LENGTH ||
			name.length() >= EXTENSION_NAME_MAX_LENGTH) {
				error("EXT_NM_R", name);
			} else if (!std::regex_search(name, std::regex("^[a-zA-Z_0-9-]*$"))) {
				error("EXT_NM_I", name);
			}
			else {
				newExtension.name = name;
			}
	
			std::string etype = item.value()["type"].get<std::string>();
			if (etype.length() < EXTENSION_NAME_MIN_LENGTH ||
			etype.length() > EXTENSION_NAME_MAX_LENGTH) {
			} else {
				newExtension.extension_type = etype;
			}
	
			ExtensionMetadata::Category category =
			item.value()["category"].get<ExtensionMetadata::Category>();
			if (category == ExtensionMetadata::Category::ERROR) {
			} else {
				newExtension.category = category;

				if (category == ExtensionMetadata::Category::GPS) {
					if (config_data_.extensions.gps_data_name.size() > 0) {
						error("MGPS");
					} else {
						config_data_.extensions.gps_data_name = name;
					}
				} else if (category == ExtensionMetadata::Category::BATTERY) {
					if (config_data_.extensions.battery_data_name.size() > 0) {
						error("MBAT");
					} else {
						config_data_.extensions.battery_data_name = name;
					}
				} else if (category == ExtensionMetadata::Category::SYSTEM) {
					if (config_data_.extensions.system_data_name.size() > 0) {
						error("MSYS");
					} else {
						config_data_.extensions.system_data_name = name;
					}
				} else if (category == ExtensionMetadata::Category::RADIO) {
					if (config_data_.extensions.radio_data_name.size() > 0) {
						error("MRAD");
					} else {
						config_data_.extensions.radio_data_name = name;
					}
				}
			}
	
			ExtensionMetadata::Interface interface =
			item.value()["interface"].get<ExtensionMetadata::Interface>();
			if (interface == ExtensionMetadata::Interface::ERROR) {
				// Report error here
			} else {
				if (interface == ExtensionMetadata::Interface::ONEWIRE) { // On onewire
					
					if (!item.value().contains("onewire-id")) {
						newExtension.extra_args.one_wire_id = "NOT FOUND";
						error("EXT_OW_NF", id);
					} else {
						std::string id = 
						item.value()["onewire-id"].get<std::string>();

						if (!std::regex_search(id, std::regex("28-[0-9&a-f]{12}"))) {
						
							error("EXT_OW_I", id);

						} else {
							newExtension.extra_args.one_wire_id = id;
						}
					}
	
				} else if (interface == ExtensionMetadata::Interface::I2C) { // On I2C
					std::string address = 
					item.value()["i2c-address"].get<std::string>();
	
					int address_num = -1;
					std::stringstream strs;
					strs << std::hex << address; // convert hex string to int
					strs >> address_num;
	
					if (address_num < 0 || address_num > 127) {
						error("EXT_I2_A", address);
					} else {
						newExtension.extra_args.I2C_device_address = address;
					}
	
					int bus = item.value()["i2c-bus"].get<int>();
					if (bus < 0 || bus > 2) {
						error("ETC_I2_B", bus);
					} else {
						newExtension.extra_args.I2C_bus = bus;
					}
	
				}
				newExtension.interface = interface;
			}
	
			int interval = item.value()["update-interval"].get<int>();
			if (interval < EXTENSION_INTERVAL_MIN || 
			interval > EXTENSION_INTERVAL_MAX) {
				error("EXT_UI_R", interval);
				newExtension.update_interval = 1000;
			} else {
				newExtension.update_interval = interval;
			}
	
			int flight_critical = item.value()["flight-critical"].get<int>();
			if (flight_critical != 0 && flight_critical != 1) {
				//errors_.push_back("Extension flight-critical must be 0 or 1.");
			} else {
				newExtension.critical = flight_critical;
			}
	
			if (newExtension.category == ExtensionMetadata::Category::RADIO) {
				// Check for radio-specific fields
			}
	
			config_data_.extensions.extensions_list.push_back(newExtension);
			number_of_extensions++;
		}
		catch(const std::exception& e) {
			error("EXT_P", number_of_extensions++); 
		}
	}	
	
	if (config_data_.extensions.battery_data_name.size() == 0) {
		error("NO_BAT");
	}
	if (config_data_.extensions.system_data_name.size() == 0) {
		error("NO_SYS");
	}
	if (config_data_.extensions.radio_data_name.size() == 0) {
		error("NO_RAD");
	}
	if (config_data_.extensions.gps_data_name.size() == 0) {
		error("NO_GPS");
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

	config_data_.debug.web_server_socket_port = 
		json_buffer_["debugging"]["web-server-socket-port"].get<int>();
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

	std::string callsign = json_buffer_["telemetry"]["call-sign"].get<std::string>();
	if (callsign == "" || callsign == "NOCALL") {
		//errors_.push_back("Your callsign is invalid.");
	} else {
		config_data_.telemetry.call_sign = callsign;
	}

	int aprs_enabled = json_buffer_["telemetry"]["aprs-enabled"].get<bool>();
	config_data_.telemetry.aprs_enabled = 0; // First disable

	if (aprs_enabled) {
		std::string aprs_frequency = 
			json_buffer_["telemetry"]["aprs-frequency"].get<std::string>();
		/** @todo Check if frequency is valid */

		int ssid = json_buffer_["telemetry"]["aprs-ssid"].get<int>();
		/** @todo Check if ssid is valid */

		std::string symbol = 
		json_buffer_["telemetry"]["aprs-symbol"].get<std::string>();
		/** @todo Check if symbol is valid */

		std::string memo = 
		json_buffer_["telemetry"]["aprs-memo"].get<std::string>();
		/** @todo Check if memo is valid */

		// if all are valid, set in config_data_ and enabled
		config_data_.telemetry.aprs_enabled = 1;
		config_data_.telemetry.aprs_freq = aprs_frequency;
		config_data_.telemetry.aprs_ssid = ssid;
		config_data_.telemetry.aprs_symbol = symbol;
		config_data_.telemetry.aprs_memo = memo;
	}

	int sstv_enabled = json_buffer_["telemetry"]["sstv-enabled"].get<bool>();
	config_data_.telemetry.sstv_enabled = 0; // First disable
	
	if (sstv_enabled) {
		std::string sstv_frequency =
			json_buffer_["telemetry"]["sstv-frequency"].get<std::string>();
		/** @todo Check if frequency is valid */

		config_data_.telemetry.sstv_enabled = 1;
		config_data_.telemetry.sstv_freq = sstv_frequency;
	}

	int afsk_enabled = json_buffer_["telemetry"]["afsk-enabled"].get<bool>();
	config_data_.telemetry.afsk_enabled = 0; // First disable

	if (afsk_enabled) {
		std::string afsk_frequency =
			json_buffer_["telemetry"]["afsk-frequency"].get<std::string>();
		/** @todo Check if frequency is valid */

		config_data_.telemetry.afsk_enabled = 1;
		config_data_.telemetry.afsk_freq = afsk_frequency;
	}

	int psk_enabled = json_buffer_["telemetry"]["psk-enabled"].get<bool>();
	config_data_.telemetry.psk_enabled = 0; // First disable

	if (psk_enabled) {
		std::string psk_frequency =
			json_buffer_["telemetry"]["psk-frequency"].get<std::string>();
		/** @todo Check if frequency is valid */

		std::string psk_mode = "bpsk125"; // Default

		try {
			psk_mode = 
				json_buffer_["telemetry"]["psk-mode"].get<std::string>();
			if (psk_mode != "bpsk125" 
				&& psk_mode != "bpsk250" 
				&& psk_mode != "bpsk500"
				&& psk_mode != "bpsk1000") {
				error("TL_PSK_M");
			}
		} catch (std::exception &e) {
			error("TL_PSK_F");
		}
		config_data_.telemetry.psk_enabled = 1;
		config_data_.telemetry.psk_freq = psk_frequency;
		config_data_.telemetry.psk_mode = psk_mode;
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
	for (const auto& item : json_buffer_["data-log-data-and-packet-contents"].items()) {
		Data::DataTypes::DataType newDataType;
		try 
		{
			newDataType.source = item.value()["source"].get<std::string>();
			newDataType.unit = item.value()["unit"].get<std::string>();
			newDataType.include_in_telemetry = 
			item.value()["include-in-telemetry"].get<bool>();
			newDataType.telemetry_name = item.value()["telemetry-name"].get<std::string>();

			config_data_.data_types.types.push_back(newDataType);
		} catch (const std::exception& e) {
			//errors_.push_back("Error parsing data-types." + (std::string) e.what());
		}
	}
}

/**
 * @brief Parses the flight proc section of the configuration file.
 * @details This is the portion that contains the actual tasks of the flight.
 * @param None
 * @return void
 */
void ConfigModule::parseFlightProcedures() {
	for (const auto& item : json_buffer_["flight-procs"].items()) {
		FlightProcedure newFlightProcedure;

		newFlightProcedure.type = item.value()["type"].get<FlightProcedure::Type>();

		if (newFlightProcedure.type == FlightProcedure::Type::ERROR) {
			//errors_.push_back("Invalid flight proc type.");
		}

		newFlightProcedure.enabled = item.value()["enabled"].get<bool>();

		newFlightProcedure.intervals.data_log = 
		item.value()["intervals"]["data-log"].get<int>();
		newFlightProcedure.intervals.data_packet =
		item.value()["intervals"]["data-packet"].get<int>();
		newFlightProcedure.intervals.sstv =
		item.value()["intervals"]["sstv"].get<int>();
		newFlightProcedure.intervals.aprs =
		item.value()["intervals"]["aprs"].get<int>();
		newFlightProcedure.intervals.picture =
		item.value()["intervals"]["picture"].get<int>();
		newFlightProcedure.intervals.health_check =
		item.value()["intervals"]["health-check"].get<int>();


		if (newFlightProcedure.type == FlightProcedure::Type::TESTING) {
			config_data_.flight_procs.testing = newFlightProcedure;
		} else if (newFlightProcedure.type == FlightProcedure::Type::STANDARD) {
			config_data_.flight_procs.standard = newFlightProcedure;
		} else if (newFlightProcedure.type == FlightProcedure::Type::RECOVERY) {
			config_data_.flight_procs.recovery = newFlightProcedure;
		}
	}
}