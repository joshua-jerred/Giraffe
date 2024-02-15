# Style Guide

In general, if anything is not covered here, more info will be in the following
files:
- `.vscode/settings.json`
- `.clang-format`
- `.clang-tidy`
- `.cmakelintrc`


## File names:
under_score_between_words.md

***

## C++
- Use the clang-format file in the root of the project.
- The clang-tidy file is not fully implemented or enforced, but it is a good 
- idea to use it.
- All header files will have the extension '.hpp'.

### Naming

class names: `PascalCase`
function names: `camelCase`

variables:
local: `look_like_this`
member: `look_like_this_`
static: `s_look_like_this`
pointer: `p_look_like_this`

enum class: `PascalCase`
enum values: `ALL_CAPS`
Only use enum class when possible.

constants/constexpr: `K_ALL_CAPS`

### Namespace
Ideally everything giraffe related will be in the 'giraffe' namespace. This is
a recent change, so only some things are in the namespace. Everything new should
be in the namespace.

### Doxygen
doxygen header for all .cpp and .hpp files:
/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   flight_runner_data.cpp
 * @brief  Implementation of non-volatile data for the flight runner.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-01-03
 * @copyright  2024 (license to be defined)
 */

doxygen header for all functions:
/**
 * @brief Single short sentence describing the function.
 * @details Not required.
 * @param name Description of the parameter.
 * @return Description of the return value. Required, can use void.
 */

doxygen header for all classes:
/**
 * @brief Single short sentence describing the class.
 * @details Longer description of the class. Can be multiple lines. Not required.
 */


### Example Header File
```cpp
/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   flight_runner_data.hpp
 * @brief  Non-volatile data for the flight runner.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-01-03
 * @copyright  2024 (license to be defined)
 */

#ifndef FLIGHT_RUNNER_DATA_HPP_
#define FLIGHT_RUNNER_DATA_HPP_

namespace giraffe {
} // namespace giraffe

#endif // FLIGHT_RUNNER_DATA_HPP_
```

***

## Python

Honestly, this is not a core component of the project so it is not all that
enforced. In general, follow PEP 8 or additonally use black.

***

## JavaScript

No style guide, however the `prettier` formatter is used with the default
configuration.

***

## CMake

Format enforced by cmake-lint using the configuration in the root of the 
project.