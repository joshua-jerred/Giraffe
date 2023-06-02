#ifndef DATA_LOG_HPP_
#define DATA_LOG_HPP_

#include "configuration.hpp"
#include "shared_data.hpp"
#include <BoosterSeat/stopwatch.hpp>

namespace data_middleware {

inline const std::string kDataLogPath = "./data/";
inline const std::string kDataLogArchivePath = "./data/archive/";

class DataLog {
public:
  DataLog(data::SharedData &shared_data, cfg::Configuration &config);
  ~DataLog() = default;

  void logDataPacket(const data::DataPacket &packet);
  void logDataFrame();

  void logLogPacket(const data::LogPacket &packet);

private:
  void createDataLogFile();
  void appendStringToDataLog(const std::string &text);

  data::SharedData &shared_data_;
  cfg::Configuration &config_;
  
  BoosterSeat::Stopwatch data_frame_stopwatch_ = BoosterSeat::Stopwatch();
};

}; // namespace data_middleware

#endif /* DATALOG_HPP_ */