/**
 * @file module-server.cpp
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief The debugging server module implementation
 * @details This handles the server socket for the python client to
 * connect to.
 * 
 * It currently sends large chunks of data even when nothing has changed.
 * This should be changed in the future but it has not caused any
 * problems yet and it makes catching weird bugs easier.
 * This is not used during flight, so it isn't the biggest priority,
 * but this can be optimized a lot.
 * 
 * @date 2023-01-05
 * @copyright Copyright (c) 2023
 * @version 0.3
 */

#include <iostream>
#include <nlohmann/json.hpp>
#include <deque>
#include <ctime>
#include <sstream>
using json = nlohmann::ordered_json; 

#include "socket.h"
#include "modules.h"
using namespace modules;

ServerModule::ServerModule(const Data config_data, DataStream &stream):
			Module(stream, MODULE_SERVER_PREFIX, "server"),
			config_data_(config_data) {
}

ServerModule::~ServerModule() {
	stop();
}

void ServerModule::start() {
	updateStatus(ModuleStatus::STARTING);
	stop_flag_ = 0;
	runner_thread_ = std::thread(&ServerModule::runner, this);
}

void ServerModule::stop() {
	updateStatus(ModuleStatus::STOPPING);
	if (status() == ModuleStatus::STOPPED && !runner_thread_.joinable()) {
		return;
	}
	stop_flag_ = 1;
	if (runner_thread_.joinable()) {
		runner_thread_.join();
		updateStatus(ModuleStatus::STOPPED);
	}
}

int ServerModule::checkShutdown() {
	return gfs_shutdown_flag_;
}

void ServerModule::runner() {
	ServerSocket server_socket(MODULE_SERVER_PORT, 1);  // Create non-blocking so it can be checked for shutdown
	int empty_request_count = 0;
	while (!stop_flag_) {
		updateStatus(ModuleStatus::RUNNING);
		try {
			ServerSocket new_sock;  // Create a new socket for the connection
			if (server_socket.accept(new_sock)) {
				data("SOCKET", "CONNECTED");
			} else {
				std::this_thread::sleep_for(std::chrono::milliseconds(50));
				data("SOCKET", "DISCONNECTED");
				continue;
			}
			
			while (!stop_flag_) {
				std::this_thread::sleep_for(std::chrono::milliseconds(5));
				std::string request;
				new_sock >> request;  // Read the request from the client

				if (request.empty()) {
					empty_request_count++;
					if (empty_request_count > 10) { // If there are 10 empty requests in a row, assume the client has disconnected
						break;
					}
					continue;
				}
				std::string command_check = request.substr(0, 4);
				if (command_check == "cmd/") {
					data_stream_.addToCommandQueue(request);
					continue;
				}

				if (request == "get-config") {
					sendConfig(new_sock);
				} else if (request == "get-status") {
					sendStatus(new_sock);
				} else if (request == "get-gfs-data") {
					sendGfsData(new_sock);
				} else if (request == "get-extension-statuses") {
					sendExtensionStatuses(new_sock);
				} else if (request == "get-telemetry-data"){
					sendTelemetryData(new_sock);
				} else if (request == "get-data-frame") {
					sendDataFrame(new_sock);
				} else if (request == "get-error-frame") {
					sendErrorFrame(new_sock);
				}
				else if (request == "shutdownServer") {
					data_stream_.addData(
						MODULE_SERVER_PREFIX, 
						"SOCKET", 
						"SHUTDOWN");
					module_status_ = ModuleStatus::STOPPED;
					stop_flag_ = 1;
					return;
				} else if (request == "shutdownGFS") {
					data_stream_.addData(
						MODULE_SERVER_PREFIX, 
						"SOCKET", 
						"SHUTDOWN_GFS"
						);
					module_status_ = ModuleStatus::STOPPED;
					stop_flag_ = 1;
					gfs_shutdown_flag_ = 1;
					return;
				} else if (request == "DISCONNECT") {
					data_stream_.addData(
						MODULE_SERVER_PREFIX, 
						"SOCKET", 
						"DISCONNECTED"
						);
					break;
				} else {
					error("Unknown Request", request);
					break;
				}
			}

		} catch (SocketException &e) {
			data_stream_.addError(MODULE_SERVER_PREFIX, "Socket Creation",
										e.description(), 0);
		}
		
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}
	return;
}

