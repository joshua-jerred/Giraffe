#include "data_log.hpp"
#include <nlohmann/json.hpp>

#include <iostream>

using namespace data_middleware;

/**
 * @brief Construct a new DataLog::DataLog object
 * 
 * @param shared_data @see data::SharedData
 * @param config @see cfg::Configuration
 */
DataLog::DataLog(data::SharedData &shared_data, cfg::Configuration &config)
    : shared_data_(shared_data), config_(config) {
  data_frame_stopwatch_.start();
};

/**
 * @brief Used to add a single data packet to the file log.
 * @param packet @see data::DataPacket
 */
void DataLog::logDataPacket(const data::DataPacket &packet) {
  // ignore status packets, there are a lot of them.
  if (packet.type == data::DataPacket::Type::GENERIC) {
    std::cout << "DataLog::logDataPacket" << std::endl;
    std::cout << node::identification_to_string.at(packet.source) << std::endl;
    std::cout << static_cast<int>(packet.identifier) << std::endl;
    std::cout << packet.value << std::endl;
  }
}

/**
 * @brief Used for periodic logging of all data.
 * @param packet @see data::LogPacket
 */
void DataLog::logDataFrame() {
  int log_interval_ms = config_.data_module_data.getLogIntervalMs();
  int time_since_last_log_ms =
      data_frame_stopwatch_.elapsed(BoosterSeat::Resolution::MILLISECONDS);

  if (time_since_last_log_ms > log_interval_ms) {
    std::cout << "DataLog::logDataFrame" << std::endl;

    data_frame_stopwatch_.reset();
    data_frame_stopwatch_.start();
  }
}

/**
 * @brief Used to log a single log packet to the file log.
 * @param packet @see data::LogPacket
 */
void DataLog::logLogPacket(const data::LogPacket &packet) {
  (void)packet;
  std::cout << "DataLog::logLogPacket" << std::endl;
}

/**
 * @brief Creates a new data log file.
 */
void DataLog::createDataLogFile() {
  std::cout << "DataLog::createDataLogFile" << std::endl;
}

/**
 * @brief Appends a string to the data log file.
 * @param text The string to append to the data log file.
 */
void DataLog::appendStringToDataLog(const std::string &text) {
  (void)text;
  std::cout << "DataLog::appendStringToDataLog" << std::endl;
}