#ifndef SOCKET_HPP_
#define SOCKET_HPP_

#include <netinet/in.h>

#include <BoosterSeat/rolling_average.hpp>
#include <string>

namespace sock {
constexpr int K_MAX_HOST_NAME_SIZE = 200;
constexpr int K_MAX_CONNECTIONS = 5;
constexpr int K_MAX_RECEIVE = 500;

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

#endif