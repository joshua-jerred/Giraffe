/**
 * @file gpio.cpp
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief Low level control of Raspberry Pi BCM pins
 * @details This is a simple low level BCM interface to the raspberry pi GPIO
 * pins. It's similar to and inspired by the bcm2835 library made by Mike
 * McCauley (https://www.airspayce.com/mikem/bcm2835/index.html) and the
 * article by Rich Youngkin.
 *
 * @cite https://www.airspayce.com/mikem/bcm2835/index.html
 * @cite https://github.com/youngkin/gpio/blob/main/ledblink/bcmfuncs.c
 *
 * @date 2023-03-18
 * @copyright Copyright (c) 2023
 * @version 0.4
 */

#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#include <cstdint>
#include <mutex>
#include <string>

#include "interface.h"

using namespace interface;

// GPIO Pin
Gpio::Pin::Pin(uint8_t pin_number, Gpio::PinMode mode, bool initial_state)
    : pin_number_(pin_number), mode_(mode), initial_state_(initial_state) {
  if (pin_number > 27) {  // 0-27 pins on the raspberry pi
    throw GpioException("Invalid pin number");
  }
  if (mode == PinMode::UNINITIALIZED) {
    throw GpioException("Invalid pin mode: " + this->ToString());
  }
  if (mode == PinMode::INPUT && initial_state) {
    throw GpioException("Cannot set initial state on input pin: " +
                        this->ToString());
  }
}

std::string Gpio::Pin::Pin::ToString() const {
  return std::to_string(pin_number_) + ":" +
         (mode_ == PinMode::INPUT ? "I" : "O") +
         (mode_ == PinMode::OUTPUT ? ":" + std::to_string(initial_state_) : "");
}

// Static members
volatile uint32_t *Gpio::gpio_memory_map_ = (uint32_t *)MAP_FAILED;
std::array<Gpio::Pin, 28> Gpio::reserved_pins_ = {};
std::mutex Gpio::gpio_lock_;

// Static
void Gpio::Initialize() {
  gpio_lock_.lock();
  if (gpio_memory_map_ != MAP_FAILED) {  // Already initialized
    gpio_lock_.unlock();
    return;
  }

  int memfd = open("/dev/gpiomem", O_RDWR | O_SYNC);
  if (memfd < 0) {
    gpio_lock_.unlock();
    throw GpioException("Failed to open /dev/gpiomem");
  }

  gpio_memory_map_ = static_cast<uint32_t *>(mmap(NULL, kGpioMemoryMapSize,
                                                  (PROT_READ | PROT_WRITE),
                                                  MAP_SHARED, memfd, 0));

  if (gpio_memory_map_ == MAP_FAILED) {
    gpio_lock_.unlock();
    throw GpioException("Failed to map the GPIO registers");
  }
  close(memfd);
  gpio_lock_.unlock();
}

void Gpio::Close() {
  gpio_lock_.lock();
  if (gpio_memory_map_ == MAP_FAILED) {
    gpio_lock_.unlock();
    return;
  }
  munmap((void *)gpio_memory_map_, kGpioMemoryMapSize);
  gpio_memory_map_ = (uint32_t *)MAP_FAILED;

  reserved_pins_ = {};
  gpio_lock_.unlock();
}

bool Gpio::IsInitialized() {
  gpio_lock_.lock();
  bool initialized = gpio_memory_map_ != MAP_FAILED;
  gpio_lock_.unlock();
  return initialized;
}

