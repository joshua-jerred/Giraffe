/**
 * @file protocol.cpp
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief Implementation for the Giraffe Protocol C++ Implementation
 * @date 2023-05-15
 * @copyright Copyright (c) 2023
 */

#include "protocol.h"

#include <nlohmann/json.hpp>
#include <unordered_map>

using json = nlohmann::ordered_json;

protocol::Message::Message(const protocol::Endpoint src,
                           const protocol::Endpoint dst,
                           const protocol::Type typ,
                           const protocol::Category cat,
                           const protocol::Id ident, const protocol::Body body)
    : src_(src), dst_(dst), typ_(typ), cat_(cat), id_(ident), body_(body) {
}

inline std::string endpointToString(const protocol::Endpoint endpoint) {
  switch (endpoint) {
    case protocol::Endpoint::GFS:
      return "gfs";
    case protocol::Endpoint::GGS:
      return "ggs";
    case protocol::Endpoint::CLIENT:
      return "client";
    case protocol::Endpoint::TELEMETRY:
      return "telemetry";
  }

  return "";
}

inline std::string typeToString(const protocol::Type type) {
  switch (type) {
    case protocol::Type::INFO:
      return "info";
    case protocol::Type::REQUEST:
      return "req";
    case protocol::Type::RESPONSE:
      return "rsp";
    case protocol::Type::SET:
      return "set";
  }

  return "";
}

inline std::string categoryToString(const protocol::Category category) {
  switch (category) {
    case protocol::Category::PING:
      return "ping";
    case protocol::Category::DATA:
      return "data";
    case protocol::Category::SETTING:
      return "settings";
  }

  return "";
}

std::string protocol::Message::getMessageString() {
  json message = {{"src", endpointToString(src_)},
                  {"dst", endpointToString(dst_)},
                  {"typ", typeToString(typ_)},
                  {"cat", categoryToString(cat_)},
                  {"id", id_},
                  {"body", body_}};
  return message.dump();
}