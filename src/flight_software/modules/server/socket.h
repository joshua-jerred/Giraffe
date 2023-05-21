#ifndef SOCKET_H_
#define SOCKET_H_

#include <netinet/in.h>

#include <string>

namespace sock {
constexpr int kMaxHostName = 200;
constexpr int kMaxConnections = 5;
constexpr int kMaxReceive = 500;

class TcpSocketServer {
 public:
  TcpSocketServer();
  ~TcpSocketServer();

  bool init(int port_number);

  bool create();
  bool bind(const int port);
  bool listen() const;
  bool accept(sock::TcpSocketServer &sock);

  bool send(const std::string &data) const;
  bool receive(std::string &data) const;

  bool is_valid() const;

 private:
  int sock_ = -1;
  sockaddr_in addr_ = {};
};
}  // namespace sock

#endif