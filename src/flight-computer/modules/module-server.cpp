#include "module-server.h"

ServerModule::ServerModule(const ConfigData config_data,
													 DataStream *data_stream) {
	config_data_ = config_data;
	update_interval_ = config_data.debug.web_server_update_interval;
	data_stream_ = data_stream;
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
	ServerSocket server_socket(MODULE_SERVER_PORT, 1);  // Create non-blocking socket
	while (!stop_flag_) {
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
		try {
			
			ServerSocket new_sock;  // Create a new socket for the connection
			server_socket.accept(new_sock);

			data_stream_->addData(
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
					data_stream_->addData(
						MODULE_SERVER_PREFIX, 
						"SOCKET", 
						"SHUTDOWN", 
						0
						);
					module_status_ = ModuleStatus::STOPPED;
					stop_flag_ = 1;
					return;
				} else if (request == "shutdownGFS") {
					data_stream_->addData(
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
					data_stream_->addData(
						MODULE_SERVER_PREFIX, 
						"SOCKET", 
						"DISCONNECTED", 
						0
						);
					break;
				} else {
					data_stream_->addData(
						MODULE_SERVER_PREFIX, 
						"SOCKET", 
						"UNKNOWN_REQUEST", 
						0
						);
				}
			}

		} catch (SocketException &e) {
			data_stream_->addError(MODULE_SERVER_PREFIX, "Socket Creation",
										e.description(), update_interval_);
		}
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
		DataFrame snapshot = data_stream_->getDataFrameCopy();
		json data;
		int i = 0;
		for (auto const &[key, packet] : snapshot) {
			data[std::to_string(i++)] = {
				{"source", packet.source},
				{"unit", packet.unit},
				{"value", packet.value}
				};
		}
		socket << data.dump();  // Send the data
	} catch (SocketException &e) {
		data_stream_->addError(MODULE_SERVER_PREFIX, "Dynamic",
			e.description(), update_interval_);
	}
}