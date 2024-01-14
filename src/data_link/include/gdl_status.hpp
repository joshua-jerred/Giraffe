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
#include <iostream>

#include <SignalEasel/aprs.hpp>

namespace gdl {

struct GdlStatus {
  size_t exchange_queue_size = 0;
  size_t broadcast_queue_size = 0;
  size_t received_queue_size = 0;
  double volume = 0.0;
  double signal_to_noise_ratio = 0.0;

  /**
   * @brief Audio processing latency in milliseconds
   */
  uint64_t network_layer_latency_ms = 0;

  signal_easel::aprs::Receiver::Stats aprs_receiver_stats{};

  // void print() {
  //   std::cout << "GDL Status: " << std::endl;
  //   std::cout << "  Exchange Queue Size: " << exchange_queue_size <<
  //   std::endl; std::cout << "  Broadcast Queue Size: " <<
  //   broadcast_queue_size
  //             << std::endl;
  //   std::cout << "  Received Queue Size: " << received_queue_size <<
  //   std::endl; std::cout << "  Network Layer Latency: " <<
  //   network_layer_latency_ms << "ms"
  //             << std::endl;
  // }
};

} // namespace gdl

#endif /* GDL_STATUS_HPP_ */