/**
 * @file serial.cpp
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief A simple linux serial wrapper for extensions.
 * @date 2023-03-17
 * @copyright Copyright (c) 2023
 * @version 0.4
 */

#include <errno.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

#include <string>

#include "extension-interface.h"


