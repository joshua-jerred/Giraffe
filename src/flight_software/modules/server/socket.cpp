#include "socket.h"

#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <string.h>

#include <iostream> /** @todo remove this */

sock::TcpSocketServer::TcpSocketServer() {
  memset(&addr_, 0, sizeof(addr_));
}

sock::TcpSocketServer::~TcpSocketServer() {
}

bool sock::TcpSocketServer::create() {
  sock_ = socket(AF_INET, SOCK_STREAM, 0);
  return is_valid();

}

bool sock::TcpSocketServer::bind(const int port) {
}

bool sock::TcpSocketServer::listen() const {
}

bool sock::TcpSocketServer::accept(sock::TcpSocketServer &sock) {
}

bool sock::TcpSocketServer::send(const std::string &data) const {
}

bool sock::TcpSocketServer::receive(std::string &data) const {
}

bool sock::TcpSocketServer::is_valid() const {
  return sock_ != -1;
}