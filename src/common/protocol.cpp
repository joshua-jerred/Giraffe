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

protocol::Id protocol::generateId() {
  return "tmpid";
}

inline std::string endpointToString(protocol::Endpoint endpoint) {
  switch (endpoint) {
    case protocol::Endpoint::UNKNOWN:
      return "unknown";
    case protocol::Endpoint::CLIENT:
      return "client";
    case protocol::Endpoint::GFS:
      return "gfs";
    case protocol::Endpoint::GGS:
      return "ggs";
    case protocol::Endpoint::TELEMETRY:
      return "telemetry";
  }

  throw protocol::ProtocolException("Invalid endpoint.");
}

inline protocol::Endpoint stringToEndpoint(std::string endpoint_string) {
  if (endpoint_string == "client") {
    return protocol::Endpoint::CLIENT;
  } else if (endpoint_string == "gfs") {
    return protocol::Endpoint::GFS;
  } else if (endpoint_string == "ggs") {
    return protocol::Endpoint::GGS;
  } else if (endpoint_string == "telemetry") {
    return protocol::Endpoint::TELEMETRY;
  } else {
    throw protocol::ProtocolException("Invalid endpoint string.");
  }
}

inline std::string typeToString(protocol::Type type) {
  switch (type) {
    case protocol::Type::INFO:
      return "info";
    case protocol::Type::REQUEST:
      return "req";
    case protocol::Type::RESPONSE:
      return "rsp";
    case protocol::Type::SET:
      return "set";
    case protocol::Type::UNKNOWN:
      return "unknown";
  }
  throw protocol::ProtocolException("Invalid type.");
}

inline protocol::Type stringToType(std::string type_string) {
  if (type_string == "info") {
    return protocol::Type::INFO;
  } else if (type_string == "req") {
    return protocol::Type::REQUEST;
  } else if (type_string == "rsp") {
    return protocol::Type::RESPONSE;
  } else if (type_string == "set") {
    return protocol::Type::SET;
  } else {
    throw protocol::ProtocolException("Invalid type string.");
  }
}

inline std::string categoryToString(protocol::Category category) {
  switch (category) {
    case protocol::Category::PING:
      return "ping";
    case protocol::Category::DATA:
      return "data";
    case protocol::Category::SETTING:
      return "setting";
    case protocol::Category::ERROR:
      return "error";
    case protocol::Category::UNKNOWN:
      return "unknown";
  }
  throw protocol::ProtocolException("Invalid category.");
}

inline protocol::Category stringToCategory(std::string category_string) {
  if (category_string == "ping") {
    return protocol::Category::PING;
  } else if (category_string == "data") {
    return protocol::Category::DATA;
  } else if (category_string == "setting") {
    return protocol::Category::SETTING;
  } else {
    throw protocol::ProtocolException("Invalid category string.");
  }
}

protocol::Message::Message(const protocol::Endpoint src,
                           const protocol::Endpoint dst,
                           const protocol::Type typ,
                           const protocol::Category cat,
                           const protocol::Id ident, const protocol::Body body)
    : src_(src), dst_(dst), typ_(typ), cat_(cat), id_(ident), body_(body) {
}

protocol::Message::Message(const std::string& json_string) {
  json message_json;

  try {
    message_json = json::parse(json_string);
  } catch (const std::exception& e) {
    throw protocol::ProtocolException("Could not parse message string as JSON");
  }

  if (!message_json.contains("src"))
    throw protocol::ProtocolException("Missing src field.");

  if (!message_json.contains("dst"))
    throw protocol::ProtocolException("Missing dst field.");

  if (!message_json.contains("typ"))
    throw protocol::ProtocolException("Missing typ field.");

  if (!message_json.contains("cat"))
    throw protocol::ProtocolException("Missing cat field.");

  if (!message_json.contains("id"))
    throw protocol::ProtocolException("Missing id field.");

  if (!message_json.contains("body"))
    throw protocol::ProtocolException("Missing body field");

  src_ = stringToEndpoint(message_json["src"].get<std::string>());
  dst_ = stringToEndpoint(message_json["dst"].get<std::string>());
  typ_ = stringToType(message_json["typ"].get<std::string>());
  cat_ = stringToCategory(message_json["cat"].get<std::string>());
  id_ = message_json["id"].get<std::string>();
  body_ = message_json["body"].get<std::string>();
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