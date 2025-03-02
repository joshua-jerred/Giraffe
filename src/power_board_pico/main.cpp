#include "pico/binary_info.h"
#include "pico/stdlib.h"

#include <stdio.h>

// If debug build, allow software restart
#ifndef PICO_STDIO_USB_ENABLE_RESET_VIA_VENDOR_INTERFACE
#define PICO_STDIO_USB_ENABLE_RESET_VIA_VENDOR_INTERFACE 1
#endif
// #define PICO_STDIO_USB_RESET_MAGIC_BAUD_RATE 19200

const uint LED_PIN = 25;
const uint SLEEP_MS = 100;

int main() {
  stdio_init_all();
  // setup_default_uart();

  bi_decl(bi_program_description("First Blink"));
  bi_decl(bi_1pin_with_name(LED_PIN, "On-board LED"));

  gpio_init(LED_PIN);
  gpio_set_dir(LED_PIN, GPIO_OUT);
  while (1) {
    gpio_put(LED_PIN, 0);
    sleep_ms(SLEEP_MS);
    gpio_put(LED_PIN, 1);
    sleep_ms(SLEEP_MS);
    printf("Hello, world!\n");
  }

  return 0;
}

// #include <Arduino.h>

// // put function declarations here:
// int myFunction(int, int);

// void setup() {
//   // put your setup code here, to run once:
//   int result = myFunction(2, 3);
// }

// void loop() {
//   // put your main code here, to run repeatedly:
// }

// // put function definitions here:
// int myFunction(int x, int y) {
//   return x + y;
// }