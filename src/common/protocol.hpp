/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   protocol.h
 * @brief  Declares the protocol namespace and its associated types.
 * @details This is the C++ implementation of the Giraffe Protocol.
 * @see protocol.md
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-06-09
 * @copyright  2023 (license to be defined)
 */

#ifndef PROTOCOL_HPP_
#define PROTOCOL_HPP_

#include "json.hpp"
#include <string>

namespace protocol {
/**
 * @brief Endpoint enum, for the source and destination of a message.
 */
enum class Endpoint {
  UNKNOWN, // Error Endpoint
  GFS,     // Giraffe Flight System
  GGS,     // Giraffe Ground Station
  GDL,     // Giraffe Data Link
  GWC      // Giraffe Web Client
};

/**
 * @brief Type enum, for the type of message.
 */
enum class MessageType {
  UNKNOWN, // Error Type
  REQ,     // Request
  SET,     // Set
  RSP      // Response
};

enum class ResponseCode {
  UNKNOWN, // Error Response Code
  GOOD,    // OK
  ERROR    // Error
};

/**
 * @brief MessageId type, for the id of a message.
 */
typedef std::string MessageId;
typedef std::string Resource;

/**
 * @brief Message struct, for the Giraffe Protocol Message.
 */
struct Message {
  /**
   * @brief Get the message as a JSON string.
   * @return std::string - The message as a JSON string.
   */
  std::string getJsonString();

  /**
   * @brief Get the message as a JSON object.
   * @return json - The message as a JSON object.
   */
  json getJson();

  /**
   * @brief Get the message body as a JSON object.
   * @return json
   */
  json getBodyJson();

  // Required Fields
  protocol::Endpoint src = protocol::Endpoint::UNKNOWN;
  protocol::Endpoint dst = protocol::Endpoint::UNKNOWN;
  protocol::MessageType typ = protocol::MessageType::UNKNOWN;
  MessageId id = "";

  // Type Specific Fields
  std::string rsc = "";      // Resource identification, for REQ and SET
  json dat = json::object(); // Data, for SET and RSP
  protocol::ResponseCode rsp =
      protocol::ResponseCode::UNKNOWN; // Response Code, for RSP
};

/**
 * @brief Parse a JSON string into a Message.
 * @param json_string - The JSON string to parse.
 * @param message - The message object to parse into.
 * @return true - If the message was parsed successfully.
 * @return false - If the message was not parsed successfully.
 */
bool parseMessage(const std::string &json_string, Message &message);

/**
 * @brief Create a Request Message.
 *
 * @param src - The source of the message.
 * @param dst - The destination of the message.
 * @param rsc - The resource to request.
 * @return true - If the message is valid.
 * @return false - If the message is not valid.
 */
void createRequestMessage(Message &message, protocol::Endpoint src,
                          protocol::Endpoint dst, Resource rsc);

/**
 * @brief Create a Set Message.
 *
 * @param src - The source of the message.
 * @param dst - The destination of the message.
 * @param rsc - The resource to set.
 * @param dat - The data to set the resource to.
 * @return true - If the message is valid.
 * @return false - If the message is not valid.
 */
void createSetMessage(Message &message, protocol::Endpoint src,
                      protocol::Endpoint dst, Resource rsc, json dat);

/**
 * @brief Create a Response Message.
 *
 * @param src - The source of the message.
 * @param dst - The destination of the message.
 * @param id - The id of the message to respond to.
 * @param dat - The data to respond with.
 * @param rsp - The response code.
 * @return true - If the message is valid.
 * @return false - If the message is not valid.
 */
void createResponseMessage(Message &message, protocol::Endpoint src,
                           protocol::Endpoint dst, MessageId id,
                           protocol::ResponseCode rsp, json dat);

} // namespace protocol

#endif // PROTOCOL_HPP_