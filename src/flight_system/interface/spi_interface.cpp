/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   spi_interface.cpp
 * @brief  Implementation of the SPI interface class for the Giraffe flight
 * system.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-01-26
 * @copyright  2024 (license to be defined)
 */

#include "spi_interface.hpp"
#include "giraffe_assert.hpp"

#include <fcntl.h>
#include <linux/spi/spidev.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

namespace giraffe {

bool SpiInterface::openDevice(std::string device_path) {
  giraffe_assert(spi_fd_ == -1);
  if ((spi_fd_ = open(device_path.c_str(), O_RDWR)) >= 0) {
    return true;
  }

  spi_fd_ = -1;
  return false;
}

void SpiInterface::closeDevice() {
  if (spi_fd_ != -1) {
    close(spi_fd_);
    spi_fd_ = -1;
  }
}

bool SpiInterface::transfer(uint8_t *tx_buffer, uint8_t *rx_buffer,
                            size_t transfer_size) {
  giraffe_assert(spi_fd_ != -1);

  spi_ioc_transfer spi_transfer;
  memset(&spi_transfer, 0, sizeof(spi_transfer));
  spi_transfer.tx_buf = (unsigned long)tx_buffer;
  spi_transfer.rx_buf = (unsigned long)rx_buffer;
  spi_transfer.len = transfer_size;
  spi_transfer.delay_usecs = config_.delay_usecs;
  spi_transfer.speed_hz = config_.speed_hz;
  spi_transfer.bits_per_word = config_.bits_per_word;

  return (ioctl(spi_fd_, SPI_IOC_MESSAGE(1), &spi_transfer) >= 0);
}

} // namespace giraffe