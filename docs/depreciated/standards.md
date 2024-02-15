# File names:
dashes-between-words.md


### C++
No lines longer than 80 characters.

class names: PascalCase
function names: camelCase

variables:
local: look_like_this
member: look_like_this_
static: s_look_like_this
pointer: p_look_like_this

Macros: ALL_CAPS (but don't use them)

enum class: PascalCase
enum values: ALL_CAPS

Only use enum class. The values are scoped so they don't need to be prefixed
with the enum name.

doxygen header for all .cpp and .h files:
/**
 * @file file-name.cpp
 * @author First Last ([link to github profile])
 * @brief Single short sentence describing the file.
 * @details Longer description of the file. Can be multiple lines. Not required.
 * @date YYYY-MM-DD
 * @copyright Copyright (c) Year
 * @version 0.1 - development branch number when file was last documented
 */

doxygen header for all functions:
/**
 * @brief Single short sentence describing the function.
 * @details Not required.
 * @param type name Description of the parameter.
 * @return type Description of the return value. Required, can use void.
 */

doxygen header for all classes:
/**
 * @brief Single short sentence describing the class.
 * @details Longer description of the class. Can be multiple lines. Not required.
 */


#ifndef EXAMPLE_H_
#define EXAMPLE_H_

#include <stdlib1>
#include <stdlib2>

#include "utility-1.h"
#include "utility-2.h"

#include "module-1.h"
#include "module-2.h"

#include "base-class.h"
#include "example.h" // Header for defined classes

/**
 */
enum class WhateverEnum

/**
 */
class ClassName

/**
 */
void functionName(std::type parameter_name) {
    // Code
}

#endif // EXAMPLE_H_

#### See 'errors.md' for error format