void ServerModule::sendConfig(ServerSocket &socket) {
	json general_config;
	general_config["project-name"] = config_data_.general.project_name;

	std::string main_board_type;
	switch (config_data_.general.main_board)
	{
	case Data::Mainboard::ERROR:
		main_board_type = "ERROR";
		break;
	case Data::Mainboard::PI_ZERO_W_2:
		main_board_type = "Pi Zero W 2";
		break;
	case Data::Mainboard::PI_4:
		main_board_type = "Pi 4";
		break;
	default:
		main_board_type = "Unknown";
		break;
	}
	general_config["main-board-type"] = main_board_type;

	std::string starting_proc;
	switch (config_data_.general.starting_proc) 
	{
	case FlightProcedure::Type::ERROR:
		starting_proc = "ERROR";
		break;
	case FlightProcedure::Type::TESTING:
		starting_proc = "TESTING";
		break;
	default:
		starting_proc = "Unknown";
		break;
	}
	general_config["starting-procedure"] = starting_proc;

	general_config["gfs-version"] = (std::string)GFS_VERSION;


	json debug_config;
	debug_config["console-enabled"] = config_data_.debug.console_enabled;
	debug_config["console-update-interval"] = config_data_.debug.console_update_interval;
	debug_config["web-server-enabled"] = config_data_.debug.web_server_enabled;
	debug_config["web-server-socket-port"] = config_data_.debug.web_server_socket_port;

	json telemetry_config;
	telemetry_config["telemetry-enabled"] = config_data_.telemetry.telemetry_enabled;
	telemetry_config["call-sign"] = config_data_.telemetry.call_sign;
	telemetry_config["data-packet-mode"] = config_data_.telemetry.data_packet_mode;
	telemetry_config["aprs-enabled"] = config_data_.telemetry.aprs_enabled;
	telemetry_config["aprs-frequency"] = config_data_.telemetry.aprs_freq;
	telemetry_config["aprs-ssid"] = config_data_.telemetry.aprs_ssid;
	telemetry_config["aprs-key"] = config_data_.telemetry.aprs_key;
	telemetry_config["aprs-memo"] = config_data_.telemetry.aprs_memo;
	telemetry_config["sstv-enabled"] = config_data_.telemetry.sstv_enabled;
	telemetry_config["sstv-frequency"] = config_data_.telemetry.sstv_freq;
	telemetry_config["sstv-mode"] = config_data_.telemetry.sstv_mode;
	telemetry_config["afsk-enabled"] = config_data_.telemetry.afsk_enabled;
	telemetry_config["afsk-frequency"] = config_data_.telemetry.afsk_freq;
	telemetry_config["psk-enabled"] = config_data_.telemetry.psk_enabled;
	telemetry_config["psk-frequency"] = config_data_.telemetry.psk_freq;
	telemetry_config["psk-mode"] = config_data_.telemetry.psk_mode;

	json extensions_config = json::array();
	for (ExtensionMetadata extension : config_data_.extensions.extensions_list) {
		json extension_json;
		extension_json["id"] = extension.id;
		extension_json["name"] = extension.name;
		extension_json["type"] = extension.extension_type;
		extension_json["category"] = 
			kExtensionCategoryToString.at(extension.category);
		extension_json["interface"] = 
			kExtensionInterfaceToString.at(extension.interface);
		extension_json["interval"] = extension.update_interval;
		extension_json["critical"] = extension.critical;
		// Extra Arguments
		if (extension.interface == ExtensionMetadata::Interface::I2C) {
			extension_json["extra-args"] =  "BS: " +
				std::to_string(extension.extra_args.I2C_bus) + " ADDR:" 
				+ extension.extra_args.I2C_device_address;
		} else if (extension.interface == ExtensionMetadata::Interface::ONEWIRE) {
			extension_json["extra-args"] = "ID: " +
				extension.extra_args.one_wire_id;
		} else {
			extension_json["extra-args"] = "N/A";
		}
		extensions_config.push_back(extension_json);
	}

	json config = {
		{"general", general_config},
		{"debugging", debug_config},
		{"telemetry", telemetry_config},
		{"extensions", extensions_config},
		{"data-log-data-and-packet-contents", "N/A"},
		{"flight-procs", "N/A"}
	};
	
	socket << config.dump();  // Send the static data
}

