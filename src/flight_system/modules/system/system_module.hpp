#ifndef SYSTEM_MODULE_HPP_
#define SYSTEM_MODULE_HPP_

#include <iostream>

#include "module.hpp"

namespace modules {

class SystemModule : public Module {
public:
  SystemModule(data::SharedData &, cfg::Configuration &);
  ~SystemModule() override;

private:
  void startup() override;
  void loop() override;
  void shutdown() override;
  void processCommand(const cmd::Command &);

  void updateCpuAndMemoryInfo();
  void updateCpuTemp();
  void updateDiskInfo();
  void updateNetworkInfo();

  data::blocks::SystemInfo data_ = data::blocks::SystemInfo();
};

} // namespace modules

#endif