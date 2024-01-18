/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   gdl_message.hpp
 * @brief  The message struct for GDL
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-06-24
 * @copyright  2023 (license to be defined)
 */

#ifndef GDL_MESSAGE_HPP_
#define GDL_MESSAGE_HPP_

#include <BoosterSeat/time.hpp>

#include <cstdint>
#include <string>

namespace gdl {

/**
 * @brief A generic message struct for GDL - Layer 4
 */
struct Message {
  enum class Type : uint8_t {
    UNDEFINED = 0b00000000,
    BROADCAST = 0b11000011,
    EXCHANGE = 0b00111100,
    ACK = 0b11110000,
    NACK = 0b00001111,
  };

  enum class UpperLayer { UNDEFINED, APRS_MESSAGE, APRS_GPS };

  std::string id{}; // string 0-5 characters
  Type type{Type::UNDEFINED};
  UpperLayer upper_layer{UpperLayer::UNDEFINED};
  std::string data{""};
  uint32_t retries = 0;

  /**
   * @brief Set when the message is decoded. Used to keep track of the age of a
   * message.
   */
  bst::Time time_decoded{};

  bool isValid() {
    return true;
  }

  bool isAck() {
    return data == "ACK";
  }
};

} // namespace gdl

#endif /* GDL_MESSAGE_HPP_ */