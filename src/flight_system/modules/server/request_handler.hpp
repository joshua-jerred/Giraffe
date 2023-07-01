#ifndef REQUEST_HANDLER_HPP_
#define REQUEST_HANDLER_HPP_

#include "configuration.hpp"
#include "protocol.hpp"
#include "shared_data.hpp"
#include "socket.hpp"

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

  void oneSecondTick() {
    bytes_per_second_down_.intervalCall();
    bytes_per_second_up_.intervalCall();
  }

  double getBytesPerSecondDown() const {
    return bytes_per_second_down_.getAverage();
  }

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
   *
   * @param client
   * @param msg
   */
  void handleSettingRequest(sock::TcpSocketServer &client,
                            protocol::Message &msg);

  /**
   * @brief This function responds to data requests.
   *
   * @param client
   * @param msg
   */
  void handleDataRequest(sock::TcpSocketServer &client, protocol::Message &msg);

  void sendMessage(protocol::Message &response, sock::TcpSocketServer &client);

  data::SharedData &shared_data_;
  cfg::Configuration &config_;
  data::blocks::ServerModuleStats &stats_;

  BoosterSeat::RollingAverage bytes_per_second_down_ = {60};
  BoosterSeat::RollingAverage bytes_per_second_up_ = {60};
};

#endif