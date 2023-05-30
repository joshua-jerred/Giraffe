#include "socket.hpp"

#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream> /** @todo remove this */

sock::TcpSocketServer::TcpSocketServer() {
  memset(&addr_, 0, sizeof(addr_));
}

sock::TcpSocketServer::~TcpSocketServer() {
  if (is_valid()) ::close(sock_);
}

bool sock::TcpSocketServer::init(int port_number) {
  if (!create()) {
    std::cout << "Failed to create" << std::endl;
    return false;
  }

  // Non-Blocking
  constexpr bool non_blocking = true;
  int opts;
  opts = fcntl(sock_, F_GETFL);
  if (opts >= 0) {
    if (non_blocking)
      opts = (opts | O_NONBLOCK);
    else
      opts = (opts & ~O_NONBLOCK);
    fcntl(sock_, F_SETFL, opts);
  }

  if (!bind(port_number)) {
    std::cout << "Failed to bind" << std::endl;
    return false;
  }

  if (!listen()) {
    std::cout << "Failed to listen" << std::endl;
    return false;
  }

  return true;
}

bool sock::TcpSocketServer::create() {
  sock_ = socket(AF_INET, SOCK_STREAM, 0);
  if (!is_valid()) return false;

  int on = 1;
  if (setsockopt(sock_, SOL_SOCKET, SO_REUSEADDR, (const char *)&on,
                 sizeof(on)) == -1) {
    return false;
  }

  return true;
}

bool sock::TcpSocketServer::bind(const int port) {
  if (!is_valid()) {
    return false;
  }

  addr_.sin_family = AF_INET;
  addr_.sin_addr.s_addr = INADDR_ANY;
  addr_.sin_port = htons(port);

  int bind_status = ::bind(sock_, (struct sockaddr *)&addr_, sizeof(addr_));
  if (bind_status == -1) {
    return false;
  }
  return true;
}

bool sock::TcpSocketServer::listen() const {
  if (!is_valid()) {
    return false;
  }

  int listen_status = ::listen(sock_, sock::kMaxConnections);
  if (listen_status == -1) {
    return false;
  }
  return true;
}

bool sock::TcpSocketServer::accept(sock::TcpSocketServer &new_socket) {
  int address_length = sizeof(addr_);
  new_socket.sock_ =
      ::accept(sock_, (sockaddr *)&addr_, (socklen_t *)&address_length);

  if (new_socket.sock_ <= 0) {
    return false;
  }
  return true;
}

bool sock::TcpSocketServer::send(const std::string &data) const {
  int status = ::send(sock_, data.c_str(), data.size(), MSG_NOSIGNAL);
  if (status == -1) {
    return false;
  } else {
    return true;
  }
}

bool sock::TcpSocketServer::receive(std::string &data) const {
  char buf[sock::kMaxReceive + 1];

  data = "";

  memset(buf, 0, sock::kMaxReceive + 1);

  int status = ::recv(sock_, buf, sock::kMaxReceive, 0);

  if (status == -1) {
    return 0;
  } else if (status == 0) {
    return 0;
  } else {
    data = buf;
    return status;
  }
}

bool sock::TcpSocketServer::is_valid() const {
  return sock_ != -1;
}