#ifndef DATA_MODULE_H_
#define DATA_MODULE_H_

#include <iostream>

#include "module.h"
#include "streams.h"

namespace modules {

static MetaData metadata("data_module", node::Identification::DATA_MODULE);

class DataModule : public Module {
 public:
  DataModule(data::Streams &streams, cfg::Configuration &config);
  ~DataModule() override;

 private:
  void startup() override;
  void loop() override;
  void shutdown() override;
  void processCommand(const command::Command &command);
};

}  // namespace modules

#endif