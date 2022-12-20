/**
 * @file module-data.cpp
 * @author Joshua Jerred (github.com/joshua-jerred)
 * @brief This file implements the class DataModule which is defined
 * in module-data.h.
 * @version 0.1
 * @date 2022-10-03
 * @copyright Copyright (c) 2022
 */

#include "module-data.h"

/**
 * @brief Construct a new DataModule object,
 * @details Initializes the DataStream, opens the log files for data and errors,
 * sets up the data frame. This does not start the module.
 * @param config_data
 */
DataModule::DataModule():
  Module(nullptr, MODULE_DATA_PREFIX) {
  p_data_stream_ = new DataStream();

  std::time_t t = std::time(0);
  std::tm* now = std::localtime(&t);

  data_log_file_path_ =
      DATA_LOG_LOCATION + std::to_string(now->tm_year + 1900) + "-" +
      std::to_string(now->tm_mon + 1) + "-" + std::to_string(now->tm_mday) +
      "-" + std::to_string(now->tm_hour) + "-" + std::to_string(now->tm_min) +
      "-" + std::to_string(now->tm_sec) + ".csv";

  error_log_file_path_ =
      ERROR_LOG_LOCATION + std::to_string(now->tm_year + 1900) + "-" +
      std::to_string(now->tm_mon + 1) + "-" + std::to_string(now->tm_mday) +
      "-" + std::to_string(now->tm_hour) + "-" + std::to_string(now->tm_min) +
      "-" + std::to_string(now->tm_sec) + ".csv";
}

/**
 * @brief Destroys the DataModule object.
 */
DataModule::~DataModule() {
  delete p_data_stream_;  // Deconstructor of data stream will first acquire all
                          // locks
}

void DataModule::addConfigData(ConfigData config_data) {
  for (ConfigData::DataTypes::ExtensionDataType data_type :
       config_data.data_types.types) {  // for each data type in the config file
    addDataTypeToFrame(data_type);
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
  module_status_ = ModuleStatus::STARTING;
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
  shutdown_signal_ = 1;
  runner_thread_.join();
  module_status_ = ModuleStatus::STOPPED;
}

/**
 * @brief Returns a pointer to the DataStream.
 * @param None
 * @return DataStream*
 */
DataStream* DataModule::getDataStream() { return p_data_stream_; }

/**
 * @brief logs the data in the data frame to the data log file.
 * @details This function will make a copy of the DataFrame and it will then log
 * it to the data log file.
 * @param None
 * @return void
 */
void DataModule::log() {
  std::ofstream logfile;
  logfile.open(data_log_file_path_, std::ios_base::app);

  DataFrame dataframe_copy(p_data_stream_->getDataFrameCopy());

  std::time_t t = std::time(0);
  std::tm* now = std::localtime(&t);

  for (auto& [source_and_unit, packet] : dataframe_copy) {
    logfile << now->tm_hour << ":" << now->tm_min << ":" << now->tm_sec << ", "
            << packet.source << ", " << packet.unit << ", " << packet.value
            << std::endl;
  }

  std::ofstream error_logfile;
  error_logfile.open(error_log_file_path_, std::ios_base::app);
  for (auto& [source_and_unit, packet] : errorframe_) {
    error_logfile << packet.source << ", " << packet.error_code << ", "
                  << packet.info << std::endl;
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
    ConfigData::DataTypes::ExtensionDataType data_type) {
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
  int packetCount = p_data_stream_->getNumDataPackets();
  DataStreamPacket dpacket;
  for (int i = 0; i < packetCount; i++) {
    /** @todo Check to see if it exists in the dataframe first, if not
     * add an error.
     */
    dpacket = p_data_stream_->getNextDataPacket();
    dataframe_.insert_or_assign(dpacket.source + ":" + dpacket.unit, dpacket);
  }
  p_data_stream_->updateDataFrame(dataframe_);
}

/**
 * @brief Parses the errors from the DataStream into the errorframe.
 * @details This is called within the runner thread. It will pull all of the
 * errors from the stream and add them to the errorframe. These errors will
 * be removed from the frame after they expire or have been resolved.
 * @param None
 * @return void
 */
void DataModule::parseErrorStream() {
  std::ofstream error_file;
  error_file.open(error_log_file_path_, std::ios_base::app);

  int packetCount = p_data_stream_->getNumErrorPackets();
  ErrorStreamPacket epacket;
  for (int i = 0; i < packetCount; i++) {
    epacket = p_data_stream_->getNextErrorPacket();
    errorframe_.insert_or_assign(
        epacket.source + ":" + epacket.error_code, epacket);
  }
  p_data_stream_->updateErrorFrame(errorframe_);
}

/**
 * @brief This function checks for state data within the dataframe.
 * @details This function looks at the expiry time of each item within the
 * data frame and if it has expired it will change it's value to "NO_DATA".
 * @param None
 * @return void
 */
void DataModule::checkForStaleData() {
  std::time_t now = std::time(NULL);
  for (auto& [source_and_unit, packet] : dataframe_) {
    if (packet.expiration_time == 0) {  // 0 means it never expires
      continue;
    }
    if ((int)packet.expiration_time < (int)now) {
      packet.value = "NO-DATA";
    }
  }
}

void DataModule::checkForStaleErrors() {
  if (errorframe_.empty()) {
    return;
  }
  std::time_t now = std::time(NULL);
  for (auto& [source_and_unit, packet] : errorframe_) {
    if (packet.expiration_time == 0) {  // 0 means it never expires
      continue;
    }
    if ((int)packet.expiration_time < (int)now) {
      errorframe_.erase(source_and_unit);
    }
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
  module_status_ = ModuleStatus::RUNNING;
  while (!shutdown_signal_) {
    std::this_thread::sleep_for(std::chrono::milliseconds(
        MODULE_DATA_FRAME_UPDATE_INTERVAL_MILI_SECONDS));
    parseDataStream();
    parseErrorStream();
    checkForStaleData();
    checkForStaleErrors();
  }
}
