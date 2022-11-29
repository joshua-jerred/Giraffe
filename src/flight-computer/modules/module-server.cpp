#include "module-server.h"

#include <iostream>

ServerModule::ServerModule(const ConfigData config_data,
													 DataStream *data_stream) {
	config_data_ = config_data;
	update_interval_ = config_data.debug.web_server_update_interval;
	p_data_stream_ = data_stream;
	gfs_shutdown_flag_ = 0;
}

ServerModule::~ServerModule() { 
	stop(); 
}

void ServerModule::start() {
	stop_flag_ = 0;
	runner_thread_ = std::thread(&ServerModule::runner, this);
}

void ServerModule::stop() {
	if (status() == ModuleStatus::STOPPED) {
		return;
	}
	stop_flag_ = 1;
	if (runner_thread_.joinable()) {
		runner_thread_.join();
	}
}

int ServerModule::checkShutdown() {
	return gfs_shutdown_flag_;
}

void ServerModule::runner() {
	ServerSocket server_socket(MODULE_SERVER_PORT, 0);  // Create blocking
	int empty_request_count = 0;
	while (!stop_flag_) {
		try {
			p_data_stream_->addData(
				MODULE_SERVER_PREFIX, "SOCKET", "WAITING", 0);

			ServerSocket new_sock;  // Create a new socket for the connection
			server_socket.accept(new_sock);

			p_data_stream_->addData(
				MODULE_SERVER_PREFIX, "SOCKET", "CONNECTED", 0);
			
			while (!stop_flag_) {
				std::this_thread::sleep_for(std::chrono::milliseconds(50));
				std::string request;
				new_sock >> request;  // Read the request from the client

				if (request == "static") {
					sendStaticData(new_sock);
				} else if (request == "dynamic") {
					sendDynamicData(new_sock);
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
					p_data_stream_->addData(
						MODULE_SERVER_PREFIX, 
						"SOCKET", 
						"UNKNOWN_REQUEST", 
						0
						);
					break;
				}
			}

		} catch (SocketException &e) {
			p_data_stream_->addError(MODULE_SERVER_PREFIX, "Socket Creation",
										e.description(), update_interval_);
		}
		
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}
	return;
}

void ServerModule::sendStaticData(ServerSocket &socket) {
	json static_data;
	static_data["project-name"] = config_data_.general.project_name;
	static_data["main-board"] = config_data_.general.main_board;
	static_data["starting-proc"] = config_data_.general.starting_proc;
	static_data["console-enabled"] = config_data_.debug.console_enabled;
	static_data["telemetry-enabled"] = config_data_.telemetry.telemetry_enabled;

	static_data["extensions"] = json::array();
	for (ExtensionMetadata extension : config_data_.extensions.extensions_list) {
		json extension_json;
		extension_json["name"] = extension.name;
		extension_json["id"] = extension.id;
		extension_json["type"] = extension.extension_type;
		extension_json["category"] = extension.category;
		extension_json["interface"] = extension.interface;
		extension_json["interval"] = extension.update_interval;
		extension_json["critical"] = extension.critical;
		// extension_json["extra-args"] = extension.extra_args;
		static_data["extensions"].push_back(extension_json);
	}
	socket << static_data.dump();  // Send the static data
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

		json dynamic_data = {
			{"dynamic", data},
			{"tx-queue", tx_queue_json}
		};

		socket << dynamic_data.dump();  // Send the data
	} catch (SocketException &e) {
		p_data_stream_->addError(MODULE_SERVER_PREFIX, "Dynamic",
			e.description(), update_interval_);
	}
}