void ServerModule::sendStatus(ServerSocket &socket) {
	time_t now = time(0); // UTC time
	std::tm *gmt = gmtime(&now);
	std::string system_time = 
		(gmt->tm_hour < 10 ? "0" + std::to_string(gmt->tm_hour) : std::to_string(gmt->tm_hour)) + ":" + 
		std::to_string(gmt->tm_min) + ":" + 
		std::to_string(gmt->tm_sec);
	std::string system_date = std::to_string(gmt->tm_mday) + "-" + std::to_string(gmt->tm_mon + 1) + "-" + std::to_string(gmt->tm_year + 1900);

	std::string system_data_source = config_data_.extensions.system_data_name;
	std::string system_uptime_hours = data_stream_.getData(system_data_source, "uptime_hours");
	
	std::string ld_1, ld_5, ld_15, load_avg;
	ld_1 = data_stream_.getData(system_data_source, "load_avg_1");
	ld_5 = data_stream_.getData(system_data_source, "load_avg_5");
	ld_15 = data_stream_.getData(system_data_source, "load_avg_15");
	load_avg = ld_1 + " " + ld_5 + " " + ld_15;

	std::string cpu_temp = data_stream_.getData(system_data_source, "cpu_temp");
	std::string mem_usage = data_stream_.getData(system_data_source, "ram_used_prcnt");
	std::string storage_usage = data_stream_.getData(system_data_source, "disk_used_prcnt");

	std::string battery_voltage =
		data_stream_.getData(config_data_.extensions.battery_data_name, "voltage");

	json system_status = {
		{"system-time-utc", system_time},
		{"system-date", system_date},
		{"system-uptime", system_uptime_hours},
		{"cpu-load-average", load_avg},
		{"cpu-temp", cpu_temp},
		{"memory-usage", mem_usage},
		{"storage-usage", storage_usage},
		{"battery-voltage", battery_voltage}
	};

	time_t gfs_uptime = now - config_data_.start_time;
	std::string gfs_uptime_hours = std::to_string(gfs_uptime / 3600);
	std::string gfs_uptime_minutes = std::to_string((gfs_uptime % 3600) / 60);
	std::string gfs_uptime_seconds = std::to_string(gfs_uptime % 60);
	std::string gfs_uptime_str = 
		(gfs_uptime / 3600 < 10 ? "0" + gfs_uptime_hours : gfs_uptime_hours) + ":" + 
		((gfs_uptime % 3600) / 60 < 10 ? "0" + gfs_uptime_minutes : gfs_uptime_minutes) + ":" + 
		(gfs_uptime % 60 < 10 ? "0" + gfs_uptime_seconds : gfs_uptime_seconds);
	


	std::unordered_map<std::string, ModuleStatus> module_statuses = data_stream_.getModuleStatuses();
	json gfs_status = {
		{"gfs-uptime", gfs_uptime_str},
		{"health-status", "good"},
		{"reported-errors", data_stream_.getNumErrorPackets()},
		{"current-flight-proc", "N-I"},
		{"flight-phase", "N-I"},
		{"modules-status", {
			{"configuration", module_status_to_string_map.at(module_statuses["configuration"])},
			{"data", module_status_to_string_map.at(module_statuses["data"])},
			{"extensions", module_status_to_string_map.at(module_statuses["extensions"])},
			{"telemetry", module_status_to_string_map.at(module_statuses["telemetry"])},
			{"server", module_status_to_string_map.at(module_statuses["server"])},
			{"console", module_status_to_string_map.at(module_statuses["console"])}
		}}
	};

	json statuses = {
		{"system", system_status},
		{"gfs", gfs_status}
	};

	try {
		socket << statuses.dump();
	} catch (SocketException &e) {
		error("WE", "Status");
	}
}

