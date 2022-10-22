#include "module-server.h"

ServerModule::ServerModule(const ConfigData config_data, DataStream *data_stream) {
    config_data_ = config_data;
    update_interval_ = config_data.debug.web_server_update_interval;
    data_stream_ = data_stream;
}

ServerModule::~ServerModule() {
    stop();
}

void ServerModule::start() {
    stop_flag_ = 0;
    runner_thread_ = std::thread(&ServerModule::runner, this);
}

void ServerModule::stop() {
    stop_flag_ = 1;
    runner_thread_.join();
}

void ServerModule::runner() {
    try {
        ServerSocket server_socket(MODULE_SERVER_PORT); // Create the socket
        
        while (!stop_flag_) {
            std::this_thread::sleep_for(
                std::chrono::milliseconds(update_interval_));
            
            ServerSocket new_sock; // Create a new socket for the connection
            server_socket.accept( new_sock );

            // ----------------------------------
            // ----------------------------------
            // The data transferring part is here

            /**
             * @details All of the data that is sent from the C++ socket server
             * to the web client is in JSON format as it plays nice
             * with python.
             * 
             * All values are strings.
             * 
             * Data format is as follows (Comments are between `these`):
             * |||START||| `Signals the start of the data`
             * |||STATIC||| `Signals the start of the static data`
             * {
             *    "project-name": "",
             *    "main-board": "",
             *    "starting-loop": "",
             *    "console-enabled": "",
             *    "telemetry-enabled": "",
             *    "extensions": [ `List of extensions`
             *       "name": "",
             *       "id": "",
             *       "type": "",
             *       "category": "",
             *       "interface": "",
             *       "interval": "",
             *       "critical": "",
             *       "extra-args": []
             *    ]
             * }
             *    `more will be added as needed`
             * }
             * |||END_STATIC|||
             * |||DYNAMIC||| `Signals the start of the dynamic data`
             * {
             *    [
             *       "name:unit": "DATA" `just like in the console module`
             *    ]
             * }
             * |||END_DYNAMIC||| `After each dynamic data update`
             * |||END||| `Signals the closing of the socket`
             */

            // --- Send Static Data to Client ---

            // Create the JSON object
            json static_data;
            static_data["project-name"] = config_data_.general.project_name;
            static_data["main-board"] = config_data_.general.main_board;
            static_data["starting-loop"] = config_data_.general.starting_loop;
            static_data["console-enabled"] = config_data_.debug.console_enabled;
            static_data["telemetry-enabled"] = 
                config_data_.telemetry.telemetry_enabled;

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
                //extension_json["extra-args"] = extension.extra_args;
                static_data["extensions"].push_back(extension_json);
            }


            new_sock << static_data.dump(); // Send the static data
            try {
                while (!stop_flag_) { // Loop through the data stream
                    std::this_thread::sleep_for(
                        std::chrono::milliseconds(update_interval_));

                    DataFrame snapshot = data_stream_->getDataFrameCopy();
                    json data;
                    int i = 0;
                    for (auto const& [key, packet] : snapshot) {
                        data[std::to_string(i++)] = {
                            {"source", packet.source},
                            {"unit", packet.unit},
                            {"value", packet.value}
                            };
                    }
                    new_sock << data.dump(); // Send the data
                }
            }
            catch (SocketException &e) {
                data_stream_->addError(
                    MODULE_SERVER_ERROR_PREFIX, 
                    "Socket Connection",
                    e.description(), 
                    update_interval_);
            }

            // End of the socket connection
            // ----------------------------------
            // ----------------------------------

        }
    }
    catch (SocketException &e) {
        data_stream_->addError(MODULE_SERVER_ERROR_PREFIX, "Socket Creation",
        e.description(), update_interval_);
    }
}