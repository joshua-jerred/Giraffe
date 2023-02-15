/**
 * @file modules.h
 * @author Joshua Jerred (https://joshuajer.red/)
 * @brief Defines the enum module_status and parent class for the modules.
 * @version 0.1
 * @date 2022-09-25
 * 
 * @copyright Copyright (c) 2022
 */

#ifndef MODULE_H_
#define MODULE_H_

#include <string>
#include <atomic>
#include <thread>
#include <nlohmann/json.hpp>
using json = nlohmann::ordered_json; 

#include "status.h"
#include "data-stream.h"
#include "configurables.h"
#include "socket.h"

#include "extensions.h"

namespace modules {
/**
 * @brief All modules used by the FlightRunner inherit this class. This allows
 * for some simplicity when creating small modules.
 */
class Module {
    public:
        Module(DataStream &stream, std::string error_prefix, std::string module_name);

        Module(const Module&) = delete; // No copy constructor
        Module& operator=(const Module&) = delete; // No copy assignment
        virtual ~Module( ) { };

        ModuleStatus status( ) { return module_status_; };
        virtual void start( ) { updateStatus(ModuleStatus::ERROR_STATE); };
        virtual void stop( ) { updateStatus(ModuleStatus::ERROR_STATE); };

        bool addCommand(GFSCommand command);

    protected:
        void updateStatus(ModuleStatus new_status);

        void error(std::string error_code, std::string info);
        void error(std::string error_code);

        void data(std::string data_name, std::string data_value);
        void data(std::string data_name, int data_value);

        const int kDefaultSleepTime_ = 1000; // 1 second, default sleep time

        /**
         * @brief std::this_thread::sleep_for() with a default sleep time set
         * by kDefaultSleepTime_.
         * 
         * @param sleep_time Optional, time to sleep in milliseconds.
         */
        void module_sleep(int sleep_time = -1);

        DataStream &data_stream_;
        std::atomic<ModuleStatus> module_status_;

        
        // Separate command queue to avoid the numerous race conditions that will appear with later implementation.
        void parseCommands();
        virtual void doCommand(GFSCommand command) {
            error("MCNI", command.id); // Module Command Not Implemented
        }; 

    private:
        std::mutex command_queue_lock_ = std::mutex();
        std::queue<GFSCommand> command_queue_ = std::queue<GFSCommand>();
        
        std::string error_source_;
        std::string module_name_;
};

/**
 * @brief This class is responsible for handling the radio
 * and transmitting data upon request. Currently only supports PSK.
 * @todo Two way communication, AFSK, APRS, etc.
 */
class TelemetryModule : public Module {
public:
    TelemetryModule(Data config_data, DataStream &stream);
    TelemetryModule(const TelemetryModule &other) = delete; // No copy constructor
    TelemetryModule &operator=(const TelemetryModule &other) = delete; // No copy assignment
    ~TelemetryModule();

    void start();
    void stop();

    void sendDataPacket();
    void sendAFSK(std::string message);
    void sendPSK(std::string message);
    void sendAPRS();
    void sendSSTVImage();
    
private:
    int getNextTXNumber();

    // TX Queue is stored in the data stream
    void addToTXQueue(Transmission transmission);

    std::string generateAFSK(const std::string &message, const int tx_number);
    std::string generatePSK(const std::string &message, const int tx_number);
    std::string generateAPRS();
    std::string generateSSTV();

    void runner();
    void playWav(std::string wav_location, std::string tx_type, int tx_length);
    FILE *aplay_fp_ = nullptr;

    int tx_number_ = 0; // TX ID, first TX is 1
    std::string call_sign_ = CALLSIGN_FAILSAFE;

    std::thread tx_thread_ = std::thread();
    std::atomic<int> stop_flag_ = 0;

    void doCommand(GFSCommand command); // Override Module::doCommand()

    Data config_data_;
    int psk_length_ = 0;
};

class ServerModule : public Module {
public:
    ServerModule(const Data config_data, DataStream &stream);
    ServerModule(const ServerModule&) = delete; // No copy constructor
    ServerModule& operator=(const ServerModule&) = delete; // No copy assignment
    ~ServerModule();

    void start();
    void stop();

    int checkShutdown();

private:
    void runner();
    void sendConfig(ServerSocket &socket);
    void sendStatus(ServerSocket &socket);
    void sendGfsData(ServerSocket &socket);
    void sendExtensionStatuses(ServerSocket &socket);
    void sendTelemetryData(ServerSocket &socket);
    void sendDataFrame(ServerSocket &socket);
    void sendErrorFrame(ServerSocket &socket);

    Data config_data_;

    std::thread runner_thread_ = std::thread();
    //std::thread py_runner_thread_;

    /**
     * @details This flag is an atomic so it can be accessed by both the thread
     * and the main thread. It is set to 1 to signal the thread to stop.
     */
    std::atomic <int> stop_flag_ = 0;
    std::atomic <int> gfs_shutdown_flag_ = 0;
};

/**
 * @brief The extension class manages all extensions.
 * @details This class is responsible for starting, stopping, and managing
 * extensions. This includes all sensors and data collection tools.
 */
class ExtensionsModule : public Module {
public:
    ExtensionsModule(const Data config_data, DataStream &stream);
    ExtensionsModule(const ExtensionsModule &other) = delete; // No copy constructor
    ExtensionsModule &operator=(const ExtensionsModule &other) = delete; // No copy assignment
    ~ExtensionsModule();
    void start();
    void stop();

