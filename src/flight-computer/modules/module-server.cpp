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
using json = nlohmann::ordered_json; 

#include "socket.h"
#include "modules.h"
using namespace modules;

ServerModule::ServerModule(const ConfigData config_data,
		DataStream *data_stream):
			Module(data_stream, MODULE_SERVER_PREFIX),
			config_data_(config_data),
			p_data_stream_(data_stream) {
}

ServerModule::~ServerModule() {
	stop();
}

void ServerModule::start() {
	stop_flag_ = 0;
	runner_thread_ = std::thread(&ServerModule::runner, this);
}

void ServerModule::stop() {
	if (status() == ModuleStatus::STOPPED && !runner_thread_.joinable()) {
		return;
	}
	stop_flag_ = 1;
	if (runner_thread_.joinable()) {
		runner_thread_.join();
		module_status_ = ModuleStatus::STOPPED;
	}
}

int ServerModule::checkShutdown() {
	return gfs_shutdown_flag_;
}

void ServerModule::runner() {
	ServerSocket server_socket(MODULE_SERVER_PORT, 1);  // Create non-blocking so it can be checked for shutdown
	int empty_request_count = 0;
	module_status_ = ModuleStatus::RUNNING;
	while (!stop_flag_) {
		try {
			ServerSocket new_sock;  // Create a new socket for the connection
			if (server_socket.accept(new_sock)) {
				data("SOCKET", "CONNECTED", 0);
			} else {
				std::this_thread::sleep_for(std::chrono::milliseconds(50));
				data("SOCKET", "DISCONNECTED", 0);
				continue;
			}
			
			while (!stop_flag_) {
				std::this_thread::sleep_for(std::chrono::milliseconds(50));
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
					p_data_stream_->addToCommandQueue(request);
					continue;
				}

				if (request == "get-config") {
					sendConfig(new_sock);
				} else if (request == "dynamic") {
					sendDynamicData(new_sock);
				} else if (request == "telemetry"){
					sendTelemetryData(new_sock);
				} else if (request == "shutdownServer") {
					p_data_stream_->addData(
						MODULE_SERVER_PREFIX, 
						"SOCKET", 
						"SHUTDOWN", 
						0
						);
					module_status_ = ModuleStatus::STOPPED;
					stop_flag_ = 1;
					return;
				} else if (request == "shutdownGFS") {
					p_data_stream_->addData(
						MODULE_SERVER_PREFIX, 
						"SOCKET", 
						"SHUTDOWN_GFS", 
						0
						);
					module_status_ = ModuleStatus::STOPPED;
					stop_flag_ = 1;
					gfs_shutdown_flag_ = 1;
					return;
				} else if (request == "DISCONNECT") {
					p_data_stream_->addData(
						MODULE_SERVER_PREFIX, 
						"SOCKET", 
						"DISCONNECTED", 
						0
						);
					break;
				} else {
					error("Unknown Request", request);
					break;
				}
			}

		} catch (SocketException &e) {
			p_data_stream_->addError(MODULE_SERVER_PREFIX, "Socket Creation",
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
	case ConfigData::MainboardType::ERROR:
		main_board_type = "ERROR";
		break;
	case ConfigData::MainboardType::PI_ZERO_W_2:
		main_board_type = "Pi Zero W 2";
		break;
	case ConfigData::MainboardType::PI_4:
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
	case FlightProcedure::ProcType::ERROR:
		starting_proc = "ERROR";
		break;
	case FlightProcedure::ProcType::TESTING:
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
		//extension_json["category"] = extension.category;
		//extension_json["interface"] = extension.interface;
		extension_json["interval"] = extension.update_interval;
		extension_json["critical"] = extension.critical;
		// Extra Arguments (Optional)
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

void ServerModule::sendDynamicData(ServerSocket &socket) {
	try {
		DataFrame snapshot = p_data_stream_->getDataFrameCopy();
		json data;
		int i = 0;
		for (auto const &[key, packet] : snapshot) {
			data[std::to_string(i++)] = {
				{"source", packet.source},
				{"unit", packet.unit},
				{"value", packet.value}
			};
		}

		json tx_queue_json = {};
		p_data_stream_->lockTXQueue();
		std::queue<Transmission> queue = p_data_stream_->getTXQueue();
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
		p_data_stream_->unlockTXQueue();

		json extension_status = {};
		std::unordered_map<std::string, ExtensionStatus> extension_status_map = p_data_stream_->getExtensionStatuses();

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

		json errors = {};
		ErrorFrame error_snapshot = p_data_stream_->getErrorFrameCopy();
		int k = 0;
		for (auto const &[key, error] : error_snapshot) {
			errors[std::to_string(k++)] = {
				{"source", error.source},
				{"code", error.error_code},
				{"info", error.info}
			};
		}

		json dynamic_data = {
			{"dynamic", data},
			{"tx-queue", tx_queue_json},
			{"extension-status", extension_status},
			{"errors", errors}
		};

		socket << dynamic_data.dump();  // Send the data
	} catch (SocketException &e) {
		p_data_stream_->addError(MODULE_SERVER_PREFIX, "Dynamic",
			e.description(), 0);
	}
}

void ServerModule::sendTelemetryData(ServerSocket &socket) {
	json tx_log_info = {}; // Transmission Log Info
	json tx_log = {}; // Transmission Log
	
	const DataStream::TXLogInfo info = p_data_stream_->getTXLogInfo();
	
	tx_log_info["size"] = info.tx_log_size;
	tx_log_info["max_size"] = info.max_size;
	tx_log_info["first"] = info.first_tx_in_log;
	tx_log_info["last"] = info.last_tx_in_log;

	p_data_stream_->lockTXLog();
	const std::deque<Transmission> &log = p_data_stream_->getTXLog();
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
	p_data_stream_->unlockTXLog();

	json telemetry_data = {
		{"tx-log-info", tx_log_info},
		{"tx-log", tx_log}
	};
	try {
		socket << telemetry_data.dump();  // Send the data
	} catch (SocketException &e) {
		p_data_stream_->addError(MODULE_SERVER_PREFIX, "Telemetry",
			e.description(), 0);
	}
}