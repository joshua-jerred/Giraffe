/**
 * @file module-data.cpp
 * @author Joshua Jerred (https://joshuajer.red/)
 * @brief This file implements the class DataModule which is defined
 * in module-data.h.
 * @version 0.1
 * @date 2022-10-03
 * @copyright Copyright (c) 2022
 */

#include <fstream>
#include <filesystem>
#include <iostream>
#include <nlohmann/json.hpp>

#include "modules.h"
using namespace modules;

/**
 * @brief Construct a new DataModule object,
 * @details Initializes the DataStream, opens the log files for data and errors,
 * sets up the data frame. This does not start the module.
 * @param config_data
 */
DataModule::DataModule(DataStream &stream):
  Module(stream, MODULE_DATA_PREFIX, "data") {

  std::time_t t = std::time(0);
  std::tm* now = std::localtime(&t);

  const std::string absolute_path = std::filesystem::current_path().string();
  const std::string current_time = std::to_string(now->tm_year + 1900) + "-" +
      std::to_string(now->tm_mon + 1) + "-" + std::to_string(now->tm_mday) +
      "-" + std::to_string(now->tm_hour) + "-" + std::to_string(now->tm_min) +
      "-" + std::to_string(now->tm_sec) + ".csv";
  
  data_log_file_path_ = absolute_path + "/data_logs/";
  error_log_file_path_ = absolute_path + "/error_logs/";

  if (!std::filesystem::exists(data_log_file_path_)) {
    std::filesystem::create_directory(data_log_file_path_);
  }
  if (!std::filesystem::exists(error_log_file_path_)) {
    std::filesystem::create_directory(error_log_file_path_);
  }

  data_log_file_path_ += current_time;
  error_log_file_path_ += current_time;

  std::cout << "Data Log: " << data_log_file_path_ << std::endl;
  std::cout << "Error Log: " << error_log_file_path_ << std::endl;
}

/**
 * @brief Destroys the DataModule object.
 */
DataModule::~DataModule() {
}

void DataModule::addConfigData(Data config_data) {
    config_data_ = config_data;

    // Check for GPS Extensions
    if (config_data_.extensions.gps_data_name.size() > 0) {
        gps_data_source_ = config_data_.extensions.gps_data_name;
    } else {
        error("NGPS");
    }

    if (config_data_.extensions.pressure_data_name.size() > 0) {
        pressure_data_source_ = config_data_.extensions.pressure_data_name;
    } else {
        error("NPRES");
    }

    if (config_data_.extensions.battery_data_name.size() > 0) {
        battery_data_source_ = config_data_.extensions.battery_data_name;
    } else {
        error("NBAT");
    }

    if (config_data_.extensions.system_data_name.size() > 0) {
        system_data_source_ = config_data_.extensions.system_data_name;
    } else {
        error("NSYS");
    }

    if (config_data_.extensions.radio_data_name.size() > 0) {
        radio_data_source_ = config_data_.extensions.radio_data_name;
    } else {
        error("NRAD");
    }
}

/**
 * @brief Starts the DataModule.
 * @details Starts the thread that parses the DataStream into the DataFrame
 * and logs the data and errors.
 * @param None
 * @return void
 */
void DataModule::start() {
  updateStatus(ModuleStatus::STARTING);
  shutdown_signal_ = 0;
  runner_thread_ = std::thread(&DataModule::runner, this);
}

/**
 * @brief Stops the DataModule.
 * @details Stops the thread that parses the DataStream. Currently not
 * implemented.
 * @param None
 * @return void
 * @todo Implement this.
 */
void DataModule::stop() {
  updateStatus(ModuleStatus::STOPPING);
  shutdown_signal_ = 1;
  runner_thread_.join();
  updateStatus(ModuleStatus::STOPPED);
}

void DataModule::log() {
  std::ofstream logfile;
  logfile.open(data_log_file_path_, std::ios_base::app);

  if (!logfile.is_open()) {
    error("LOGFILE");
  }

  DataFrame dataframe_copy(data_stream_.getDataFrameCopy());

  std::time_t t = std::time(0);
  std::tm* now = std::localtime(&t);

  logfile << std::endl; // Add a blank line between each log
  logfile << now->tm_mday << "/" << now->tm_mon + 1 << " ";
  logfile << now->tm_hour << ":" << now->tm_min << ":" << now->tm_sec << std::endl;
  //for (auto& [source_and_unit, packet] : dataframe_copy) { // Log each item in the dataframe
  //  logfile << now->tm_hour << ":" << now->tm_min << ":" << now->tm_sec << ", "
  //          << packet.source << ", " << packet.unit << ", " << packet.value
  //          << std::endl;
  //}
  std::string key = "";
  for (Data::DataTypes::DataType type : config_data_.data_types.types) {

    key = type.source + ":" + type.unit;
    if (dataframe_copy.contains(key)) {
        logfile << key + " - " + dataframe_copy[key].value << std::endl;
    } else {
        logfile << key + " - NO_DATA" << std::endl;
    }
  }

  float size = -1;
  try {
    size = std::filesystem::file_size(data_log_file_path_);
    size = size / 1024.0; // Convert to KB
    data("DLOG", size);
  } catch (std::filesystem::filesystem_error &e) {
    error("NLOG");
  }
}

