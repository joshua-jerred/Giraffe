/**
 * @file module-console.h
 * @author Joshua Jerred (github.com/joshua-jerred)
 * @brief 
 * 
 * @version 0.0.9
 * @date 2022-10-10
 * @copyright Copyright (c) 2022
 */

#ifndef MODULE_CONSOLE_H_
#define MODULE_CONSOLE_H_


#include <string>
#include <thread>
#include <atomic>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <ctime>


#include "utility-configurables.h"
#include "utility-config-types.h"
#include "utility-data-stream.h"

#include "module.h"


/**
 * @brief This class takes data from a data frame snapshot and prints it out
 * to a simple display in the console.
 * @details Currently runs in it's own thread to prevent any interruptions to
 * the main thread.
 * @todo This class should stop using the data module and use the data stream
 * instead.
 * @todo Redo all of this with ncurses.
 */
class ConsoleModule : public Module {
public:
    ConsoleModule(const ConfigData config_data, DataStream *data);
    ~ConsoleModule();

    void start();
    void stop();

private:
    void runner();
    void clearScreen();
    void printData();

    ConfigData config_data_;
    DataStream* data_stream_;

    int update_interval_;

    std::thread runner_thread_;

    /**
     * @details This flag is an atomic so it can be accessed by both the thread
     * and the main thread. It is set to 1 to signal the thread to stop.
     */
    std::atomic <int> stop_flag_;
};

#endif // MODULE_CONSOLE_H_