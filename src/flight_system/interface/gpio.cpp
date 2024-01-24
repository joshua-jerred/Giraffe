/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   gpio.cpp
 * @brief  GPIO interface source file
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-01-24
 * @copyright  2024 (license to be defined)
 */

/**
 * @cite https://datasheets.raspberrypi.com/bcm2835/bcm2835-peripherals.pdf
 * @cite https://www.airspayce.com/mikem/bcm2835/index.html
 * @cite https://github.com/youngkin/gpio/blob/main/ledblink/bcmfuncs.c
 */

#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#include "giraffe_exception.hpp"
#include "gpio.hpp"

/**
 * @brief The Gpio memory map is used store the mapping to the BCM registers.
 * @details Initialized in Gpio::Initialize().
 */
static volatile uint32_t *s_gpio_memory_map = (uint32_t *)MAP_FAILED;
/**
 * @brief Size of the memory map, one page.
 * @todo I need to find a source for this.
 */
static const uint32_t GPIO_MEMORY_MAP_SIZE = 0x1000;

/**
 * @brief A function to check if the BCM GPIO registers have been mapped.
 * @details This function is not thread safe, the caller must lock the mutex
 * prior to calling this function.
 * @return true Mapped, GPIO is ready to use.
 * @return false Not mapped.
 */
bool verifyInitialized() {
  return s_gpio_memory_map != MAP_FAILED;
}

/**
 * @brief Writes a value to a memory address with a memory barrier.
 * @cite https://www.airspayce.com/mikem/bcm2835/index.html
 * @param address The address to write to.
 * @param value The value to write.
 */
void writeWithBarrier(volatile uint32_t *address, uint32_t value) {
  __sync_synchronize();
  *address = value;
  __sync_synchronize();
}

/**
 * @brief Reads a value from a memory address with a memory barrier.
 * @cite https://www.airspayce.com/mikem/bcm2835/index.html
 * @param address The address to read from.
 * @return uint32_t The value read from the address.
 */
uint32_t readWithBarrier(volatile uint32_t *address) {
  uint32_t value;
  __sync_synchronize();
  value = *address;
  __sync_synchronize();
  return value;
}

volatile uint32_t *calculateAddress(uint8_t offset, uint8_t pin) {
  return (uint32_t *)(s_gpio_memory_map + offset / 4 + pin / 32);
}

