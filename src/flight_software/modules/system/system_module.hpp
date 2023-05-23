#ifndef SYSTEM_MODULE_HPP_
#define SYSTEM_MODULE_HPP_

#include <iostream>

#include "module.h"

namespace modules {

class SystemModule : public Module {
 public:
  SystemModule(data::SharedData &, cfg::Configuration &);
  ~SystemModule() override;

 private:
  void startup() override;
  void loop() override;
  void shutdown() override;
  void processCommand(const command::Command &);

  void updateMiscInfo();
  void updateCpuAndMemoryInfo();
  void updateCpuTemp();
  void updateDiskInfo();
  void updateNetworkInfo();

  data::SysInfoPacket data_ = data::SysInfoPacket();
};

}  // namespace modules

#endif