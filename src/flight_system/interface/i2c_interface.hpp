/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   i2c_interface.hpp
 * @brief  A wrapper around I2C communication.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-07-01
 * @copyright  2023 (license to be defined)
 */

#ifndef I2C_INTERFACE_HPP_
#define I2C_INTERFACE_HPP_

#include <cstdint>
#include <mutex>
#include <vector>

#include "configuration_enums.hpp"
#include "giraffe_diagnostics.hpp"

/**
 * @brief A wrapper around I2C communication that restricts a single thread
 * from accessing the bus at a time.
 */
class I2cInterface {
public:
  /**
   * @brief Construct a new I2cInterface instance.
   *
   * @param bus - The bus to connect to.
   * @param address - The address of the device on the bus.
   * @param bus_lock - A common mutex to lock the bus with.
   */
  I2cInterface(cfg::gEnum::I2CBus bus, uint8_t address, std::mutex &bus_lock);

  /**
   * @brief Destroy the I2cInterface instance. Disconnects from the bus if it's
   * open.
   */
  ~I2cInterface();

  /**
   * @brief Return codes for I2cInterface methods.
   */
  enum class Result : int {
    /**
     * @brief Generic success code.
     */
    SUCCESS = 0,
    BUS_OPEN_ERROR = 1,
    ADDRESS_SET_ERROR = 2,

    /**
     * @brief The file descriptor is not open. This is likely because the bus
     * has not been connected to.
     */
    FD_NOT_OPEN = 3,
    DISCONNECT_CLOSE_ERROR = 4,
    WRITE_ERROR = 5,

    /**
     * @brief If the number of bytes to be written is not equal to the number
     * of bytes that were actually written, this error is returned.
     */
    WRITE_ERROR_NUM_BYTES = 6,

    /**
     * @brief Passing an empty vector as a C array is undefined behavior.
     */
    DATA_SIZE_ERROR = 6,
    READ_ERROR = 7,
    READ_ERROR_WRITE_REG = 8,
    READ_ERROR_NUM_BYTES

  };

  /**
   * @brief Connect to the bus. If it's already connected, disconnect and
   * reconnect.
   * @return Result - 'SUCCESS' if successful, otherwise an error code.
   */
  Result connect();

  /**
   * @brief Disconnect from the bus. Reports an error if the bus is not
   * connected.
   * @return Result - 'SUCCESS' if successful, otherwise an error code.
   */
  Result disconnect();

  /**
   * @brief Disconnect from the bus if it's connected, otherwise do nothing.
   * @details Will not report any error. This is really just a safe way to
   * reset the connection.
   */
  void reset();

  /**
   * @brief Get the address of the device on the bus.
   *
   * @return uint8_t - The address of the device on the bus.
   */
  uint8_t getAddress() const;

  //&&&&&&&&&&&& Write Methods &&&&&&&&&&&& //

  /**
   * @brief Write a single byte to the device without specifying a register.
   *
   * @param data_byte - The byte to write.
   * @return Result - 'SUCCESS' if successful, otherwise an error code.
   */
  Result writeByte(uint8_t data_byte);

  /**
   * @brief Write a single byte to the device at the specified register.
   * @details This will first write the register address, then the data byte.
   *
   * @param data_byte - The byte to write.
   * @param register_address - The register address to write to.
   * @return Result - 'SUCCESS' if successful, otherwise an error code.
   */
  Result writeByteToReg(uint8_t data_byte, uint8_t register_address);

  /**
   * @brief Write a chunk of data to the device at the specified register.
   *
   * @param data - The data to write.
   * @return Result - 'SUCCESS' if successful, otherwise an error code.
   */
  Result writeChunk(std::vector<uint8_t> &data);

  //&&&&&&&&&&&& Read Methods &&&&&&&&&&&& //

  /**
   * @brief Read a single byte from the device without specifying a register.
   *
   * @param data_byte (out) - The byte read from the device.
   * @return Result - 'SUCCESS' if successful, otherwise an error code.
   */
  Result readByte(uint8_t &data_byte);

  /**
   * @brief Read a single byte from the device at the specified register.
   *
   * @param data_byte (out) - The byte read from the device.
   * @param register_address - The register address to read from.
   * @return Result - 'SUCCESS' if successful, otherwise an error code.
   */
  Result readByteFromReg(uint8_t &data_byte, uint8_t register_address);

  /**
   * @brief Read a chunk of data from the device at the specified register.
   * @details - The 'data' vector will be cleared and resized to 'num_bytes'.
   *
   * @param data (out) - The data read from the device.
   * @param length - The number of bytes to read.
   * @param register_address - The register address to read from.
   * @return Result - 'SUCCESS' if successful, otherwise an error code.
   */
  Result readChunkFromReg(std::vector<uint8_t> &data, uint8_t register_address,
                          size_t num_bytes);

  /**
   * @brief Read a chunk of data from the device without specifying a register.
   * @details - The 'data' vector will be cleared and resized to 'num_bytes'.
   *
   * @param data (out) - The data read from the device.
   * @param num_bytes - The number of bytes to read.
   * @return Result - 'SUCCESS' if successful, otherwise an error code.
   */
  Result readChunk(std::vector<uint8_t> &data, size_t num_bytes);

private:
  /**
   * @brief Checks the file descriptor to see if it's connected to the bus.
   *
   * @return true - The file descriptor is open.
   * @return false - The file descriptor is not open.
   */
  bool isConnected() const;

  /**
   * @brief The bus to connect to. This will be translated to a file name.
   */
  const cfg::gEnum::I2CBus bus_;

  /**
   * @brief The address of the device on the bus to connect to.
   */
  const uint8_t address_;

  /**
   * @brief A common mutex to lock the bus with.
   */
  std::mutex &bus_lock_;

  /**
   * @brief The file name of the bus to connect to.
   */
  std::string file_name_{};

  /**
   * @brief The file descriptor of the bus.
   */
  int i2c_fd_ = -1;
};

#endif /* I2C_INTERFACE_HPP_ */