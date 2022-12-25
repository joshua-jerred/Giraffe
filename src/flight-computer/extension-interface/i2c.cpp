
#include <cstdint>
#include <cstdio>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
extern "C" {
    #include <linux/i2c-dev.h>
    #include <i2c/smbus.h>
}

#include "utility-configurables.h"
#include "extension-interface.h"

I2C::I2C(int bus_number, int address):
    status_(I2C_STATUS::NOT_CONNECTED), 
    bus_number_(bus_number),
    address_(address),
    i2c_fd_(-1){

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
    if (ioctl(i2c_fd_, I2C_SLAVE_FORCE, address_) < 0) { // SLAVE_FORCE incase a kernel driver is already using the device
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

int32_t I2C::writeByte(uint8_t data) {
    if (i2c_fd_ < 0 || status_ != I2C_STATUS::OK) {
        return -1;
    }

    volatile int32_t result = i2c_smbus_write_byte(i2c_fd_, data);
    
    if (result < 0) {
        status_ = I2C_STATUS::WRITE_ERROR;
        return -1;
    } else {
        return 0;
    }
}

int32_t I2C::writeByteToReg(uint8_t reg_address, uint8_t data) {
    if (i2c_fd_ < 0 || status_ != I2C_STATUS::OK) {
        return -1;
    }

    volatile int32_t result = i2c_smbus_write_byte_data(i2c_fd_, reg_address, data);
    if (result < 0) {
        status_ = I2C_STATUS::WRITE_ERROR;
        return -1;
    } else {
        return 0;
    }
    return result;
}

int32_t I2C::readByte() {
    if (i2c_fd_ < 0 || status_ != I2C_STATUS::OK) {
        return -1;
    }

    volatile int32_t result = i2c_smbus_read_byte(i2c_fd_);
    
    if (result < 0) {
        status_ = I2C_STATUS::READ_ERROR;
        return -1;
    } else {
        return result;
    }
}

int32_t I2C::readByteFromReg(uint8_t reg_address) {
    if (i2c_fd_ < 0 || status_ != I2C_STATUS::OK) {
        return -1;
    }

    volatile int32_t result = i2c_smbus_read_byte_data(i2c_fd_, reg_address);

    if (result < 0) {
        status_ = I2C_STATUS::READ_ERROR;
        return -1;
    } else {
        return result;
    }
}
