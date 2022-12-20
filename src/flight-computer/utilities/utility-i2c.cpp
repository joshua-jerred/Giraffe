/**
 * @file utility-i2c.cpp
 * @author Joshua Jerred (github.com/joshua-jerred)
 * @brief Basic implementation of I2C interface class
 * @cite https://manpages.debian.org/unstable/i2c-tools/libi2c.3.en.html
 * @cite https://www.kernel.org/doc/html/latest/i2c/dev-interface.html
 * @date 2022-12-19 - Modified
 * @date 2022-10-31
 * @copyright Copyright (c) 2022
 * @version 0.0.9
 */

#include "utility-i2c.h"

I2C::I2C(int bus_number, int address):
    bus_number_(bus_number),
    address_(address) {
    sprintf(file_name_, "/dev/i2c-%d", bus_number_);
}

I2C::~I2C () {
    disconnect();
}

I2C_STATUS I2C::status() {
    return status_;
}

int I2C::connect() {
    // Check if the bus number and address are valid
    if (bus_number_ < I2C_BUS_NUMBER_LOW || bus_number_ > I2C_BUS_NUMBER_HIGH) {
        status_ = I2C_STATUS::CONFIG_ERROR_BUS;
        return -1;
    } else if (address_ < ADDRESS_LOW || address_ > ADDRESS_HIGH) {
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
    if (ioctl(i2c_fd_, I2C_SLAVE, address_) < 0) {
        status_ = I2C_STATUS::ADDRESS_ERROR;
        return -1;
    }

    status_ = I2C_STATUS::OK;
    return 0;
}

int I2C::disconnect() {
    if (i2c_fd_ < 0) { // Check if the bus is open
        return 0;
    }
    return close(i2c_fd_);
}

int I2C::writeByte(uint8_t data) {
    if (i2c_fd_ < 0 || status_ != I2C_STATUS::OK) {
        return -1;
    }

    int32_t result = i2c_smbus_write_byte(i2c_fd_, data);
    
    if (result < 0) {
        status_ = I2C_STATUS::WRITE_ERROR;
        return -1;
    } else {
        return 0;
    }
}

int I2C::writeByteToReg(uint8_t data, uint8_t reg) {
    if (i2c_fd_ < 0 || status_ != I2C_STATUS::OK) {
        return -1;
    }

    int32_t result = i2c_smbus_write_byte_data(i2c_fd_, reg, data);

    return 0;
}

int I2C::readByte() {
    if (i2c_fd_ < 0 || status_ != I2C_STATUS::OK) {
        return -1;
    }

    int result = i2c_smbus_read_byte(i2c_fd_);
    
    if (result < 0) {
        status_ = I2C_STATUS::READ_ERROR;
        return -1;
    } else {
        return result;
    }
}

int I2C::readByteFromReg(uint8_t reg_address) {
    if (i2c_fd_ < 0 || status_ != I2C_STATUS::OK) {
        return -1;
    }

    int result = i2c_smbus_read_byte_data(i2c_fd_, reg_address);

    if (result < 0) {
        status_ = I2C_STATUS::READ_ERROR;
        return -1;
    } else {
        return result;
    }
}