void ServerModule::sendExtensionStatuses(ServerSocket &socket) {
	json extension_status = {};
	std::unordered_map<std::string, ExtensionStatus> extension_status_map 
		= data_stream_.getExtensionStatuses();

	for (auto const &[key, status] : extension_status_map) {
		
		std::string status_str = "";
		switch (status) {
			case ExtensionStatus::ERROR:
				status_str = "ERROR";
				break;
			case ExtensionStatus::STOPPED:
				status_str = "STOPPED";
				break;
			case ExtensionStatus::STARTING:
				status_str = "STARTING";
				break;
			case ExtensionStatus::RUNNING:
				status_str = "RUNNING";
				break;
			case ExtensionStatus::STOPPING:
				status_str = "STOPPING";
				break;
			case ExtensionStatus::STOPPED_ERROR_STATE:
				status_str = "STOPPED_ERROR_STATE";
				break;
		}

		extension_status[key] = status_str;
	}

	try {
		socket << extension_status.dump();
	} catch (SocketException &e) {
		error("WE", "Extension Status");
	}
}

void ServerModule::sendGfsData(ServerSocket &socket) {
	// DataStream Stats
	int total_data_packets = data_stream_.getTotalDataPackets();
	int current_data_packets = data_stream_.getNumDataPackets();

	int total_gps_packets = data_stream_.getTotalGPSPackets();
	int current_gps_packets = data_stream_.getNumGPSPackets();

	int total_error_packets = data_stream_.getTotalErrorPackets();
	int current_error_packets = data_stream_.getNumErrorPackets();

	json data_stream_stats_json = {
		{"total-data-packets", total_data_packets},
		{"current-data-packets", current_data_packets},
		{"total-gps-packets", total_gps_packets},
		{"current-gps-packets", current_gps_packets},
		{"total-error-packets", total_error_packets},
		{"current-error-packets", current_error_packets}
	};

	// Critical Data
	CriticalData c_data = data_stream_.getCriticalData();
	std::stringstream ss;
	ss << std::fixed << std::setprecision(2) << c_data.battery_voltage;
	std::string battery_voltage = ss.str();

	json critical_data_json = {
		{"flight-phase", FLIGHT_PHASE_TO_STRING.at(c_data.phase)},
		{"gps-data-valid", c_data.gps_data_valid},
		{"gps-fix", GPS_FIX_TO_STRING.at(c_data.gps_data.fix)},
		{"gps-alt", c_data.gps_data.altitude},
		{"gps-lat", c_data.gps_data.latitude},
		{"gps-lon", c_data.gps_data.longitude},
		{"pressure-data-valid", c_data.pressure_data_valid},
		{"pressure-mbar", c_data.pressure_mbar},
		{"battery-data-valid", c_data.battery_data_valid},
		{"battery-voltage", battery_voltage},
		{"system-data-valid", c_data.system_data_valid},
		{"ram-usage", c_data.ram_usage},
		{"disk-usage", c_data.disk_usage},
		{"radio-status", c_data.radio_status}
	};

	// Data Log Status
	json data_log_status_json = {
		{"enabled", "N-I"},
		{"file-name", "N-I"},
		{"file-size-mb", "N-I"}
	};

	// Position Data
	GPSFrame &gps_frame = c_data.gps_data;
	json position_data_json = {
		{"source", gps_frame.source},
		{"time", gps_frame.time},
		{"fix", GPS_FIX_TO_STRING.at(c_data.gps_data.fix)},
		{"num-sats", gps_frame.num_satellites},
		{"latitude", gps_frame.latitude},
		{"longitude", gps_frame.longitude},
		{"altitude", gps_frame.altitude},
		{"ground-speed", gps_frame.ground_speed},
		{"vertical-speed", gps_frame.vertical_speed},
		{"horz-accuracy", gps_frame.horz_accuracy},
		{"vert-accuracy", gps_frame.vert_accuracy},
		{"heading-of-motion", gps_frame.heading_of_motion},
		{"heading-accuracy", gps_frame.heading_accuracy}
	};

	// Environmental Data
	json environmental_data_json = {
		{"pressure", {
			{"source1", {
				{"unit", "N-I"},
				{"value", "N-I"}
			}},
			{"source2", {
				{"unit", "N-I"},
				{"value", "N-I"}
			}}
		}},
		{"temperature", {
			{"source1", {
				{"unit", "N-I"},
				{"value", "N-I"}
			}},
			{"source2", {
				{"unit", "N-I"},
				{"value", "N-I"}
			}}
		}},
		{"humidity", {
			{"source1", {
				{"unit", "N-I"},
				{"value", "N-I"}
			}},
			{"source2", {
				{"unit", "N-I"},
				{"value", "N-I"}
			}}
		}}
	};

	// Calculated Data
	json calculated_data_json = {
		{"distance-traveled", "N-I"},
		{"overall-heading", "N-I"},
		{"time-since-launch", "N-I"}
	};

	json gfs_data = {
		{"data-stream", data_stream_stats_json},
		{"critical-data", critical_data_json},
		{"data-log", data_log_status_json},
		{"position-data", position_data_json},
		{"environmental-data", environmental_data_json},
		{"calculated-data", calculated_data_json}
	};

	try {
		socket << gfs_data.dump();  // Send the data
	} catch (SocketException &e) {
		data_stream_.addError(MODULE_SERVER_PREFIX, "Dynamic",
			e.description(), 0);
	}
}

