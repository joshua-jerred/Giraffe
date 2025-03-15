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
#include "hardware/exception.h"
#include "hardware/gpio.h"
#include "hardware/watchdog.h"

#include "pico/binary_info.h"
#include "pico/config.h"
#include "pico/stdlib.h"
#include "pico/time.h"

// Local
#include "adc_data.hpp"
#include "power_board_comms.hpp"
#include "power_supply.hpp"
#include "sam_m8q.hpp"
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

static power_board::StatusLed status_led{100, 1000};

void exception_handler() {
  while (1) {
    printf("Exception occurred\n");
    status_led.toggle();
    sleep_ms(100);
  }
}

int main() {
  watchdog_enable(2000, false);

  exception_set_exclusive_handler(HARDFAULT_EXCEPTION, exception_handler);
  exception_set_exclusive_handler(NMI_EXCEPTION, exception_handler);
  // exception_set_exclusive_handler(MEMMANAGE_EXCEPTION, exception_handler);
  // exception_set_exclusive_handler(BUSFAULT_EXCEPTION, exception_handler);
  // exception_set_exclusive_handler(USAGEFAULT_EXCEPTION, exception_handler);
  exception_set_exclusive_handler(SVCALL_EXCEPTION, exception_handler);
  exception_set_exclusive_handler(PENDSV_EXCEPTION, exception_handler);
  exception_set_exclusive_handler(SYSTICK_EXCEPTION, exception_handler);

  status_led.toggle();
  sleep_ms(100);
  status_led.toggle();

  static power_board::PowerBoardComms power_board_comms{};

  if (watchdog_caused_reboot() || watchdog_enable_caused_reboot()) {
    printf("Rebooted by watchdog\n");
  }

  static power_board::AdcData adc_data{1000};
  static power_board::PowerSupply power_supply{1000};
  static power_board::SamM8Q sam_m8q{100};

  stdio_set_chars_available_callback(
      [](void *) {
        power_board_comms.charsAvailable();
        status_led.toggle();
      },
      NULL);

  watchdog_update();
  printf("STARTED\n");
  while (1) {
    status_led.process();
    power_board_comms.process();

    std::string command;
    std::string arg;
    bool have_command = power_board_comms.getCommand(command, arg);
    if (have_command) {
      if (command == "gps") {
        sam_m8q.processCommand(arg);
      } else if (command == "adc") {
        adc_data.processCommand(arg);
      } else {
        printf("Unknown command in main: %s\n", command.c_str());
      }
    }

    adc_data.process();
    power_supply.process();
    sam_m8q.process();

    sleep_ms(5);
    watchdog_update();
  }
  return 0;
}
