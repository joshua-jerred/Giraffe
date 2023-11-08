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

#include <netinet/in.h>

#include <BoosterSeat/rolling_average.hpp>
#include <string>

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

  bool init(int port_number);

  bool create();
  bool bind(const int port);
  bool listen() const;
  bool accept(sock::TcpSocketServer &sock);
  bool close();

  bool send(const std::string &data) const;
  bool receive(std::string &data) const;

  bool isValid() const;

private:
  int sock_ = -1;
  sockaddr_in addr_ = {};
};
} // namespace sock

#endif // SOCKET_HPP_