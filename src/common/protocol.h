/**
 * @file protocol.h
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief Header for the Giraffe Protocol (C++ Implementation)
 * @date 2023-05-15
 * @copyright Copyright (c) 2023
 */

#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#include <stdexcept>
#include <string>

namespace protocol {
enum class Endpoint { UNKNOWN, GFS, GGS, CLIENT, TELEMETRY };
enum class Type { UNKNOWN, INFO, REQUEST, SET, RESPONSE };
enum class Category { UNKNOWN, PING, DATA, SETTING, ERROR };
typedef std::string Id;
typedef std::string Body;

protocol::Id generateId();

struct Message {
  Message(const protocol::Endpoint src, const protocol::Endpoint dst,
          const protocol::Type typ, const protocol::Category cat,
          const protocol::Id id, const protocol::Body body);
  Message(const std::string &json_string);

  std::string getMessageString();

  protocol::Endpoint src_ = protocol::Endpoint::UNKNOWN;
  protocol::Endpoint dst_ = protocol::Endpoint::UNKNOWN;
  protocol::Type typ_ = protocol::Type::UNKNOWN;
  protocol::Category cat_ = protocol::Category::UNKNOWN;
  protocol::Id id_ = "";
  protocol::Body body_ = "";
};

class ProtocolException : public std::runtime_error {
 public:
  ProtocolException(const std::string& message) : std::runtime_error(message) {
  }
};



}  // namespace protocol

#endif