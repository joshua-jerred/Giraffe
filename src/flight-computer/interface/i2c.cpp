/**
 * @file i2c.cpp
 * @author Joshua Jerred (https://joshuajer.red/)
 * @brief The wrapper for simplifying I2C communication, error handling, and 
 * making sure that only one thread is in a transaction at a time.
 * 
 * @version 0.3
 * @date 2023-01-06
 * @copyright Copyright (c) 2023
 */
#include <cstdint>
#include <cstdio>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <mutex>
extern "C" {
    #include <linux/i2c-dev.h>
    #include <i2c/smbus.h>
}

#include "configurables.h"
#include "interface.h"

interface::I2C::I2C(int bus_number, int address, std::mutex &bus_lock):
    status_(I2C_STATUS::NOT_CONNECTED), 
    bus_number_(bus_number),
    address_(address),
    i2c_fd_(-1),
    bus_lock_(bus_lock){

    sprintf(file_name_, "/dev/i2c-%d", bus_number_);

}

interface::I2C::~I2C () {
    disconnect();
}

interface::I2C_STATUS interface::I2C::status() {
    return status_;
}

int interface::I2C::connect() {
    // Check if the bus number and address are valid
    if (bus_number_ < I2C_BUS_NUMBER_LOW || bus_number_ > I2C_BUS_NUMBER_HIGH) {
        status_ = I2C_STATUS::CONFIG_ERROR_BUS;
        return -1;
    } else if (address_ < ADDRESS_LOW || address_ > ADDRESS_HIGH) {
        /** @todo consider a check for '& 0x01' as this bit should not be set */
        status_ = I2C_STATUS::CONFIG_ERROR_ADDRESS;
        return -1;
    }
    
    // Open the bus and check for errors
    // Possibly add a more specific check for error types
    i2c_fd_ = open(file_name_, O_RDWR);
    if (i2c_fd_ < 0) {
        status_ = I2C_STATUS::BUS_ERROR;
        return -1;
    }

    // Try to access the I2C device
    if (ioctl(i2c_fd_, I2C_SLAVE_FORCE, address_) < 0) { // SLAVE_FORCE incase a kernel driver is already using the device
        status_ = I2C_STATUS::ADDRESS_ERROR;
        return -1;
    }

    status_ = I2C_STATUS::OK;
    return 0;
}

int interface::I2C::disconnect() {
    if (i2c_fd_ < 0) { // Check if the bus is open
        return 0;
    }
    return close(i2c_fd_);
}

uint8_t interface::I2C::getAddress() {
    return address_ & 0xFF;
}

int32_t interface::I2C::writeByte(uint8_t data) {
    if (i2c_fd_ < 0 || status_ != I2C_STATUS::OK) {
        return -1;
    }
    bus_lock_.lock();
    volatile int32_t result = i2c_smbus_write_byte(i2c_fd_, data);
    bus_lock_.unlock();

    if (result < 0) {
        status_ = I2C_STATUS::WRITE_ERROR;
        return -1;
    } else {
        return 0;
    }
}

int32_t interface::I2C::writeByteToReg(uint8_t reg_address, uint8_t data) {
    if (i2c_fd_ < 0 || status_ != I2C_STATUS::OK) {
        return -1;
    }
    bus_lock_.lock();
    volatile int32_t result = i2c_smbus_write_byte_data(i2c_fd_, reg_address, data);
    bus_lock_.unlock();
    if (result < 0) {
        status_ = I2C_STATUS::WRITE_ERROR;
        return -1;
    } else {
        return 0;
    }
    return result;
}

int interface::I2C::writeChunk(uint8_t* data, uint8_t length) {
    if (i2c_fd_ < 0 || status_ != I2C_STATUS::OK) {
        return -1;
    }

    bus_lock_.lock();
    int bytes_written = write(i2c_fd_, data, length);
    bus_lock_.unlock();

    if (bytes_written < 0) {
        status_ = I2C_STATUS::WRITE_ERROR;
        return -1;
    } else {
        return bytes_written;
    }
}

int32_t interface::I2C::readByte() {
    if (i2c_fd_ < 0 || status_ != I2C_STATUS::OK) {
        return -1;
    }

    bus_lock_.lock();
    volatile int32_t result = i2c_smbus_read_byte(i2c_fd_);
    bus_lock_.unlock();

    if (result < 0) {
        status_ = I2C_STATUS::READ_ERROR;
        return -1;
    } else {
        return result;
    }
}

int32_t interface::I2C::readByteFromReg(uint8_t reg_address) {
    if (i2c_fd_ < 0 || status_ != I2C_STATUS::OK) {
        return -1;
    }

    bus_lock_.lock();
    volatile int32_t result = i2c_smbus_read_byte_data(i2c_fd_, reg_address);
    bus_lock_.unlock();

    if (result < 0) {
        status_ = I2C_STATUS::READ_ERROR;
        return -1;
    } else {
        return result;
    }
}

int interface::I2C::readChunkFromReg(uint8_t* data, int length, uint8_t reg_address) {
    if (i2c_fd_ < 0 || status_ != I2C_STATUS::OK) {
        return -1;
    }

    bus_lock_.lock();
    int status = write(i2c_fd_, &reg_address, 1);
    if (status < 0) {
        status_ = I2C_STATUS::WRITE_ERROR;
        bus_lock_.unlock();
        return -1;
    }

    int bytes_read = read(i2c_fd_, data, length);
    if (bytes_read < 0) {
        status_ = I2C_STATUS::READ_ERROR;
        bus_lock_.unlock();
        return -1;
    } else {
        bus_lock_.unlock();
        return bytes_read;
    }
}

int interface::I2C::readChunk(uint8_t* data, int length) {
    if (i2c_fd_ < 0 || status_ != I2C_STATUS::OK) {
        return -1;
    }

    bus_lock_.lock();
    int bytes_read = read(i2c_fd_, data, length);
    bus_lock_.unlock();

    if (bytes_read < 0) {
        status_ = I2C_STATUS::READ_ERROR;
        return -1;
    } else {
        return bytes_read;
    }
}