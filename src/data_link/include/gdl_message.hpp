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

#include <cstdint>
#include <string>

namespace gdl {

/**
 * @brief A generic message struct for GDL
 */
struct Message {
  uint16_t id;
  std::string data;
};

} // namespace gdl

#endif /* GDL_MESSAGE_HPP_ */