void Gpio::SetupPin(Pin pin) {
  gpio_lock_.lock();
  if (!VerifyInitialized()) {
    gpio_lock_.unlock();
    throw GpioException("GPIO not initialized, can not setup pin: " +
                        pin.ToString());
  }
  if (IsPinReserved(pin)) {
    gpio_lock_.unlock();
    throw GpioException("Pin" + pin.ToString() + " is already reserved");
  }
  if (pin.mode_ == PinMode::UNINITIALIZED) {
    gpio_lock_.unlock();
    throw GpioException("Pin" + pin.ToString() + " has an invalid mode");
  }

  const uint32_t kSetUpPinOffset = 0x00;
  const uint32_t kGpioSelectMask = 0x07;

  volatile uint32_t *pin_address =
      gpio_memory_map_ + kSetUpPinOffset / 4 + (pin.pin_number_ / 10);
  uint8_t shift = (pin.pin_number_ % 10) * 3;
  uint32_t mask = kGpioSelectMask << shift;

  uint8_t mode = pin.mode_ == PinMode::INPUT ? 0x00 : 0x01;
  uint32_t value = mode << shift;

  volatile uint32_t current_value = ReadWithBarrier(pin_address);
  current_value = (current_value & ~mask) | (value & mask);  // see bcm2835.c
  WriteWithBarrier(pin_address, current_value);
  ReservePin(pin);
  SetOwner(pin);
  gpio_lock_.unlock();
}

void Gpio::Write(Pin pin, bool on) {
  gpio_lock_.lock();
  if (!VerifyInitialized()) {
    gpio_lock_.unlock();
    throw GpioException("GPIO not initialized, can not write pin: " +
                        pin.ToString());
  }
  if (!IsPinReserved(pin) || !IsOwner(pin)) {
    gpio_lock_.unlock();
    throw GpioException("Pin" + pin.ToString() + " is not reserved");
  }
  if (pin.mode_ != PinMode::OUTPUT) {
    gpio_lock_.unlock();
    throw GpioException("Pin" + std::to_string(pin.pin_number_) +
                        " is not in output mode");
  }

  const uint32_t kPinOn = 0x1c;
  const uint32_t kPinOff = 0x28;

  uint8_t state = on ? kPinOn : kPinOff;
  WriteWithBarrier(CalculateAddress(state, pin.pin_number_),
                   1 << pin.pin_number_);
  gpio_lock_.unlock();
}

bool Gpio::Read(Pin pin) {
  gpio_lock_.lock();
  const uint32_t kReadPin = 0x34;

  if (!VerifyInitialized()) {
    gpio_lock_.unlock();
    throw GpioException("GPIO not initialized, can not read pin: " +
                        pin.ToString());
  }
  if (!IsPinReserved(pin) || !IsOwner(pin)) {
    gpio_lock_.unlock();
    throw GpioException("Pin" + pin.ToString() + " is not reserved");
  }
  if (pin.mode_ != PinMode::INPUT) {
    gpio_lock_.unlock();
    throw Gpio::GpioException("Pin" + std::to_string(pin.pin_number_) +
                              " is not in input mode");
  }

  uint32_t value = ReadWithBarrier(CalculateAddress(kReadPin, pin.pin_number_));
  uint8_t shift = pin.pin_number_;
  gpio_lock_.unlock();
  return (value & (1 << shift)) ? 1 : 0;
}

// Lock must be held
bool Gpio::VerifyInitialized() { return !(gpio_memory_map_ == MAP_FAILED); }

// Lock must be held
bool Gpio::IsPinReserved(const Pin &pin) {
  if (reserved_pins_[pin.pin_number_].mode_ == PinMode::UNINITIALIZED &&
      reserved_pins_[pin.pin_number_].pin_number_ == 0) {
    return false;
  }
  return true;
}

void Gpio::ReservePin(Pin pin) {
  reserved_pins_[pin.pin_number_] = pin;
}

bool Gpio::IsOwner(const Pin &pin) {
  bool is_owner = pins_owned_ & (1 << pin.pin_number_);
  return is_owner;
}

void Gpio::SetOwner(const Pin &pin) {
  pins_owned_ |= (1 << pin.pin_number_);
}

// From bcm2835.c
// Write with a memory barrier
void Gpio::WriteWithBarrier(volatile uint32_t *address, uint32_t value) {
  __sync_synchronize();
  *address = value;
  __sync_synchronize();
}

// From bcm2835.c
// Read with a memory barrier
uint32_t Gpio::ReadWithBarrier(volatile uint32_t *address) {
  uint32_t value;
  __sync_synchronize();
  value = *address;
  __sync_synchronize();
  return value;
}

volatile uint32_t *Gpio::CalculateAddress(uint8_t offset, uint8_t pin) {
  return (uint32_t *)(gpio_memory_map_ + offset / 4 + pin / 32);
}

