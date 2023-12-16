/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   server_module.hpp
 * @brief  Header file for the server module.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-06-30
 * @copyright  2023 (license to be defined)
 */

/**
 * @defgroup ServerModule Server Module
 */

#ifndef SERVER_MODULE_HPP_
#define SERVER_MODULE_HPP_

#include "module.hpp"
#include "request_handler.hpp"
#include "socket.hpp"
#include <BoosterSeat/rolling_average.hpp>
#include <BoosterSeat/timer.hpp>

namespace modules {

/**
 * @brief The server module. Provides a network interface for interacting with
 * the flight system.
 * @ingroup ServerModule
 */
class ServerModule : public Module {
public:
  ServerModule(data::SharedData &, cfg::Configuration &);
  ~ServerModule() override = default;

private:
  /**
   * @brief Startup the server module.
   * @details Reads the socket port from the configuration and initializes the
   * socket with it. Override of Module::startup()
   */
  void startup() override;

  /**
   * @brief Loop module function for the server module.
   * @details Connects to clients and handles the requests. Override of
   * Module::loop()
   */
  void loop() override;

  /**
   * @brief Shutdown the server module.
   * @details Closes the socket. Override of Module::shutdown()
   */
  void shutdown() override;

  /**
   * @brief Process a command that was received from the flight runner.
   * @param command The command to process.
   */
  void processCommand(const cmd::Command &command);

  /**
   * @brief Used to handle the requests from the client.
   */
  RequestRouter request_router_;

  /**
   * @brief The TCP socket used to communicate with the client.
   */
  sock::TcpSocketServer server_socket_ = {};

  /**
   * @brief Local copy of the server module stats. This is used to update the
   * stats every loop.
   */
  data::blocks::ServerModuleStats stats_ = {};

  /**
   * @brief Timer for the rolling average.
   * Used to call 'server_socket_.oneSecondTick()' every second.
   */
  bst::Timer rolling_average_timer_ = {1000};

  /**
   * @brief Used to update the 'connected' status of the server.
   */
  bst::Timer connected_timeout_ = {2000};
};

} // namespace modules

#endif // SERVER_MODULE_HPP_