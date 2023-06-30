#include <thread>
#include <iostream>

#include "interface.h"

int main() {
  using namespace interface;
  Gpio::Initialize();

  Gpio::Pin led_pin(17, Gpio::PinMode::OUTPUT);
  Gpio::Pin button_pin(27, Gpio::PinMode::INPUT);

  Gpio gpio;
  gpio.SetupPin(led_pin);
  gpio.SetupPin(button_pin);

  int i = 10;
  while (i--) {
    gpio.Write(led_pin, true);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    gpio.Write(led_pin, false);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    std::cout << "Button: " << gpio.Read(button_pin) << std::endl;
  }

  Gpio::Close();

  return 0;
}