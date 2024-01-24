// GPIO13 <-> GPIO12
// GPIO26 <-> GPIO16

#include "gpio.hpp"
#include "unit_test.hpp"

#include <BoosterSeat/sleep.hpp>

using namespace giraffe;

TEST(HostTest_GPIO, TestBasicIO) {

  Gpio::initialize();
  Gpio gpio{};

  Gpio::Pin pin26{26, Gpio::PinMode::OUTPUT};
  gpio.setupPin(pin26);

  gpio.write(pin26, true);
  bst::sleep(1000);
  gpio.write(pin26, false);
  bst::sleep(1000);
  gpio.write(pin26, true);
  bst::sleep(1000);
  gpio.write(pin26, false);

  // Gpio::Pin pin13{13, Gpio::PinMode::OUTPUT};
  // Gpio::Pin pin12{12, Gpio::PinMode::INPUT};
  //
  // gpio.setupPin(pin13);
  // gpio.setupPin(pin12);
  //
  // gpio.write(pin13, false);
  // EXPECT_EQ(gpio.read(pin12), false);
  //
  // gpio.write(pin13, true);
  // EXPECT_EQ(gpio.read(pin12), true);
  //
  // gpio.write(pin13, false);
  // EXPECT_EQ(gpio.read(pin12), false);
}