namespace giraffe {

/**
 * @brief This mutex is used to ensure that only one instance of Gpio can
 * access the BCM registers at a time along with static members.
 */
std::mutex Gpio::gpio_lock_;
std::array<Gpio::Pin, 28> Gpio::reserved_pins_ = {};

inline constexpr uint8_t MAX_PIN_NUMBER = 27;

Gpio::Pin::Pin(uint8_t pin_number, PinMode mode, bool default_state)
    : pin_number(pin_number), mode(mode), default_state(default_state) {
  if (pin_number > MAX_PIN_NUMBER) {
    throw GiraffeException(DiagnosticId::INTERFACE_invalidGpioPin,
                           std::to_string(pin_number));
  }
  if (mode == PinMode::UNINITIALIZED) {
    throw GiraffeException(DiagnosticId::INTERFACE_invalidGpioPinMode,
                           std::to_string(pin_number));
  }
  if (mode == PinMode::INPUT && default_state) {
    throw GiraffeException(DiagnosticId::INTERFACE_initialStateInputPin,
                           std::to_string(pin_number));
  }
}

void Gpio::initialize() {
  const std::lock_guard<std::mutex> lock(gpio_lock_);
  if (s_gpio_memory_map != MAP_FAILED) { // Already initialized
    return;
  }
  int mem_fd = open("/dev/gpiomem", O_RDWR | O_SYNC);
  if (mem_fd < 0) {
    throw GiraffeException(DiagnosticId::INTERFACE_gpioFailedToInit);
  }

  s_gpio_memory_map = static_cast<uint32_t *>(mmap(NULL, GPIO_MEMORY_MAP_SIZE,
                                                   (PROT_READ | PROT_WRITE),
                                                   MAP_SHARED, mem_fd, 0));

  if (s_gpio_memory_map == MAP_FAILED) {
    throw GiraffeException(DiagnosticId::INTERFACE_gpioFailedToMapRegisters);
  }
  close(mem_fd);
}

void Gpio::deInitialize() {
  const std::lock_guard<std::mutex> lock(gpio_lock_);
  if (s_gpio_memory_map == MAP_FAILED) {
    return;
  }
  munmap((void *)s_gpio_memory_map, GPIO_MEMORY_MAP_SIZE);
  s_gpio_memory_map = (uint32_t *)MAP_FAILED;
  reserved_pins_ = {}; // clear the reserved pins array
}

bool Gpio::isInitialized() {
  const std::lock_guard<std::mutex> lock(gpio_lock_);
  return verifyInitialized();
}

void Gpio::setupPin(Pin pin) {
  const std::lock_guard<std::mutex> lock(gpio_lock_);
  if (!verifyInitialized()) {
    throw GiraffeException(DiagnosticId::INTERFACE_gpioNotInitialized,
                           std::to_string(pin.pin_number));
  }
  if (isPinReserved(pin)) {
    throw GiraffeException(DiagnosticId::INTERFACE_gpioPinAlreadyReserved,
                           std::to_string(pin.pin_number));
  }
  if (pin.mode == PinMode::UNINITIALIZED) {
    throw GiraffeException(DiagnosticId::INTERFACE_invalidGpioPinMode,
                           std::to_string(pin.pin_number));
  }

  const uint32_t kSetUpPinOffset = 0x00;
  const uint32_t kGpioSelectMask = 0x07;

  volatile uint32_t *pin_address =
      s_gpio_memory_map + kSetUpPinOffset / 4 + (pin.pin_number / 10);
  uint8_t shift = (pin.pin_number % 10) * 3;
  uint32_t mask = kGpioSelectMask << shift;

  uint8_t mode = (pin.mode == PinMode::INPUT ? 0x00 : 0x01);
  uint32_t value = mode << shift;

  volatile uint32_t current_value = readWithBarrier(pin_address);
  current_value = (current_value & ~mask) | (value & mask); // see bcm2835.c
  writeWithBarrier(pin_address, current_value);
  reservePin(pin);
  setOwner(pin);
}

void Gpio::write(Pin pin, bool high) {
  const std::lock_guard<std::mutex> lock(gpio_lock_);
  if (!verifyInitialized()) {
    throw GiraffeException(DiagnosticId::INTERFACE_gpioNotInitialized,
                           std::to_string(pin.pin_number));
  }
  if (!isOwner(pin)) {
    throw GiraffeException(DiagnosticId::INTERFACE_gpioNotOwner,
                           std::to_string(pin.pin_number));
  }
  if (pin.mode != PinMode::OUTPUT) {
    throw GiraffeException(DiagnosticId::INTERFACE_gpioPinNotOutput,
                           std::to_string(pin.pin_number));
  }

  const uint32_t kPinOn = 0x1c;
  const uint32_t kPinOff = 0x28;

  uint8_t state = high ? kPinOn : kPinOff;
  writeWithBarrier(calculateAddress(state, pin.pin_number),
                   1 << pin.pin_number);
  gpio_lock_.unlock();
}

bool Gpio::read(Pin pin) {
  const std::lock_guard<std::mutex> lock(gpio_lock_);
  const uint32_t kReadPin = 0x34;

  if (!verifyInitialized()) {
    throw GiraffeException(DiagnosticId::INTERFACE_gpioNotInitialized,
                           std::to_string(pin.pin_number));
  }
  if (!isPinReserved(pin) || !isOwner(pin)) {
    throw GiraffeException(DiagnosticId::INTERFACE_gpioNotReserverOrNotOwner,
                           std::to_string(pin.pin_number));
  }
  if (pin.mode != PinMode::INPUT) {
    throw GiraffeException(DiagnosticId::INTERFACE_gpioCanNotReadOutputPin,
                           std::to_string(pin.pin_number));
  }

  uint32_t value = readWithBarrier(calculateAddress(kReadPin, pin.pin_number));
  uint8_t shift = pin.pin_number;
  return (value & (1 << shift)) ? 1 : 0;
}

bool Gpio::isPinReserved(const Pin &pin) {
  if (reserved_pins_.at(pin.pin_number).mode == PinMode::UNINITIALIZED &&
      reserved_pins_.at(pin.pin_number).pin_number == 0) {
    return false;
  }
  return true;
}

void Gpio::reservePin(const Pin &pin) {
  reserved_pins_.at(pin.pin_number) = pin;
}

bool Gpio::isOwner(const Pin &pin) {
  return pins_owned_ & (1 << pin.pin_number);
}

void Gpio::setOwner(const Pin &pin) {
  pins_owned_ |= (1 << pin.pin_number);
}

} // namespace giraffe