/**
 * @file modules.h
 * @author Joshua Jerred (https://joshuajer.red/)
 * @brief Defines the enum module_status and parent class for the modules.
 * @version 0.1
 * @date 2022-09-25
 *
 * @copyright Copyright (c) 2022
 * @todo Documentation
 */

#ifndef MODULE_H_
#define MODULE_H_

#include <atomic>
#include <nlohmann/json.hpp>
#include <string>
#include <thread>
using json = nlohmann::ordered_json;

#include "configurables.h"
#include "data-stream.h"
#include "extensions.h"
#include "socket.h"
#include "status.h"
#include "mwav.h"
#include "radio.h"

namespace modules {
/**
 * @brief All modules used by the FlightRunner inherit this class. This allows
 * for some simplicity when creating small modules.
 */
class Module {
 public:
  Module(DataStream &stream, std::string error_prefix, std::string module_name);

  Module(const Module &) = delete;             // No copy constructor
  Module &operator=(const Module &) = delete;  // No copy assignment
  virtual ~Module(){};

  ModuleStatus status() { return module_status_; };
  virtual void start() { updateStatus(ModuleStatus::ERROR_STATE); };
  virtual void stop() { updateStatus(ModuleStatus::ERROR_STATE); };

  bool addCommand(GFSCommand command);

 protected:
  void updateStatus(ModuleStatus new_status);

  void error(std::string error_code, std::string info);
  void error(std::string error_code, int info);
  void error(std::string error_code);

  void data(std::string data_name, std::string data_value);
  void data(std::string data_name, int data_value);
  void data(std::string data_name, double data_value, int precision = 2);

  const int kDefaultSleepTime_ = 1000;  // 1 second, default sleep time

  /**
   * @brief std::this_thread::sleep_for() with a default sleep time set
   * by kDefaultSleepTime_.
   *
   * @param sleep_time Optional, time to sleep in milliseconds.
   */
  void module_sleep(int sleep_time = -1);

  DataStream &data_stream_;
  std::atomic<ModuleStatus> module_status_;

  // Separate command queue to avoid the numerous race conditions that will
  // appear with later implementation.
  void parseCommands();
  virtual void doCommand(GFSCommand command) {
    error("MCNI", command.id);  // Module Command Not Implemented
  };
  void CommandArgumentError(std::string command_name, std::string argument);

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
  TelemetryModule(ConfigData config_data, DataStream &stream);
  TelemetryModule(const TelemetryModule &other) =
      delete;  // No copy constructor
  TelemetryModule &operator=(const TelemetryModule &other) =
      delete;  // No copy assignment
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
  void ChooseRadio();

  // TX Queue is stored in the data stream
  void addToTXQueue(Transmission transmission);

  std::string generateAFSK(const std::string &message, const int tx_number);
  std::string generatePSK(const std::string &message, const int tx_number);
  std::string GenerateAprsLocation(const int tx_number);
  std::string GenerateAprsTelemetry(const int tx_number);
  std::string GenerateSSTV(const int tx_number);

  void runner();
  void Transmit(std::string wav_location, std::string tx_type, int tx_length);
  FILE *aplay_fp_ = nullptr;

  mwav::AprsRequiredFields aprs_generic_ = mwav::AprsRequiredFields();

  int tx_number_ = 0;  // TX ID, first TX is 1
  std::string call_sign_ = "";

  std::thread tx_thread_ = std::thread();
  std::atomic<int> stop_flag_ = 0;

  void doCommand(GFSCommand command);  // Override Module::doCommand()

  std::vector<radio::Radio *> radios_= {};
  radio::Radio *primary_radio_ = nullptr;
  int primary_radio_index_ = -1;

  std::string data_frequency = {};
  std::string aprs_frequency = {};
  std::string sstv_frequency = {};

  int psk_length_ = 0;
  ConfigData config_data_;
};

class ServerModule : public Module {
 public:
  ServerModule(const ConfigData config_data, DataStream &stream);
  ServerModule(const ServerModule &) = delete;  // No copy constructor
  ServerModule &operator=(const ServerModule &) = delete;  // No copy assignment
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
  void sendLogFiles(ServerSocket &socket);

  ConfigData config_data_;

