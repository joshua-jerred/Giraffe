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
 * @ref
 * https://mcuoneclipse.com/2021/01/23/assert-__file__-path-and-other-cool-gnu-gcc-tricks-to-be-aware-of/
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-06-13
 * @copyright  2023 (license to be defined)
 */

#ifndef GIRAFFE_ASSERT_HPP_
#define GIRAFFE_ASSERT_HPP_

#include <exception>
#include <stdio.h>

void __assert_func(const char *__file, int __line, const char *__expr);

#define giraffe_assert(__e)                                                    \
  ((__e) ? (void)0 : __assert_func(__FILE__, __LINE__, #__e))

#endif /* GIRAFFE_ASSERT_HPP_ */