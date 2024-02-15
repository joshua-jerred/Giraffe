/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   mcp3021.hpp
 * @brief  MCP3021 I2C ADC Driver
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-07-05
 * @copyright  2023 (license to be defined)
 */

#ifndef MCP3021_HPP_
#define MCP3021_HPP_

#include "adc_extension_base.hpp"
#include "i2c_extension_adapter.hpp"

namespace extension {

class Mcp3021 : public AdcExtensionBase<10>, public I2cExtensionAdapter {
public:
  Mcp3021(ExtensionResources &resources, cfg::ExtensionMetadata metadata);

private:
  void adcStartup() override;
  bool i2cHandshake() override;
  bool readAdc(uint32_t &value) override;
};

} // namespace extension

#endif /* MCP3021_HPP_ */