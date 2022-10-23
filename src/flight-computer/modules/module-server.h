#ifndef MODULE_SERVER_H_
#define MODULE_SERVER_H_

#include <string>
#include <thread>
#include <atomic>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <nlohmann/json.hpp>

#include "utility-configurables.h"
#include "utility-config-types.h"
#include "utility-data-stream.h"
#include "utility-socket.h"

#include "module.h"

using json = nlohmann::ordered_json; 

class ServerModule : public Module {
public:
    ServerModule(const ConfigData config_data, DataStream *data);
    ~ServerModule();

    void start();
    void stop();

    int checkShutdown();

private:
    void runner();
    int pyRunner();
    void sendStaticData(ServerSocket &socket);
    void sendDynamicData(ServerSocket &socket);

    int update_interval_;

    ConfigData config_data_;
    DataStream* data_stream_;

    std::thread runner_thread_;
    std::thread py_runner_thread_;

    /**
     * @details This flag is an atomic so it can be accessed by both the thread
     * and the main thread. It is set to 1 to signal the thread to stop.
     */
    std::atomic <int> stop_flag_;
    std::atomic <int> gfs_shutdown_flag_;
};

#endif // MODULE_SERVER_H_