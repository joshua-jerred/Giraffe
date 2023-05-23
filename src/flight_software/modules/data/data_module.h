#ifndef DATA_MODULE_H_
#define DATA_MODULE_H_

#include <iostream>

#include "module.h"

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

  template <class PKT>
  void parseStream(data::Stream<PKT> &stream,
                   std::function<void(PKT &)> callback);

  void parseStreams();
};

}  // namespace modules

#endif