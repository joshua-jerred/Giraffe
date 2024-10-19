/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   gdl_message.hpp
 * @brief  Message and Message Queue for Giraffe Data Link
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-01-18
 * @copyright  2024 (license to be defined)
 */

#ifndef GDL_MESSAGE_HPP_
#define GDL_MESSAGE_HPP_

#include <cstdint>
#include <iomanip>
#include <mutex>
#include <queue>
#include <sstream>
#include <string>
#include <variant>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include <SignalEasel/aprs/packets.hpp>

#include <giraffe_assert.hpp>

#include <gdl_constants.hpp>

namespace giraffe::gdl {

/// @brief A message that can be sent via GDL
class Message {
public:
  /// @brief The type of message
  enum class Type : uint8_t { BROADCAST, EXCHANGE, LOCATION, IMAGE, TELEMETRY };

  struct Location {
    double latitude = 0;
    double longitude = 0;
    uint32_t altitude = 0;
    double speed = 0;
    int heading = 0;
    std::string time_code{};
  };

  struct Image {
    std::string path{};
  };

  struct AprsTelemetry {
    using TelemType = signal_easel::aprs::Packet::Type;
    using TelemetryData = signal_easel::aprs::telemetry::TelemetryData;

    TelemType telemetry_type = TelemType::UNKNOWN;
    TelemetryData telemetry_data{};
  };

  Message() = default;

  bool setBroadcastMessage(std::string broadcast_message, uint32_t identifier) {
    if (broadcast_message.size() > GDL_MESSAGE_DATA_MAX_SIZE) {
      return false;
    }

    identifier_ = identifier;
    contents_ = std::move(broadcast_message);
    type_ = Type::BROADCAST;
    return true;
  }

  bool setExchangeMessage(std::string exchange_message, uint32_t identifier) {
    if (exchange_message.size() > GDL_MESSAGE_DATA_MAX_SIZE) {
      return false;
    }

    identifier_ = identifier;
    contents_ = std::move(exchange_message);
    type_ = Type::EXCHANGE;
    return true;
  }

  bool setLocationMessage(Location location, uint32_t identifier) {
    identifier_ = identifier;
    contents_ = location;
    type_ = Type::LOCATION;
    return true;
  }

  void setImageMessage(Image image_info, uint32_t identifier) {
    identifier_ = identifier;
    contents_ = image_info;
    type_ = Type::IMAGE;
  }

  void setTelemetryMessage(AprsTelemetry telemetry, uint32_t identifier) {
    identifier_ = identifier;
    contents_ = telemetry;
    type_ = Type::TELEMETRY;
  }

  uint32_t getIdentifier() const {
    return identifier_;
  }

  void setIdentifier(uint32_t identifier) {
    identifier_ = identifier;
  }

  bool setIdentifierFromHex(const std::string &identifier) {
    // convert the hex string to an integer
    try {
      identifier_ = std::stoul(identifier, nullptr, 16);
    } catch (...) {
      return false;
    }
    return true;
  }

  std::string getIdentifierString() const {
    // return the identifier as a hex string
    std::stringstream str_s;
    str_s << std::setfill('0') << std::setw(8) << std::hex << identifier_;
    return str_s.str();
  }

  Type getType() const {
    return type_;
  }

  void setType(Type type) {
    type_ = type;
  }

  void setData(std::string data) {
    contents_ = std::move(data);
  }

  std::string getData() const {
    return std::get<std::string>(contents_);
  }

  void setLocation(Location location) {
    contents_ = location;
  }

  void setTelemetry(AprsTelemetry telemetry) {
    contents_ = telemetry;
  }

  Location getLocation() const {
    return std::get<Location>(contents_);
  }

  Image getImage() const {
    return std::get<Image>(contents_);
  }

  AprsTelemetry getTelemetry() const {
    return std::get<AprsTelemetry>(contents_);
  }

