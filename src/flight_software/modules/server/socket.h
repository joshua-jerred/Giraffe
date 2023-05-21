#ifndef SOCKET_H_
#define SOCKET_H_

#include <netinet/in.h>

#include <string>

namespace sock {
class TcpSocketServer {
 public:
  TcpSocketServer();
  ~TcpSocketServer();

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