/**
 * @file utility-i2c.cpp
 * @author Joshua Jerred (github.com/joshua-jerred)
 * @brief Basic implementation of I2C interface class
 * @date 2022-10-31
 * @copyright Copyright (c) 2022
 * @version 0.0.9
 */

#include "utility-i2c.h"

I2C::I2C(): status_(I2C_STATUS::NOT_CONNECTED),
            file_descriptor_(-1) {
}

I2C::~I2C () {
    disconnect();
}

I2C_STATUS I2C::getStatus() {
    return status_;
}

int I2C::connect(uint8_t bus_number, uint8_t address) {
    if (bus_number < I2C_BUS_NUMBER_LOW || bus_number > I2C_BUS_NUMBER_HIGH) {
        status_ = I2C_STATUS::CONFIG_ERROR_BUS;
        return -1;
    }
    sprintf(file_name_, "/dev/i2c-%d", bus_number_);
    if (address < ADDRESS_LOW || address > ADDRESS_HIGH) {
        status_ = I2C_STATUS::CONFIG_ERROR_ADDRESS;
        return -1;
    }
    bus_number_ = bus_number;
    address_ = address;
    
    file_descriptor_ = open(file_name_, O_RDWR);

    if (file_descriptor_ < 0) {
        status_ = I2C_STATUS::BUS_ERROR;
        return -1;
    }

    if (ioctl(file_descriptor_, I2C_SLAVE, address_) < 0) {
        status_ = I2C_STATUS::ADDRESS_ERROR;
        return -1;
    }

    return 0;
}

int I2C::disconnect() {
    return close(file_descriptor_);
}

int I2C::writeByte(uint8_t data) {
    int response = i2c_smbus_write_byte(file_descriptor_, data);
    if (response < 0) {
        status_ = I2C_STATUS::WRITE_ERROR;
        return -1;
    }
    return 0;
}

int I2C::writeByteToReg(uint8_t data, uint8_t reg) {
    int response = i2c_smbus_write_byte_data(file_descriptor_, reg, data);
    if (response < 0) {
        status_ = I2C_STATUS::WRITE_ERROR;
        return -1;
    }
    return 0;
}

int16_t I2C::readByte() {
    uint8_t res = i2c_smbus_read_byte(file_descriptor_);
    if (res == 0) {
        status_ = I2C_STATUS::READ_ERROR;
        return 0x00;
    } else {
        return res;
    }
}

int16_t I2C::readByteFromReg(uint8_t reg_address) {
    int16_t res = i2c_smbus_read_byte_data(file_descriptor_, reg_address);
    if (res < 0) {
        return -1;
    } else {
        return res;
    }
}