  json getJson() const {
    json data = {{"identifier", getIdentifierString()}};
    switch (getType()) {
    case Type::BROADCAST: {
      data["type"] = "BROADCAST";
      data["data"] = getData();
    } break;
    case Type::EXCHANGE: {
      data["type"] = "EXCHANGE";
      data["data"] = getData();
    } break;
    case Type::LOCATION: {
      data["type"] = "LOCATION";
      data["data"] = {{"latitude", getLocation().latitude},
                      {"longitude", getLocation().longitude},
                      {"altitude", getLocation().altitude},
                      {"speed", getLocation().speed},
                      {"heading", getLocation().heading},
                      {"time_code", getLocation().time_code}};
    } break;
    case Type::IMAGE: {
      data["type"] = "IMAGE";
      data["data"] = {{"path", getImage().path}};
    } break;
    case Type::TELEMETRY: {
      auto telem_message = getTelemetry();
      auto &telem = telem_message.telemetry_data;
      switch (telem_message.telemetry_type) {
      case AprsTelemetry::TelemType::TELEMETRY_DATA_REPORT: {
        using ParId = signal_easel::aprs::telemetry::Parameter::Id;
        data["type"] = "TELEMETRY_DATA_REPORT";
        data["data"] = {{
                            "sequence_number",
                            telem.getSequenceNumber(),
                        },
                        {"a1", telem.getAnalog(ParId::A1).getValue()},
                        {"a2", telem.getAnalog(ParId::A2).getValue()},
                        {"a3", telem.getAnalog(ParId::A3).getValue()},
                        {"a4", telem.getAnalog(ParId::A4).getValue()},
                        {"a5", telem.getAnalog(ParId::A5).getValue()},
                        {"d1", telem.getDigital(ParId::B1).getValue()},
                        {"d2", telem.getDigital(ParId::B2).getValue()},
                        {"d3", telem.getDigital(ParId::B3).getValue()},
                        {"d4", telem.getDigital(ParId::B4).getValue()},
                        {"d5", telem.getDigital(ParId::B5).getValue()},
                        {"d6", telem.getDigital(ParId::B6).getValue()},
                        {"d7", telem.getDigital(ParId::B7).getValue()},
                        {"d8", telem.getDigital(ParId::B8).getValue()},
                        {"comment", telem.getComment()}};
      } break;
      case AprsTelemetry::TelemType::TELEMETRY_COEFFICIENT:
        data["type"] = "TELEMETRY_COEFFICIENT";
        break;
      case AprsTelemetry::TelemType::TELEMETRY_PARAMETER_NAME:
        data["type"] = "TELEMETRY_PARAMETER_NAME";
        break;
      case AprsTelemetry::TelemType::TELEMETRY_PARAMETER_UNIT:
        data["type"] = "TELEMETRY_PARAMETER_UNIT";
        break;
      case AprsTelemetry::TelemType::TELEMETRY_BIT_SENSE_PROJ_NAME:
        data["type"] = "TELEMETRY_BIT_SENSE_PROJ_NAME";
        break;
      default:
        data["type"] = "TELEMETRY_UNKNOWN";
        break;
      };
    } break;
    }
    return data;
  }

  bool verifyType() {
    switch (getType()) {
    case Type::BROADCAST:
    case Type::EXCHANGE:
      return std::holds_alternative<std::string>(contents_);
    case Type::LOCATION:
      return std::holds_alternative<Location>(contents_);
    case Type::IMAGE:
      return std::holds_alternative<Image>(contents_);
    case Type::TELEMETRY:
      return std::holds_alternative<AprsTelemetry>(contents_);
    default:
      giraffe_assert(false);
    }
    return false;
  }

private:
  uint32_t identifier_ = 0;
  Type type_{Type::BROADCAST};
  // std::string data_{};
  // Location location_{};
  std::variant<std::string, Location, Image, AprsTelemetry> contents_;
};

/**
 * @brief A queue for messages.
 */
class MessageQueue {
public:
  MessageQueue() = default;
  ~MessageQueue() = default;

  /**
   * @brief Push a message onto the queue.
   * @details This will lock the queue. If there is no space on the queue,
   * the message will not be pushed and false will be returned.
   *
   * @param message - The message to push.
   * @return true - If the message was pushed.
   * @return false - If the message was not pushed.
   */
  bool push(Message message);

  /**
   * @brief Peek at the next message on the queue.
   * @details This will lock the queue. If there is no message on the queue,
   * false will be returned.
   *
   * @param message - The message that was peeked.
   * @return true - If a message was peeked.
   * @return false - If a message was not peeked.
   */
  bool peek(Message &message);

  /**
   * @brief Pop a message from the queue.
   * @details This will lock the queue. If there is no message on the queue,
   * false will be returned.
   *
   * @param message - The message that was popped.
   * @return true - If a message was popped.
   * @return false - If a message was not popped.
   */
  bool pop(Message &message);

  /**
   * @brief Get the size of the queue.
   * @return int - The size of the queue.
   */
  uint8_t size() const;

private:
  mutable std::mutex mutex_{};
  std::queue<Message> queue_{};
};
} // namespace giraffe::gdl

#endif /* GDL_MESSAGE_HPP_ */