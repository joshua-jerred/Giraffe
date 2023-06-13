/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   extension_module.hpp
 * @brief  This is the header file for the new extension module.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-06-12
 * @copyright  2023 (license to be defined)
 */

#ifndef EXTENSION_MODULE_HPP_
#define EXTENSION_MODULE_HPP_

#include "module.hpp"

namespace modules {

class ExtensionModule : public Module {
public:
  ExtensionModule(data::SharedData &, cfg::Configuration &);
  ~ExtensionModule() override = default;

private:
  void startup() override;
  void loop() override;
  void shutdown() override;
  void processCommand(const command::Command &);
};

} // namespace modules

#endif /* EXTENSION_MODULE_HPP_ */
