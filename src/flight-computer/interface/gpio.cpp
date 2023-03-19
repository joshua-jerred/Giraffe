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

#include "gpio.h"

#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#include <cstdint>
#include <string>

// Global gpio memory map
static volatile uint32_t* GLOBAL_GPIO_MEMORY_MAP = (uint32_t*)MAP_FAILED;
static uint32_t PIN_RESERVATION_MAP = 0;

// Size of the GPIO memory map. Used for mmap and munmap
inline constexpr uint32_t kGpioMemoryMapSize = 0x1000;
inline void CheckInitialized() {
  if (GLOBAL_GPIO_MEMORY_MAP == MAP_FAILED) {
    throw gpio::GpioException("GPIO not initialized");
  }
}

inline void VerifyNotReserved(uint8_t pin) {
  if (PIN_RESERVATION_MAP & (1 << pin)) {
    throw gpio::GpioException("Pin " + std::to_string(pin) +
                              " is already reserved");
  }
}

inline void VerifyReserved(uint8_t pin) {
  if (!(PIN_RESERVATION_MAP & (1 << pin))) {
    throw gpio::GpioException("Pin " + std::to_string(pin) +
                              " is not reserved");
  }
}

inline void ReservePin(uint8_t pin) { 
  VerifyNotReserved(pin); // I can't think of a reason why this would be needed
  PIN_RESERVATION_MAP |= (1 << pin); 
}

gpio::Pin::Pin(uint8_t bcm_pin_number, PinMode mode)
    : bcm_pin_number(bcm_pin_number), mode(mode) {
  if (bcm_pin_number > 27) {
    throw gpio::GpioException("Invalid pin number: " +
                              std::to_string(bcm_pin_number));
  }
}

// From bcm2835.c
// Write with a memory barrier
inline void WriteWithBarrier(volatile uint32_t *address, uint32_t value) {
  CheckInitialized();
  __sync_synchronize();
  *address = value;
  __sync_synchronize();
}

// From bcm2835.c
// Read with a memory barrier
inline uint32_t ReadWithBarrier(volatile uint32_t *address) {
  CheckInitialized();
  uint32_t value;
  __sync_synchronize();
  value = *address;
  __sync_synchronize();
  return value;
}

inline volatile uint32_t *CalculateAddress(uint8_t offset, uint8_t pin) {
  return (uint32_t *)(GLOBAL_GPIO_MEMORY_MAP + offset / 4 + pin / 32);
}

void gpio::Initialize() {
  if (GLOBAL_GPIO_MEMORY_MAP != MAP_FAILED) {
    return;
  }

  int memfd = open("/dev/gpiomem", O_RDWR | O_SYNC);
  if (memfd < 0) {
    throw gpio::GpioException("Failed to open /dev/gpiomem");
  }

  GLOBAL_GPIO_MEMORY_MAP = static_cast<uint32_t *>(
      mmap(NULL, kGpioMemoryMapSize, (PROT_READ | PROT_WRITE), MAP_SHARED,
           memfd, 0));

  if (GLOBAL_GPIO_MEMORY_MAP == MAP_FAILED) {
    throw gpio::GpioException("Failed to map the GPIO registers");
  }

  close(memfd);
}

void gpio::Close() {
  if (GLOBAL_GPIO_MEMORY_MAP == MAP_FAILED) {
    return;
  }
  munmap((void *)GLOBAL_GPIO_MEMORY_MAP, kGpioMemoryMapSize);
  GLOBAL_GPIO_MEMORY_MAP = (uint32_t *)MAP_FAILED;
}

void gpio::SetupPin(Pin pin) {
  CheckInitialized();
  VerifyNotReserved(pin.bcm_pin_number);
  const uint32_t kSetUpPinOffset = 0x00;
  const uint32_t kGpioSelectMask = 0x07;

  volatile uint32_t *paddr =
      GLOBAL_GPIO_MEMORY_MAP + kSetUpPinOffset / 4 + (pin.bcm_pin_number / 10);
  uint8_t shift = (pin.bcm_pin_number % 10) * 3;
  uint32_t mask = kGpioSelectMask << shift;

  uint8_t mode = pin.mode == PinMode::INPUT ? 0x00 : 0x01;
  uint32_t value = mode << shift;

  uint32_t current_value = ReadWithBarrier(paddr);
  current_value = (current_value & ~mask) | (value & mask);  // see bcm2835.c
  WriteWithBarrier(paddr, current_value);
  ReservePin(pin.bcm_pin_number);
}

void gpio::Write(Pin pin, bool on) {
  const uint32_t kPinOn = 0x1c;
  const uint32_t kPinOff = 0x28;

  VerifyReserved(pin.bcm_pin_number);

  if (pin.mode != PinMode::OUTPUT) {
    throw gpio::GpioException("Pin" + std::to_string(pin.bcm_pin_number) +
                              " is not in output mode");
  }

  uint8_t state = on ? kPinOn : kPinOff;
  WriteWithBarrier(CalculateAddress(state, pin.bcm_pin_number),
                   1 << pin.bcm_pin_number % 32);
}

bool gpio::Read(Pin pin) {
  const uint32_t kReadPin = 0x34;

  VerifyReserved(pin.bcm_pin_number);

  if (pin.mode != PinMode::INPUT) {
    throw gpio::GpioException("Pin" + std::to_string(pin.bcm_pin_number) +
                              " is not in input mode");
  }

  uint32_t value =
      ReadWithBarrier(CalculateAddress(kReadPin, pin.bcm_pin_number));
  uint8_t shift = pin.bcm_pin_number % 32;
  return (value & (1 << shift)) ? 1 : 0;
}
