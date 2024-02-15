/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   giraffe_debug_assert.cpp
 * @brief  A generic giraffe assert function for debugging.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-07-01
 * @copyright  2023 (license to be defined)
 */

#include "giraffe_assert.hpp"

#include <iostream>

#ifndef NDEBUG
void __assert_func(const char *__file, int __line, const char *__expr) {
  std::cout << "ASSERT: " << __file << ":" << __line << ": Assertion `"
            << __expr << "' failed." << std::endl;
  std::abort();
}
#else
/// @todo Implement a better way to handle asserts in release mode.
void __assert_func(const char *__file, int __line, const char *__expr) {
  std::cout << "ASSERT: " << __file << ":" << __line << ": Assertion `"
            << __expr << "' failed." << std::endl;
}
// static_assert(false);
#endif /* NDEBUG */