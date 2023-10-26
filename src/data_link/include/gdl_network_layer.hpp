/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   gdl_network_layer.hpp
 * @brief  The network layer for the Giraffe Data Link
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-10-26
 * @copyright  2023 (license to be defined)
 */

#ifndef GDL_NETWORK_LAYER_HPP_
#define GDL_NETWORK_LAYER_HPP_

#include <cstdint>
#include <vector>

#include "gdl_message.hpp"
#include "gdl_physical_layer.hpp"

namespace gdl {

class NetworkLayer {
public:
  NetworkLayer(PhysicalLayer &physical_layer)
      : physical_layer_(physical_layer) {
    physical_layer_.enable();
  }

  bool txMessage(Message &message) {
    std::vector<uint8_t> bytes(message.data.begin(), message.data.end());
    return physical_layer_.transmitBytes(bytes);
  }

  bool rxMessage(Message &message) {
    std::vector<uint8_t> bytes = physical_layer_.receiveBytes();
    std::string data = "";
    for (auto byte : bytes) {
      data += byte;
    }
    message.data = data;
    return true;
  }

protected:
  PhysicalLayer &physical_layer_;
};

} // namespace gdl

#endif /* GDL_PHYSICAL_LAYER_HPP_ */