  std::thread runner_thread_ = std::thread();
  // std::thread py_runner_thread_;

  /**
   * @details This flag is an atomic so it can be accessed by both the thread
   * and the main thread. It is set to 1 to signal the thread to stop.
   */
  std::atomic<int> stop_flag_ = 0;
  std::atomic<int> gfs_shutdown_flag_ = 0;
};

/**
 * @brief The extension class manages all extensions.
 * @details This class is responsible for starting, stopping, and managing
 * extensions. This includes all sensors and data collection tools.
 */
class ExtensionsModule : public Module {
 public:
  ExtensionsModule(const ConfigData config_data, DataStream &stream);
  ExtensionsModule(const ExtensionsModule &other) =
      delete;  // No copy constructor
  ExtensionsModule &operator=(const ExtensionsModule &other) =
      delete;  // No copy assignment
  ~ExtensionsModule();
  void start();
  void stop();

  // extension_reply command(extension_command command);

 private:
  void runner();
  std::thread runner_thread_ = std::thread();
  std::atomic<int> stop_flag_ = 0;

  std::vector<extension::Extension *> extensions_ = {};
  ConfigData config_data_;

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
  DataModule(const DataModule &) = delete;             // No copy constructor
  DataModule &operator=(const DataModule &) = delete;  // No copy assignment
  ~DataModule();

  void addConfigData(ConfigData config_data);

  void start();
  void stop();

  DataFrame getSnapshot();
  void log();

  void doCommand(GFSCommand command);  // Override Module::doCommand()

 private:
  void runner();
  void addDataTypeToFrame(ConfigData::DataTypes::DataType
                              data_type);  // add a data type to the data frame
  void checkForStaleData();
  void parseDataStream();
  void parseGPSData();
  void parseCriticalData();
  void parseErrorStream();

  void checkForStaleErrors();

  void UpdateLogFilesList();
  void RotateLogFiles();
  void DeleteDataLogFile(std::string file_name);
  void DeleteErrorLogFile(std::string file_name);

  bool checkGPSFrame(const GPSFrame &gps_frame);

  CriticalData critical_data_ = CriticalData();

  GPSFrame latest_gps_frame_ = GPSFrame();
  GPSFrame last_valid_fix_gps_frame_ = GPSFrame();

  std::string data_log_directory_ = "";
  std::string error_log_directory_ = "";
  std::string log_files_name_ = "";

  DataFrame dataframe_ = DataFrame();
  ErrorFrame errorframe_ = ErrorFrame();

  std::atomic<int> shutdown_signal_ = 0;
  std::thread runner_thread_ = std::thread();

  ConfigData config_data_ = {};

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
  ConsoleModule(const ConfigData config_data, DataStream &stream);
  ConsoleModule(const ConsoleModule &) = delete;  // No copy constructor
  ConsoleModule &operator=(const ConsoleModule &) =
      delete;  // No copy assignment
  ~ConsoleModule();

  void start();
  void stop();

 private:
  void runner();
  void clearScreen();
  void printData();

  ConfigData config_data_;

  int update_interval_ = 10;

  std::thread runner_thread_ = std::thread();

  /**
   * @details This flag is an atomic so it can be accessed by both the thread
   * and the main thread. It is set to 1 to signal the thread to stop.
   */
  std::atomic<int> stop_flag_ = 1;
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
class ConfigModule : public Module {
 public:
  ConfigModule(DataStream &stream);
  ConfigModule(const ConfigModule &other) = delete;  // no copy constructor
  ConfigModule &operator=(const ConfigModule &other) =
      delete;  // no copy assignment
  ~ConfigModule();

  int load(std::string filepath);
  ConfigData getAll();
  int getNumberOfErrors();

  void doCommand(GFSCommand command);  // Override Module::doCommand()

 private:
  void ParseAll();

  void parseGeneral();
  void parseExtensions();
  void parseDebug();
  void parseTelemetry();
  void ParseRadios(json radio_json);
  void parseDataTypes();
  void parseFlightProcedures();
  std::string config_file_path_ = "";
  json json_buffer_ = json::object();
  ConfigData config_data_ = ConfigData();
};

}  // namespace modules
#endif