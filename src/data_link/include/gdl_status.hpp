/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   gdl_status.hpp
 * @brief  GDL Status class/interface
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-12-10
 * @copyright  2023 (license to be defined)
 */

#ifndef GDL_STATUS_HPP_
#define GDL_STATUS_HPP_

#include <cstdint>

namespace gdl {

struct GdlStatus {
  size_t exchange_queue_size = 0;
  size_t broadcast_queue_size = 0;
  size_t received_queue_size = 0;
};

} // namespace gdl

#endif /* GDL_STATUS_HPP_ */