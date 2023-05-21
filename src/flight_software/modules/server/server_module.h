/**
 * @file module_server.h
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief
 * @date 2023-05-16
 * @copyright Copyright (c) 2023
 */

#ifndef SERVER_MODULE_H_
#define SERVER_MODULE_H_

#include "module.h"
#include "streams.h"

namespace modules {

class ServerModule : public Module {
 public:
  ServerModule(data::Streams &streams, cfg::Configuration &config);
  ~ServerModule() override;

 private:
  void startup() override;
  void loop() override;
  void shutdown() override;
  void processCommand(const command::Command &command);
};

}  // namespace modules

#endif