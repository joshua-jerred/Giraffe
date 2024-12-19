/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   socket.hpp
 * @brief  The socket class header.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-11-02
 * @copyright  2023 (license to be defined)
 */

#ifndef SOCKET_HPP_
#define SOCKET_HPP_

#include <string>

#include <netinet/in.h>

#include <BoosterSeat/rolling_average.hpp>

#include "optional_logger.hpp"

namespace sock {
constexpr int K_MAX_HOST_NAME_SIZE = 200;
constexpr int K_MAX_CONNECTIONS = 5;
constexpr int K_MAX_RECEIVE = 500;

/**
 * @brief The TCP socket server class.
 * @ingroup ServerModule
 */
class TcpSocketServer {
public:
  TcpSocketServer();
  ~TcpSocketServer();

  /// @brief Create, bind, and listen on a port.
  /// @param port_number - The port number to listen on.
  /// @return \c true if the socket was created, bound, and listening, \c false
  /// if denied.
  bool init(int port_number);

  bool create();
  bool bind(const int port);
  bool listen() const;
  bool accept(sock::TcpSocketServer &sock);
  bool close();

  bool send(const std::string &data) const;
  bool sendRawData(const uint8_t *data, size_t size);
  bool receive(std::string &data) const;

  bool isValid() const;

  std::string getClientAddress() const;

private:
  int sock_ = -1;
  sockaddr_in addr_ = {};
};

class TcpSocketClient : public giraffe::OptionalLogger {
public:
  TcpSocketClient() = default;

  TcpSocketClient(giraffe::ILogger &logger) {
    setLogger(logger);
  }

  ~TcpSocketClient() = default;

  bool connect(std::string sever_address, uint16_t port);

  bool transaction(const std::string &data, std::string &response);

  bool disconnect();

private:
  bool send(const std::string &data);
  bool receive(std::string &data);

  int sock_ = -1;

  bool connected_ = false;
};

} // namespace sock

#endif // SOCKET_HPP_