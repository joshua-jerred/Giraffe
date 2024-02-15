#ifndef INFLUXDB_HPP_
#define INFLUXDB_HPP_

#include <BoosterSeat/timer.hpp>

#include "configuration.hpp"
#include "shared_data.hpp"

#include <vector>

namespace data_middleware {

struct InfluxLine {
  std::string measurement{};
  std::map<std::string, std::string> tags{};
  std::map<std::string, std::string> fields{};
  std::time_t timestamp = 0;

  std::string getString() {
    std::string line = measurement;
    for (auto &tag : tags) {
      line += "," + tag.first + "=" + tag.second;
    }
    line += " ";
    for (auto &field : fields) {
      line += field.first + "=" + field.second + ",";
    }
    line.pop_back();

    if (timestamp != 0) {
      line += " " + std::to_string(timestamp);
    } else {
      uint64_t unix_time_ns =
          std::chrono::duration_cast<std::chrono::nanoseconds>(
              std::chrono::system_clock::now().time_since_epoch())
              .count();
      line += " " + std::to_string(unix_time_ns);
    }
    return line;
  }
};

class InfluxDb {
public:
  InfluxDb(data::SharedData &shared_data, cfg::Configuration &config)
      : shared_data_(shared_data), config_(config) {
    url_ = config_.data_module_influxdb.getUrl();
    org_ = config_.data_module_influxdb.getOrganization();
    data_bucket_ = config_.data_module_influxdb.getDataBucket();
    auth_token_ = config_.data_module_influxdb.getToken();
  };
  ~InfluxDb() = default;

  void logDataPacket(const data::DataPacket &packet);

  void logLogPacket(const data::LogPacket &packet);

  void writeFrames();

private:
  bool testConnection();

  bst::Timer frame_timer_{};

  std::string url_{};
  std::string org_{};
  std::string data_bucket_{};
  std::string auth_token_{};

  std::vector<InfluxLine> influx_lines_{};

  data::SharedData &shared_data_;
  cfg::Configuration &config_;
};

} // namespace data_middleware

#endif /* INFLUXDB_HPP_ */