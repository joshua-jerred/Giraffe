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
  /**
   * @brief Construct a new DataLog::DataLog object
   *
   * @param shared_data @see data::SharedData
   * @param config @see cfg::Configuration
   */
  DataLog(data::SharedData &shared_data, cfg::Configuration &config);
  ~DataLog() = default;

  /**
   * @brief Used to add a single data packet to the file log.
   * @param packet @see data::DataPacket
   */
  void logDataPacket(const data::DataPacket &packet);

  /**
   * @brief Used for periodic logging of all data.
   */
  void logDataFrame();

  /**
   * @brief Used to log a single log packet to the file log.
   * @param packet @see data::LogPacket
   */
  void logLogPacket(const data::LogPacket &packet);

private:
  /**
   * @brief Checks the log file(s) for validity and size. Creates a new log
   * file if necessary. Also checks the archive directories for size.
   */
  void checkFileSystem();
  std::string data_log_file_path_ = "";
  bool valid_data_log_file_path_ = false;

  /**
   * @brief Appends a string to the data log file.
   * @param text The string to append to the data log file.
   */
  void appendStringToDataLog(const std::string &text);

  data::SharedData &shared_data_;
  cfg::Configuration &config_;

  BoosterSeat::Stopwatch data_frame_stopwatch_ = BoosterSeat::Stopwatch();
};

}; // namespace data_middleware

#endif /* DATALOG_HPP_ */