/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   request_handler.hpp
 * @brief  The request handler class header.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-11-02
 * @copyright  2023 (license to be defined)
 */

#ifndef REQUEST_HANDLER_HPP_
#define REQUEST_HANDLER_HPP_

#include "configuration.hpp"
#include "logger.hpp"
#include "protocol.hpp"
#include "shared_data.hpp"
#include "socket.hpp"

/**
 * @brief Used to handle client requests from the server module.
 * @ingroup ServerModule
 */
class RequestRouter {
public:
  /**
   * @brief Construct a RequestRouter object.
   * @param shared_data - The shared data object.
   * @param config - The global configuration.
   * @param server_stats - The server stats data block.
   */
  RequestRouter(data::SharedData &shared_data, cfg::Configuration &config,
                data::blocks::ServerModuleStats &server_stats);
  ~RequestRouter() = default;

  /**
   * @brief This function is responsible for parsing a request and routing it to
   * the right place.
   *
   * @details The function handles incoming requests from a TCP socket server by
   * parsing them as JSON strings and checking their type and category before
   * sending the appropriate response or an error packet if they are not
   * implemented or malformed.
   *
   * @param client The `client` parameter is a reference to a
   * `sock::TcpSocketServer` object, which represents the client that sent the
   * request to the server.
   * @param request The `request` parameter is a reference to a `std::string`
   * object that contains the json message received from the client.
   *
   */
  void handleMessage(sock::TcpSocketServer &client, std::string &request);

  /**
   * @brief Used to update the rolling average of bytes per second.
   * @todo remove
   */
  void oneSecondTick() {
    bytes_per_second_down_.addValue(bps_down_in_a_second_);
    bytes_per_second_up_.addValue(bps_up_in_a_second_);

    bps_down_in_a_second_ = 0;
    bps_up_in_a_second_ = 0;
  }

  /**
   * @brief Get the average bytes per second down.
   * @return double - The average bytes per second down.
   */
  double getBytesPerSecondDown() const {
    return bytes_per_second_down_.getAverage();
  }

  /**
   * @brief Get the average bytes per second up.
   * @return double - The average bytes per second up.
   */
  double getBytesPerSecondUp() const {
    return bytes_per_second_up_.getAverage();
  }

private:
  /**
   * @brief This function is the generic error response function.
   *
   * @param client The `client` is the client socket.
   * @param error A string containing the error message.
   */
  void sendErrorPacket(sock::TcpSocketServer &client,
                       const std::string &error = "");

  /**
   * @brief This function responds to a ping request.
   * @param client
   * @param msg
   */
  void handlePingRequest(sock::TcpSocketServer &client, protocol::Message &msg);

  /**
   * @brief This function responds to a setting request.
   * @details It will respond with the configuration portion requested
   * if it exists. Otherwise it will respond with an error.
   * @param client - The client socket.
   * @param msg - The received message.
   */
  void handleSettingRequest(sock::TcpSocketServer &client,
                            protocol::Message &msg);

  /**
   * @brief This function handles a setting SET request. It will attempt to set
   * the configuration with the value provided.
   * @param client - The client socket.
   * @param msg - The received message.
   */
  void handleSettingSet(sock::TcpSocketServer &client, protocol::Message &msg);

  /**
   * @brief This function responds to data requests.
   *
   * @param client
   * @param msg
   */
  void handleDataRequest(sock::TcpSocketServer &client, protocol::Message &msg);

  /**
   * @brief This function parses a command request, sends it to the command
   * queue and sends a simple response back. It does not wait for the command to
   * be executed.
   * @param client - The client socket.
   * @param msg - The message to parse.
   */
  void handleCommandRequest(sock::TcpSocketServer &client,
                            protocol::Message &msg);

  /**
   * @brief Response to an image request. Sends the image data to the client.
   *
   * @param client - The client socket.
   * @param msg_rsc - The message resource. (Contains img/<image_name>)
   */
  void handleImageRequest(sock::TcpSocketServer &client,
                          const std::string &msg_rsc);

  /**
   * @brief Send a message to the client.
   * @param response - The message to send.
   * @param client - The client socket.
   */
  void sendMessage(protocol::Message &response, sock::TcpSocketServer &client);

  data::SharedData &shared_data_;
  cfg::Configuration &config_;
  data::blocks::ServerModuleStats &stats_;
  giraffe::Logger logger_;

  double bps_down_in_a_second_ = 0;
  double bps_up_in_a_second_ = 0;
  bst::RollingAverage bytes_per_second_down_ = {60};
  bst::RollingAverage bytes_per_second_up_ = {60};
};

#endif