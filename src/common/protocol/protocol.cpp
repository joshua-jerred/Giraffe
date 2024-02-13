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

static const std::unordered_map<protocol::MessageType, std::string>
    typeToStringMap = {{protocol::MessageType::UNKNOWN, "unknown"},
                       {protocol::MessageType::REQ, "req"},
                       {protocol::MessageType::SET, "set"},
                       {protocol::MessageType::RSP, "rsp"}};

static const std::unordered_map<std::string, protocol::MessageType>
    stringToTypeMap = {{"unknown", protocol::MessageType::UNKNOWN},
                       {"req", protocol::MessageType::REQ},
                       {"set", protocol::MessageType::SET},
                       {"rsp", protocol::MessageType::RSP}};

static const std::unordered_map<protocol::ResponseCode, std::string>
    responseCodeToStringMap = {{protocol::ResponseCode::UNKNOWN, "uk"},
                               {protocol::ResponseCode::GOOD, "ok"},
                               {protocol::ResponseCode::ERROR, "er"}};

static const std::unordered_map<std::string, protocol::ResponseCode>
    stringToResponseCodeMap = {{"uk", protocol::ResponseCode::UNKNOWN},
                               {"ok", protocol::ResponseCode::GOOD},
                               {"er", protocol::ResponseCode::ERROR}};

bool protocol::parseMessage(const std::string &json_string,
                            protocol::Message &message) {
  Json message_json;

  try {
    message_json = Json::parse(json_string);
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
    message.typ = protocol::MessageType::UNKNOWN;
    valid = false;
  }
  try {
    message.id = message_json["id"].get<std::string>();
  } catch (const std::exception &e) {
    message.id = "";
    valid = false;
  }

  if (!message_json.contains("bdy")) {
    return false; // return early, everything else is the body.
  } else if (!message_json["bdy"].is_object()) {
    return false; // body must be an object.
  }

  Json &body_json = message_json["bdy"];

  // Type-specific fields.
  if (message.typ == protocol::MessageType::REQ ||
      message.typ == protocol::MessageType::SET) {
    try {
      message.rsc = body_json["rsc"].get<std::string>();
    } catch (const std::exception &e) {
      message.rsc = "";
      valid = false;
    }
  }

  if (message.typ == protocol::MessageType::SET ||
      message.typ == protocol::MessageType::RSP) {
    try {
      if (!body_json["dat"].is_object()) {
        throw std::exception();
      }
      message.dat = body_json["dat"];
    } catch (const std::exception &e) {
      message.dat = Json::object();
      valid = false;
    }
  }

  if (message.typ == protocol::MessageType::RSP) {
    try {
      message.cde =
          stringToResponseCodeMap.at(body_json["cde"].get<std::string>());
    } catch (const std::exception &e) {
      message.cde = protocol::ResponseCode::UNKNOWN;
      valid = false;
    }
  }

  return valid;
}

std::string protocol::Message::getJsonString() {
  return getJson().dump();
}

Json protocol::Message::getJson() {
  Json message = {{"src", endpointToStringMap.at(src)},
                  {"dst", endpointToStringMap.at(dst)},
                  {"typ", typeToStringMap.at(typ)},
                  {"id", id},
                  {"bdy", getBodyJson()}};
  return message;
}

Json protocol::Message::getBodyJson() {
  Json body;
  if (typ == protocol::MessageType::REQ || typ == protocol::MessageType::SET) {
    body["rsc"] = rsc;
  }
  if (typ == protocol::MessageType::SET || typ == protocol::MessageType::RSP) {
    body["dat"] = dat;
    if (rsc != "") { // add rsc to body if it exists. (backwards compatibility)
      body["rsc"] = rsc;
    }
  }
  if (typ == protocol::MessageType::RSP) {
    body["cde"] = responseCodeToStringMap.at(cde);
  }
  return body;
}

void protocol::createRequestMessage(protocol::Message &message,
                                    protocol::Endpoint src,
                                    protocol::Endpoint dst,
                                    protocol::Resource rsc) {
  message.src = src;
  message.dst = dst;
  message.typ = protocol::MessageType::REQ;
  message.id = bst::randomHexString(8);
  message.rsc = rsc;
}

void protocol::createSetMessage(protocol::Message &message,
                                protocol::Endpoint src, protocol::Endpoint dst,
                                protocol::Resource rsc, Json dat) {
  message.src = src;
  message.dst = dst;
  message.typ = protocol::MessageType::SET;
  message.id = bst::randomHexString(8);
  message.rsc = rsc;
  message.dat = dat;
}

void protocol::createResponseMessage(protocol::Message &message,
                                     protocol::Endpoint src,
                                     protocol::Endpoint dst,
                                     protocol::MessageId id,
                                     protocol::ResponseCode cde, Json dat,
                                     std::string rsc) {
  message.src = src;
  message.dst = dst;
  message.typ = protocol::MessageType::RSP;
  message.id = id;
  message.cde = cde;
  message.dat = dat;
  message.rsc = std::move(rsc);
}