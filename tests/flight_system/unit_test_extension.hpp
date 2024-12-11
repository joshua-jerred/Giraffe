/// =*========GIRAFFE========*=
/// A Unified Flight Command and Control System
/// https://github.com/joshua-jerred/Giraffe
/// https://giraffe.joshuajer.red/
/// =*=======================*=
///
/// @file   unit_test_extension.hpp
///
/// =*=======================*=
/// @author     Joshua Jerred (https://joshuajer.red)
/// @date       2024-12-10
/// @copyright  2024 (license to be defined)

#pragma once

#include "extension_base.hpp"

namespace extension {

class UnitTestExtension : public Extension {
public:
  UnitTestExtension(ExtensionResources &resources,
                    cfg::ExtensionMetadata metadata)
      : Extension(resources, metadata) {
  }
  void loop() override {
  }
};

} // namespace extension