/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   giraffe_assert.hpp
 * @brief  For absolute failures in the code.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-06-13
 * @copyright  2023 (license to be defined)
 */

#ifndef GIRAFFE_ASSERT_HPP_
#define GIRAFFE_ASSERT_HPP_

#ifndef DNDEBUG // If debug mode is enabled.

#define giraffe_assert(condition) assert(condition __FILE__ __LINE__)

#else // If debug mode is disabled.
/**
 * @brief Currently just a no-op in release mode.
 * @todo Find a way to handle asserts in release mode.
 * @param condition
 */
void giraffe_assert(bool condition) {
}
#endif

#endif /* GIRAFFE_ASSERT_HPP_ */