/**
 * @brief Adds a data type to the data frame without data.
 * @details This sets up the data frame so it's ready to receive any data that
 * is sent to it. This is helpful to see what data is missing. Defines the
 * values as "NO_DATA" until they are overwritten. This should be called for
 * each data type at the start of the data module.
 * @param data_type The actual data type
 * @return void
 */
void DataModule::addDataTypeToFrame(
    Data::DataTypes::DataType data_type) {
  DataStreamPacket packet;
  packet.source = data_type.source;
  packet.unit = data_type.unit;
  packet.value = "NO_DATA";
  packet.expiration_time = 0;
  dataframe_.insert_or_assign(data_type.source + ":" + data_type.unit, packet);
}

/**
 * @brief Parses data from the DataStream into the DataFrame.
 * @details This is called within the runner thread. It will pull all of the
 * packets from the data stream and add them to the dataframe. It will then
 * update the dataframe within the datastream.
 * @param None
 * @return void
 */
void DataModule::parseDataStream() {
  int packetCount = data_stream_.getNumDataPackets();
  DataStreamPacket dpacket;
  for (int i = 0; i < packetCount; i++) {
    dpacket = data_stream_.getNextDataPacket();
    dataframe_.insert_or_assign(dpacket.source + ":" + dpacket.unit, dpacket);
  }
  data_stream_.updateDataFrame(dataframe_);
}

void DataModule::parseGPSData() {
  int packetCount = data_stream_.getNumGPSPackets();
  GPSFrame frame;
  for (int i = 0; i < packetCount; i++) {
    bool status = data_stream_.getNextGPSFrame(frame);
    if (status) {
      latest_gps_frame_ = frame;
      if (frame.fix == GPSFixType::FIX_3D || frame.fix == GPSFixType::FIX_2D) {
        last_valid_fix_gps_frame_ = frame;
      }
    }
  }

  critical_data_.gps_data = latest_gps_frame_;
}

/**
 * @brief Parses the errors from the DataStream into the errorframe.
 * @details This is called within the runner thread. It will pull all of the
 * errors from the stream and add them to the errorframe. These errors will
 * be removed from the frame after they expire or have been resolved.
 * 
 * This function will also log the errors to the error log file.
 * @param None
 * @return void
 */
void DataModule::parseErrorStream() {
  std::ofstream error_file;
  error_file.open(error_log_file_path_, std::ios_base::app);

  int packetCount = data_stream_.getNumErrorPackets();
  ErrorStreamPacket epacket;
  for (int i = 0; i < packetCount; i++) {
    epacket = data_stream_.getNextErrorPacket();

    if (!errorframe_.contains(epacket.source + ":" + epacket.error_code)) { // Log the error if it doesn't exist
      error_file << epacket.source << ", " << epacket.error_code << ", "
              << epacket.info << std::endl;
    }

    errorframe_.insert_or_assign(
        epacket.source + ":" + epacket.error_code, epacket);
  }
  data_stream_.updateErrorFrame(errorframe_);
}

