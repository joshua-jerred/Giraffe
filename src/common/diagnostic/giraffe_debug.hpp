/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   giraffe_debug.hpp
 * @brief  Debugging utilities for Giraffe.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-07-03
 * @copyright  2023 (license to be defined)
 */

#ifndef GIRAFFE_DEBUG_HPP_
#define GIRAFFE_DEBUG_HPP_

#if EXTENSION_DEBUG_PRINT

#warning "Extension debug print is enabled"

#include <iostream>
#include <string>

#define EXT_DEBUG(x) std::cout << "[EXT]" << x << std::endl

#else // EXTENSION_PRINT

#define EXT_DEBUG(x) ((void)0)

#endif // EXTENSION_PRINT

#endif /* GIRAFFE_DEBUG_HPP_ */