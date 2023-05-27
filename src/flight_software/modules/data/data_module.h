#ifndef DATA_MODULE_H_
#define DATA_MODULE_H_

#include <iostream>

#include "module.hpp"

namespace modules {

class DataModule : public Module {
 public:
  DataModule(data::SharedData &, cfg::Configuration &);
  ~DataModule() override;

 private:
  void startup() override;
  void loop() override;
  void shutdown() override;
  void processCommand(const command::Command &);

  void parseDataStream();
  void parseGeneralDataPacket(const data::DataPacket &packet);
  void parseStatusDataPacket(const data::DataPacket &packet);

  void parseLogStream();
};

}  // namespace modules

#endif