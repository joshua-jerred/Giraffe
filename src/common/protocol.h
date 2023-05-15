/**
 * @file protocol.h
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief Header for the Giraffe Protocol (C++ Implementation)
 * @date 2023-05-15
 * @copyright Copyright (c) 2023
 */

#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#include <string>

namespace protocol {
enum class Endpoint { GFS, GGS, CLIENT, TELEMETRY };
enum class Type { INFO, REQUEST, SET, RESPONSE };
enum class Category { PING, DATA, SETTING };
typedef std::string Id;
typedef std::string Body;

struct Message {
  Message(const protocol::Endpoint src, const protocol::Endpoint dst,
          const protocol::Type typ, const protocol::Category cat,
          const protocol::Id id, const protocol::Body body);

  std::string getMessageString();

  protocol::Endpoint src_;
  protocol::Endpoint dst_;
  protocol::Type typ_;
  protocol::Category cat_;
  protocol::Id id_;
  protocol::Body body_;
};

protocol::Message parseMessageString(std::string message_string);

}  // namespace protocol

#endif