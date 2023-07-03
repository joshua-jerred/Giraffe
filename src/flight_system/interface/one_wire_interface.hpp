/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   one_wire_interface.hpp
 * @brief  1-Wire interface header file.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-07-03
 * @copyright  2023 (license to be defined)
 */

#ifndef ONE_WIRE_INTERFACE_HPP_
#define ONE_WIRE_INTERFACE_HPP_

#include <string>

class OneWireInterface {
public:
  OneWireInterface() = default;
  ~OneWireInterface() = default;

  void setDeviceId(std::string device_id);
  bool readW1SlaveFile(std::string &data);
  bool readTemperatureFile(std::string &data);
  bool deviceExists();

private:
  std::string path_{};
};

#endif /* ONE_WIRE_INTERFACE_HPP_ */