/**
 * @file module_server.h
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief
 * @date 2023-05-16
 * @copyright Copyright (c) 2023
 */

#ifndef SERVER_MODULE_H_
#define SERVER_MODULE_H_

#include "module.hpp"
#include "request_handler.hpp"
#include "socket.hpp"
#include <BoosterSeat/rolling_average.hpp>
#include <BoosterSeat/timer.hpp>

namespace modules {

class ServerModule : public Module {
public:
  ServerModule(data::SharedData &, cfg::Configuration &);
  ~ServerModule() override = default;

private:
  void startup() override;
  void loop() override;
  void shutdown() override;
  void processCommand(const command::Command &);

  RequestRouter request_router_;
  sock::TcpSocketServer server_socket_ = {};
  data::blocks::ServerModuleStats stats_ = {};

  /**
   * @brief Timer for the rolling average.
   * Used to call 'server_socket_.oneSecondTick()' every second.
   */
  BoosterSeat::Timer rolling_average_timer_ = {1000};

  /**
   * @brief Used to update the 'connected' status of the server.
   */
  BoosterSeat::Timer connected_timeout_ = {2000};
};

} // namespace modules

#endif