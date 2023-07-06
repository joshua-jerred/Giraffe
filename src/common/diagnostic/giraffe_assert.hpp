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
 * @cite Styger
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

void __assert_func(const char *file_name, int line_number,
                   const char *expression);

#define giraffe_assert(__e)                                                    \
  ((__e) ? (void)0 : __assert_func(__FILE__, __LINE__, #__e))

#endif /* GIRAFFE_ASSERT_HPP_ */