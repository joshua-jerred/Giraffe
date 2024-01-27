/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   spi_interface.hpp
 * @brief  Contains the SPI interface class for the Giraffe flight system.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-01-26
 * @copyright  2024 (license to be defined)
 */

#ifndef SPI_INTERFACE_HPP_
#define SPI_INTERFACE_HPP_

#include <cstdint>
#include <string>

namespace giraffe {

class SpiInterface {
public:
  struct Config {
    uint8_t bits_per_word;
    uint32_t speed_hz;
    uint16_t delay_usecs;
  };

  /**
   * @brief Constructor for the SPI interface class.
   */
  SpiInterface(Config config) : config_(config) {
  }

  /**
   * @brief Destructor for the SPI interface class. Closes the file descriptors
   * if they are open.
   */
  ~SpiInterface() {
    closeDevice();
  }

  /**
   * @brief Attempts to open the SPI device. Returns true if successful, false
   * otherwise.
   * @note This function will call giraffe_assert if the device is already open.
   * @param device_path - The path to the SPI device. Example: "/dev/spidev0.1"
   * @return true If the device was opened successfully.
   * @return false If the device was not opened successfully.
   */
  bool openDevice(std::string device_path);

  /**
   * @brief Closes the SPI device. No action is taken if the device is already
   * closed.
   */
  void closeDevice();

  /**
   * @brief Returns true if the SPI device is open, false otherwise.
   * @return true The SPI device is open.
   * @return false The SPI device is closed.
   */
  bool isOpen() const {
    return spi_fd_ != -1;
  }

  /**
   * @brief SPI transfer function. Returns true if the transfer was successful,
   * false otherwise.
   *
   * @param tx_buffer The buffer to transmit (must be at least transfer_size)
   * @param rx_buffer The buffer to receive (must be at least transfer_size)
   * @param transfer_size The number of bytes to transfer
   * @return true The transfer was successful
   * @return false The transfer was not successful
   */
  bool transfer(uint8_t *tx_buffer, uint8_t *rx_buffer, size_t transfer_size);

private:
  // const std::string device_path_{};
  int spi_fd_ = -1;

  Config config_{};
};

} // namespace giraffe

#endif /* SPI_INTERFACE_HPP_ */