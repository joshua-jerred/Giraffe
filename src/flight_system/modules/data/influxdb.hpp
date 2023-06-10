#ifndef INFLUXDB_HPP_
#define INFLUXDB_HPP_

#include "configuration.hpp"
#include "shared_data.hpp"

namespace data_middleware {

class InfluxDb {
public:
  InfluxDb(data::SharedData &shared_data, cfg::Configuration &config)
      : shared_data_(shared_data), config_(config){};
  ~InfluxDb() = default;

  void logDataPacket(const data::DataPacket &packet);

  void logLogPacket(const data::LogPacket &packet);

private:
  data::SharedData &shared_data_;
  cfg::Configuration &config_;
};

}; // namespace data_middleware

#endif /* INFLUXDB_HPP_ */