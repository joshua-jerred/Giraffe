/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   protocol.cpp
 * @brief  The C++ implementation of the Giraffe Protocol.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-06-10
 * @copyright  2023 (license to be defined)
 */

#include "protocol.hpp"
#include "giraffe_exception.hpp"

#include <BoosterSeat/random.hpp>
#include <array>
#include <unordered_map>

namespace protocol = common::protocol;

/**
 * @brief Map from protocol::Type to std::string.
 */
static const std::unordered_map<protocol::Endpoint, std::string>
    endpointToStringMap = {{protocol::Endpoint::UNKNOWN, "unknown"},
                           {protocol::Endpoint::GFS, "gfs"},
                           {protocol::Endpoint::GGS, "ggs"},
                           {protocol::Endpoint::GDL, "gdl"},
                           {protocol::Endpoint::GWC, "gwc"}};

static const std::unordered_map<std::string, protocol::Endpoint>
    stringToEndpointMap = {{"unknown", protocol::Endpoint::UNKNOWN},
                           {"gfs", protocol::Endpoint::GFS},
                           {"ggs", protocol::Endpoint::GGS},
                           {"gdl", protocol::Endpoint::GDL},
                           {"gwc", protocol::Endpoint::GWC}};

static const std::unordered_map<protocol::Type, std::string> typeToStringMap = {
    {protocol::Type::UNKNOWN, "unknown"},
    {protocol::Type::REQ, "req"},
    {protocol::Type::SET, "set"},
    {protocol::Type::RSP, "rsp"}};

static const std::unordered_map<std::string, protocol::Type> stringToTypeMap = {
    {"unknown", protocol::Type::UNKNOWN},
    {"req", protocol::Type::REQ},
    {"set", protocol::Type::SET},
    {"rsp", protocol::Type::RSP}};

static const std::unordered_map<protocol::ResponseCode, std::string>
    responseCodeToStringMap = {{protocol::ResponseCode::UNKNOWN, "unknown"},
                               {protocol::ResponseCode::OK, "ok"},
                               {protocol::ResponseCode::ERR, "err"}};

static const std::unordered_map<std::string, protocol::ResponseCode>
    stringToResponseCodeMap = {{"unknown", protocol::ResponseCode::UNKNOWN},
                               {"ok", protocol::ResponseCode::OK},
                               {"err", protocol::ResponseCode::ERR}};

bool parseMessage(const std::string &json_string, protocol::Message &message) {
  json message_json;

  try {
    message_json = json::parse(json_string);
  } catch (const std::exception &e) {
    return false;
  }

  bool valid = true;

  // Parse the required fields.
  try {
    message.src =
        stringToEndpointMap.at(message_json["src"].get<std::string>());
  } catch (const std::exception &e) {
    message.src = protocol::Endpoint::UNKNOWN;
    valid = false;
  }
  try {
    message.dst =
        stringToEndpointMap.at(message_json["dst"].get<std::string>());
  } catch (const std::exception &e) {
    message.dst = protocol::Endpoint::UNKNOWN;
    valid = false;
  }
  try {
    message.typ = stringToTypeMap.at(message_json["typ"].get<std::string>());
  } catch (const std::exception &e) {
    message.typ = protocol::Type::UNKNOWN;
    valid = false;
  }
  try {
    message.id = message_json["id"].get<std::string>();
  } catch (const std::exception &e) {
    message.id = "";
    valid = false;
  }

  // Type-specific fields.
  if (message.typ == protocol::Type::REQ ||
      message.typ == protocol::Type::SET) {
    try {
      message.rsc = message_json["rsc"].get<std::string>();
    } catch (const std::exception &e) {
      message.rsc = "";
      valid = false;
    }
  }

  if (message.typ == protocol::Type::SET ||
      message.typ == protocol::Type::RSP) {
    try {
      if (!message_json["dat"].is_object()) {
        throw std::exception();
      }
      message.dat = message_json["dat"];
    } catch (const std::exception &e) {
      message.dat = json::object();
      valid = false;
    }
  }

  if (message.typ == protocol::Type::RSP) {
    try {
      message.rsp =
          stringToResponseCodeMap.at(message_json["rsp"].get<std::string>());
    } catch (const std::exception &e) {
      message.rsp = protocol::ResponseCode::UNKNOWN;
      valid = false;
    }
  }

  return valid;
}

std::string protocol::Message::getJsonString() {
  return getJson().dump();
}

json protocol::Message::getJson() {
  json message = {{"src", endpointToStringMap.at(src)},
                  {"dst", endpointToStringMap.at(dst)},
                  {"typ", typeToStringMap.at(typ)},
                  {"id", id},
                  {"body", getBodyJson()}};
  return message;
}

json protocol::Message::getBodyJson() {
  json body;
  if (typ == protocol::Type::REQ || typ == protocol::Type::SET) {
    body["rsc"] = rsc;
  }
  if (typ == protocol::Type::SET || typ == protocol::Type::RSP) {
    body["dat"] = dat;
  }
  if (typ == protocol::Type::RSP) {
    body["rsp"] = responseCodeToStringMap.at(rsp);
  }
  return body.dump();
}