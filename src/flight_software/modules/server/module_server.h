/**
 * @file module_server.h
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief
 * @date 2023-05-16
 * @copyright Copyright (c) 2023
 */

#ifndef MODULE_SERVER_H_
#define MODULE_SERVER_H_

#include "module.h"

class ServerModule : public modules::Module {
 public:
  ServerModule(data::Streams &streams);

 private:
  void startup() override;
  void loop() override;
  void shutdown() override;
  void processCommand(const command::Command &command) override;
}

#endif