    //extension_reply command(extension_command command);

private:
    void runner();
    std::thread runner_thread_ = std::thread();
    std::atomic<int> stop_flag_ = 0;

    std::vector<extension::Extension*> extensions_ = {};
    Data config_data_;

    void addExtension(ExtensionMetadata meta_data);
};

/**
 * @brief The DataModule class is responsible for managing all data between
 * modules. It is responsible for creating safe structures that different
 * concurrently running threads can access. It is in charge of the
 * DataStream. The DataStream is a queue that all of the extensions have
 * access to. They can add data whenever they want. The data module is 
 * responsible for collecting this data from the stream and placing it into a
 * data frame which is then sent to the datastream so other modules can access 
 * it.
 * This module also continually goes through the DataFrame looking for stale
 * data. If it finds stale data, it will set it's value to 'NO_DATA'.
 * This is an indication of an error within an extension.
 * 
 * The DataModule is also responsible for logging the data in the dataframe
 * to a log file when requested to do so.
 * 
 * The DataModule also logs errors which are collected through the datastream
 * in the same way as the data.
 * 
 * The data and error log directories can be set inside of 
 * 'configurables.h'.
 */
class DataModule : public Module {
public:
    DataModule(DataStream &data_stream);
    DataModule(const DataModule&) = delete; // No copy constructor
    DataModule& operator=(const DataModule&) = delete; // No copy assignment
    ~DataModule();

    void addConfigData(Data config_data);

    void start();
    void stop();

    DataFrame getSnapshot();
    void log();

    void doCommand(GFSCommand command); // Override Module::doCommand()

private:
    void addDataTypeToFrame(Data::DataTypes::DataType data_type); // add a data type to the data frame
    
    void checkForStaleData(); // check for stale data in the data frame
    void parseDataStream();
    void parseGPSData();

    void parseCriticalData();

    void checkForStaleErrors();
    void parseErrorStream();
    
    void runner();

    CriticalData critical_data_ = CriticalData();
    
    GPSFrame latest_gps_frame_ = GPSFrame();
    GPSFrame last_valid_fix_gps_frame_ = GPSFrame();

    std::string data_log_file_path_ = "";
    std::string error_log_file_path_ = "";

    DataFrame dataframe_ = DataFrame();
    ErrorFrame errorframe_ = ErrorFrame();

    std::atomic<int> shutdown_signal_ = 0;
    std::thread runner_thread_ = std::thread();

    Data config_data_ = {};

    std::string gps_data_source_ = "";
    std::string pressure_data_source_ = "";
    std::string battery_data_source_ = "";
    std::string system_data_source_ = "";
    std::string radio_data_source_ = "";
};

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
    ConsoleModule(const Data config_data, DataStream &stream);
    ConsoleModule(const ConsoleModule&) = delete; // No copy constructor
    ConsoleModule& operator=(const ConsoleModule&) = delete; // No copy assignment
    ~ConsoleModule();

    void start();
    void stop();

private:
    void runner();
    void clearScreen();
    void printData();

    Data config_data_;

    int update_interval_ = 10;

    std::thread runner_thread_ = std::thread();

    /**
     * @details This flag is an atomic so it can be accessed by both the thread
     * and the main thread. It is set to 1 to signal the thread to stop.
     */
    std::atomic <int> stop_flag_ = 1;
};

/**
 * @brief ConfigModule is used to load all of the configuration data from the 
 * config file. 
 * @details The configuration is read with Nlohmann JSON and then the data is 
 * placed into a common configuration type that is recognized system wide.
 * During parsing, the configuration module will check for errors and will
 * store them in a vector of strings. The errors can be retrieved with the
 * getErrors() function later. If everything was read and parsed correctly,
 * the getErrors() function will return an empty vector.
 * 
 * Basic usage looks like this:
 * ConfigModule config;
 * config.load("path/to/config.json");
 * config.getErrors(); // check for errors
 * config.getAll(); // get all of the configuration data
 */
class ConfigModule {
public:
    ConfigModule(DataStream &stream);
    ConfigModule(const ConfigModule &other) = delete; // no copy constructor
    ConfigModule &operator=(const ConfigModule &other) = delete; // no copy assignment
    ~ConfigModule();

    int load(std::string filepath);
    Data getAll();
    // json getAllJson();
    int getNumberOfErrors();

private:
    template <typename T>
    void error(std::string error_code, T info);
    void error(std::string error_code, std::string info);
    void error(std::string error_code);


    void parseAll();

    void parseGeneral();
    void parseExtensions();
    void parseDebug();
    void parseTelemetry();
    void parseDataTypes();
    void parseFlightProcedures();

    int number_of_errors_ = 0;

    DataStream &data_stream_;
    std::string config_file_path_ = "";
    json json_buffer_ = json::object();
    Data config_data_ = Data();
};

} // namespace modules
#endif