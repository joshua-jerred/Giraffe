#ifndef DATA_MODULE_H_
#define DATA_MODULE_H_

#include <iostream>

#include "data_log.hpp"
#include "influxdb.hpp"
#include "module.hpp"

namespace modules {

class DataModule : public Module {
public:
  DataModule(data::SharedData &, cfg::Configuration &);
  ~DataModule() override = default;

private:
  void startup() override;
  void loop() override;
  void shutdown() override;
  void processCommand(const cmd::Command &);

  void parseDataStream();
  void parseGeneralDataPacket(const data::DataPacket &packet);
  void parseStatusDataPacket(const data::DataPacket &packet);

  void parseLogStream();

  data_middleware::DataLog data_log_;
  data_middleware::InfluxDb influxdb_;

  bool data_file_enabled_ = false;
  bool log_file_enabled_ = false;
  bool influxdb_enabled_ = false;

  cfg::gEnum::LogStrategy data_file_logging_strategy_ =
      cfg::gEnum::LogStrategy::INTERVAL;
};

} // namespace modules

#endif