void ServerModule::sendTelemetryData(ServerSocket &socket) {
	json tx_queue_json = {};
	data_stream_.lockTXQueue();
	std::queue<Transmission> queue = data_stream_.getTXQueue();
	int j = 0;
	while (!queue.empty()) {
		Transmission tx = queue.front();
		queue.pop();
		tx_queue_json[std::to_string(j++)] = {
			{"file", tx.wav_location},
			{"type", tx.type},
			{"duration", tx.length}
		};
	}
	data_stream_.unlockTXQueue();
	
	json tx_log_info = {}; // Transmission Log Info
	json tx_log = {}; // Transmission Log
	
	const DataStream::TXLogInfo info = data_stream_.getTXLogInfo();
	
	tx_log_info["size"] = info.tx_log_size;
	tx_log_info["max_size"] = info.max_size;
	tx_log_info["first"] = info.first_tx_in_log;
	tx_log_info["last"] = info.last_tx_in_log;

	data_stream_.lockTXLog();
	const std::deque<Transmission> &log = data_stream_.getTXLog();
	int i = 0;
	for (auto const &tx : log) {
		std::string type = "";
		switch (tx.type) {
			case Transmission::Type::ERROR:
				type = "ERROR";
				break;
			case Transmission::Type::APRS:
				type = "APRS";
				break;
			case Transmission::Type::AFSK:
				type = "AFSK";
				break;
			case Transmission::Type::PSK:
				type = "PSK";
				break;
			case Transmission::Type::SSTV:
				type = "SSTV";
				break;
			default:
				type = "UNKNOWN";
				break;
		}
		tx_log[std::to_string(i++)] = {
			{"ID", tx.tx_num},
			{"type", type},
			{"message", tx.message},
			{"length", tx.length}
		};
	}
	data_stream_.unlockTXLog();

	json telemetry_data = {
		{"tx-log-info", tx_log_info},
		{"tx-log", tx_log}
	};
	try {
		socket << telemetry_data.dump();  // Send the data
	} catch (SocketException &e) {
		data_stream_.addError(MODULE_SERVER_PREFIX, "Telemetry",
			e.description(), 0);
	}
}

void ServerModule::sendDataFrame(ServerSocket &socket) {
	// Data Frame
	DataFrame snapshot = data_stream_.getDataFrameCopy();
	json data_frame;
	int i = 0;
	for (auto const &[key, packet] : snapshot) {
		data_frame[std::to_string(i++)] = {
			{"source", packet.source},
			{"unit", packet.unit},
			{"value", packet.value}
		};
	}

	try {
		socket << data_frame.dump();  // Send the data
	} catch (SocketException &e) {
		data_stream_.addError(MODULE_SERVER_PREFIX, "Data",
			e.description(), 0);
	}
}

void ServerModule::sendErrorFrame(ServerSocket &socket) {
	json errors = {};
	ErrorFrame error_snapshot = data_stream_.getErrorFrameCopy();
	int k = 0;
	for (auto const &[key, error] : error_snapshot) {
		errors[std::to_string(k++)] = {
			{"source", error.source},
			{"code", error.error_code},
			{"info", error.info}
		};
	}

	try {
		socket << errors.dump();  // Send the data
	} catch (SocketException &e) {
		data_stream_.addError(MODULE_SERVER_PREFIX, "Error",
			e.description(), 0);
	}
}