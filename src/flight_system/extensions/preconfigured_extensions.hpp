/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   preconfigured_extensions.hpp
 * @brief  This file contains preconfigured extension metadata.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-06-12
 * @copyright  2023 (license to be defined)
 */

#include "configuration/sections/cfg_extensions.hpp"

namespace ext {
cfg::ExtensionMetadata getBmp180Metadata();
cfg::ExtensionMetadata getBme280Metadata();
cfg::ExtensionMetadata getDs18b20Metadata();
cfg::ExtensionMetadata getSamM8qMetadata();
} // namespace ext