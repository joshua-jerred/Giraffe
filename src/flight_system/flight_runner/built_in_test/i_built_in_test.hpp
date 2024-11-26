/// =*========GIRAFFE========*=
/// A Unified Flight Command and Control System
/// https://github.com/joshua-jerred/Giraffe
/// https://giraffe.joshuajer.red/
/// =*=======================*=
///
/// @file   i_built_in_test.hpp
///
/// =*=======================*=
/// @author     Joshua Jerred (https://joshuajer.red)
/// @date       2024-11-24
/// @copyright  2024 (license to be defined)

#pragma once

#include "json.hpp"

namespace bit {

/// @brief Access to build-in test data.
class IBuiltInTest {
public:
  virtual ~IBuiltInTest() = default;

  virtual void getBitTestData(Json &out_data) = 0;
};

} // namespace bit