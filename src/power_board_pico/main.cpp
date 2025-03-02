/// =*========GIRAFFE========*=
/// A Unified Flight Command and Control System
/// https://github.com/joshua-jerred/Giraffe
/// https://giraffe.joshuajer.red/
/// =*=======================*=
///
/// @file   main.cpp
///
/// =*=======================*=
/// @author     Joshua Jerred (https://joshuajer.red)
/// @date       2025-03-01
/// @copyright  2025 (license to be defined)

// C++ Standard Library
#include <stdio.h>

// pico-sdk
#include "hardware/gpio.h"
#include "pico/binary_info.h"
#include "pico/config.h"
#include "pico/stdlib.h"
#include "pico/time.h"

// Local
#include "power_board_comms.hpp"
#include "status_led.hpp"

// If debug build, allow software restart
// #ifdef DEBUG
#ifndef PICO_STDIO_USB_ENABLE_RESET_VIA_VENDOR_INTERFACE
#define PICO_STDIO_USB_ENABLE_RESET_VIA_VENDOR_INTERFACE 1
#endif
// #endif
// #define PICO_STDIO_USB_RESET_MAGIC_BAUD_RATE 19200

bi_decl(bi_program_version_string(GIRAFFE_VERSION_NUMBER));
bi_decl(bi_program_description("Power Board Pico"));

static power_board::StatusLed status_led;

int main() {

  static power_board::PowerBoardComms power_board_comms{};
  stdio_set_chars_available_callback(
      [](void *) {
        power_board_comms.charsAvailable();

        status_led.toggle();
      },
      NULL);

  while (1) {
    status_led.process();
    power_board_comms.process();
    // stdio_putchar('.');
  }

  return 0;
}

void exception_handler() {
  while (1) {
    status_led.toggle();
    sleep_ms(100);
  }
}