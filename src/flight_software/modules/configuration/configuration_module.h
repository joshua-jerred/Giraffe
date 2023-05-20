#ifndef CONFIGURATION_MODULE_H_
#define CONFIGURATION_MODULE_H_

#include <iostream>

#include "module.h"
#include "streams.h"

namespace modules {

class ConfigurationModule : public Module {
 public:
  ConfigurationModule(data::Streams &streams);

 private:
  void startup() override;
  void shutdown() override;
  void loop() override;
  void processCommand(const command::Command &command) override;
};

}  // namespace modules

#endif