#include "data_log.hpp"
#include <BoosterSeat/filesystem.hpp>
#include <BoosterSeat/time.hpp>
#include <nlohmann/json.hpp>

#include <iostream>

using namespace data_middleware;

std::string generateFileName() {
  std::string file_name = "data_log_" + BoosterSeat::time::getDateAndTimeStr();
  file_name += ".txt";
  return file_name;
}

DataLog::DataLog(data::SharedData &shared_data, cfg::Configuration &config)
    : shared_data_(shared_data), config_(config) {
  data_frame_stopwatch_.start();
  data_log_file_path_ = kDataLogPath + generateFileName();
  checkFileSystem();
};

void DataLog::logDataPacket(const data::DataPacket &packet) {
  // ignore status packets, there are a lot of them.
  if (packet.type == data::DataPacket::Type::GENERIC) {
    std::cout << "DataLog::logDataPacket" << std::endl;
    std::cout << node::identification_to_string.at(packet.source) << std::endl;
    std::cout << static_cast<int>(packet.identifier) << std::endl;
    std::cout << packet.value << std::endl;
  }
}

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

void DataLog::logLogPacket(const data::LogPacket &packet) {
  (void)packet;
  std::cout << "DataLog::logLogPacket" << std::endl;
}

void DataLog::checkFileSystem() {
  std::cout << "DataLog::createDataLogFile" << std::endl;
  data::blocks::DataLogStats data_log_stats =
      shared_data_.blocks.data_log_stats.get();
  
  data_log_stats.data_log_file_path = data_log_file_path_;

  shared_data_.blocks.data_log_stats.set(data_log_stats);
}

void DataLog::appendStringToDataLog(const std::string &text) {
  (void)text;
  std::cout << "DataLog::appendStringToDataLog" << std::endl;
}