void DataModule::parseCriticalData() {
  // Pressure
  if (dataframe_.contains(pressure_data_source_ + ":PRES_MBAR")) {
    try {
      float pressure = std::stof(dataframe_[pressure_data_source_ + ":PRES_MBAR"].value);
      const float kMinPressure = 0.0f; /** @todo make these configurable */
      const float kMaxPressure = 1000.0f;
      if (pressure < kMinPressure || pressure > kMaxPressure) {
        critical_data_.pressure_data_valid = false;
      } else {
        critical_data_.pressure_data_valid = true;
      }
      critical_data_.pressure_mbar = pressure;
    } catch (std::invalid_argument& e) {
      error("CDPE", "PRES_MBAR");
      critical_data_.pressure_data_valid = false;
    }
  } else {
    critical_data_.pressure_data_valid = false;
  }

  // Battery voltage
  if (dataframe_.contains(battery_data_source_ + ":BAT_V")) { // Check if the data is in the dataframe
    try {
      float voltage = std::stof(dataframe_[battery_data_source_ + ":BAT_V"].value);
      const float kMinVoltage = 3.0f; /** @todo make these configurable */
      const float kMaxVoltage = 17.0f; // 4S battery
      if (voltage < kMinVoltage || voltage > kMaxVoltage) {
        critical_data_.battery_data_valid = false;
      } else {
        critical_data_.battery_data_valid = true;
      }
      critical_data_.battery_voltage = voltage; // Report the voltage if it's valid or not
    } catch (std::invalid_argument& e) { // If the data is not a float
      error("CDPE", "BAT_V"); // Critical data parse error
      critical_data_.battery_data_valid = false;
    }
  } else {
    error("CD", "BAT_V"); // Data is not in the dataframe
    critical_data_.battery_data_valid = false;
  }

  // System Data
  critical_data_.system_data_valid = true; // Assume the data is valid until proven otherwise
  // Ram usage
  if (dataframe_.contains(system_data_source_ + ":ram_used_prcnt")) { 
    try {
      float ram_usage = std::stof(dataframe_[system_data_source_ + ":ram_used_prcnt"].value);
      const float kMaxRamUsage = 99.9f; /** @todo make these configurable */
      const float kMinRamUsage = 5.0f; // I don't expect the ram usage to be this low

      if (ram_usage > kMaxRamUsage || ram_usage < kMinRamUsage) {
        critical_data_.system_data_valid = false;
      }

      critical_data_.ram_usage = ram_usage;
    } catch (std::invalid_argument& e) {
      error("CDPE", "RAM"); // Critical data parse error
      critical_data_.system_data_valid = false;
    }
  } else {
    error("CD", "RAM"); // Data is not in the dataframe
    critical_data_.system_data_valid = false;
  }

  // Disk usage
  if (dataframe_.contains(system_data_source_ + ":disk_used_prcnt")) { 
    try {
      float disk_usage = std::stof(dataframe_[system_data_source_ + ":disk_used_prcnt"].value);
      const float kMaxDiskUsage = 99.9f; /** @todo make these configurable */
      const float kMinDiskUsage = 5.0f; // I don't expect the disk usage to be this low

      if (disk_usage > kMaxDiskUsage || disk_usage < kMinDiskUsage) {
        critical_data_.system_data_valid = false;
      }

      critical_data_.disk_usage = std::stof(dataframe_[system_data_source_ + ":disk_used_prcnt"].value);
    } catch (std::invalid_argument& e) {
      error("CDPE", "DISK");
      critical_data_.system_data_valid = false;
    }
  } else {
    error("CD", "DISK");
    critical_data_.system_data_valid = false;
  }

  // Radio Status
  if (dataframe_.contains(radio_data_source_ + ":radio_status")) {
      if (dataframe_[radio_data_source_ + ":radio_status"].value == "GOOD") {
        critical_data_.radio_status = true;
      } else {
        critical_data_.radio_status = false;
      }
  } else {
    error("CD", "RADIO");
    critical_data_.radio_status = false;
  }

  data_stream_.updateCriticalData(critical_data_);
}

void DataModule::checkForStaleData() {
  //std::time_t now = std::time(NULL);
  //for (auto& [source_and_unit, packet] : dataframe_) {
  //  if (packet.expiration_time == 0) {  // 0 means it never expires
  //    continue;
  //  }
  //  if ((int)packet.expiration_time + 1000 < (int)now) {
  //    packet.value = "EXPIRED";
  //  }
  //}
}

void DataModule::checkForStaleErrors() {
  if (errorframe_.empty()) {
    return;
  }
  //std::time_t now = std::time(NULL);
  // ErrorFrame::iterator it = errorframe_.begin();
  // while (it != errorframe_.end()) {
  //   if (it->second.expiration_time == 0) { // 0 mean it never expires}
  //     it++;
  //     continue;
  //   }
  //   if ((int)it->second.expiration_time < (int)now) {
  //     it = errorframe_.erase(it); // erase returns the next iterator
  //   } else {
  //     it++;
  //   }
  // }

  /* 12/21/22 - This created as error as the iterator was being deleted by
   * the erase function.
  for (auto& [source_and_unit, packet] : errorframe_) {
    if (packet.expiration_time == 0) {  // 0 means it never expires
      continue;
    }
    if ((int)packet.expiration_time < (int)now) {
      errorframe_.erase(source_and_unit);
    }
  }
  */
}

void DataModule::doCommand(GFSCommand command) {
  if (command.category != GFSCommand::CommandCategory::DAT) {
    error("CMDT", GFS_COMMAND_CATEGORY_TO_STRING.at(command.category));
    return;
  }

  std::string command_name = command.id;
  std::string command_arg = command.arg;

  if (command_name == "cae") { // clear all errors
    if (command_arg != "") { // There should be no arguments
      error("CMD_A", command_name + "$" + command_arg + "$"); // Command Argument Error
      return;
    }
    errorframe_.clear();
  } else {
    error("CMD_NF", command_name);
  }

}

/**
 * @brief This is the function that will run in it's own thread. It parses all
 * data and errors automatically.
 * @details This function will parse all of the data and errors from the stream
 * and then it will check for stale errors and data.
 * @param None
 * @return void
 * @todo Implement the error frame.
 */
void DataModule::runner() {
  while (!shutdown_signal_) {
    updateStatus(ModuleStatus::RUNNING);
    std::this_thread::sleep_for(std::chrono::milliseconds(
        MODULE_DATA_FRAME_UPDATE_INTERVAL_MILI_SECONDS));
    parseDataStream();
    parseGPSData();
    parseErrorStream();
    parseCriticalData();
    checkForStaleData();
    checkForStaleErrors();

    parseCommands();
  }
}
