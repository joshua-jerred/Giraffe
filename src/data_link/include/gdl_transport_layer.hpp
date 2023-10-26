/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   gdl_transport_layer.hpp
 * @brief  The GDL TransportLayer class/interface
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-07-01
 * @copyright  2023 (license to be defined)
 */

#ifndef GDL_TRANSPORT_LAYER_HPP_
#define GDL_TRANSPORT_LAYER_HPP_

#include "gdl_network_layer.hpp"

namespace gdl {
class TransportLayer {
public:
  TransportLayer(NetworkLayer &network_layer) : network_layer_(network_layer) {
  }
  ~TransportLayer() = default;

  bool send(Message &message) {
    return network_layer_.txMessage(message);
  }

  bool receive(Message &message) {
    return network_layer_.rxMessage(message);
  }

private:
  NetworkLayer &network_layer_;
};
}; // namespace gdl

#endif /* GDL_SESSION_LAYER_INTERFACE_HPP_ */