/**
 * @file utility-i2c.h
 * @author Joshua Jerred (github.com/joshua-jerred)
 * @brief Basic I2C interface class
 * @date 2022-10-31
 * @copyright Copyright (c) 2022
 * @version 0.0.9
 */
#ifndef UTILITY_I2C_H_
#define UTILITY_I2C_H_



#define ADDRESS_LOW 0x03
#define ADDRESS_HIGH 0x77

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

enum class I2C_STATUS {
    NOT_CONNECTED,
    OK,
    CONFIG_ERROR_BUS,
    CONFIG_ERROR_ADDRESS,
    BUS_ERROR,
    ADDRESS_ERROR,
    READ_ERROR,
    WRITE_ERROR,
    UNKNOWN_ERROR
};

class I2C {
public:
    I2C();
    ~I2C();

    I2C_STATUS getStatus();

    int connect(uint8_t bus_number, uint8_t address);
    int disconnect();

    int writeByte(uint8_t data);
    int writeByteToReg(uint8_t data, uint8_t reg);

    uint8_t readByte();
    uint8_t readByteFromReg(uint8_t reg_address);

private:
    
    uint8_t bus_number_;
    uint8_t address_;
    
    I2C_STATUS status_;
    
    char file_name_[20];
    int file_descriptor_;

    char buffer_[2];
};

#endif /* UTILITY_I2C_H_ */