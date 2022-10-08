#ifndef MODULE_CONSOLE_H_
#define MODULE_CONSOLE_H_


#include <string>
#include <thread>
#include <atomic>

#include "module.h"

#include "utility-config-types.h"
#include "utility-data-stream.h"
#include "utility-data-snapshot.h"

#include "module-data.h"

class ConsoleModule : public Module {
public:
    ConsoleModule(const ConfigData config_data, DataModule *data);
    ~ConsoleModule();
    void start();
    void stop();

private:
    void runner();
    void clearScreen();
    void printData();

    ConfigData config_data_;
    DataModule* data_;
    int update_interval_;
    std::thread runner_thread_;
    std::atomic <int> stop_flag_;
};

#endif