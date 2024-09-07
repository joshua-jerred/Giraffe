/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   preconfigured_extensions.cpp
 * @brief  Shortcuts for quickly adding extensions to the configuration. Part of
 * the extension module.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-02-20
 * @copyright  2024 (license to be defined)
 */

#include "extension_module.hpp"

#include <iostream>

namespace modules {

void ExtensionModule::addPreConfiguredExtension(const std::string &ext_option) {
  cfg::ExtensionMetadata extension_metadata_;
  extension_metadata_.critical = false;
  extension_metadata_.enabled = true;
  extension_metadata_.update_interval = 500;

  if (ext_option == "bme280") {
    extension_metadata_.name = "bme280";
    extension_metadata_.type = cfg::gEnum::ExtensionType::BME280;
  } else if (ext_option == "samm8q") {
    extension_metadata_.name = "samm8q";
    extension_metadata_.type = cfg::gEnum::ExtensionType::SAM_M8Q;
  } else if (ext_option == "ds18b20") {
    extension_metadata_.name = "ds18b20";
    extension_metadata_.type = cfg::gEnum::ExtensionType::DS18B20;
  } else if (ext_option == "mcp3021") {
    extension_metadata_.name = "mcp3021";
    extension_metadata_.type = cfg::gEnum::ExtensionType::MCP3021;
  } else {
    error(DiagnosticId::EXTENSION_MODULE_invalidPreconfiguredExtensionType,
          ext_option);
    return;
  }

  // add to the configuration
  configuration_.extensions.addExtension(extension_metadata_);

  // add to the current list of extensions
  addExtension(extension_metadata_);

  updateLocalConfig();
}

} // namespace modules