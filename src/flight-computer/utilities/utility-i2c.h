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
#include "utility-status.h"

class I2C {
public:
    I2C(int bus_number, int address);
    ~I2C();

    int connect();
    int disconnect();

    I2C_STATUS status();

    int writeByte(uint8_t data);
    int writeByteToReg(uint8_t data, uint8_t reg);

    int readByte();
    int readByteFromReg(uint8_t reg_address);

private:
    I2C_STATUS status_;

    int bus_number_;
    int address_;
    char file_name_[20];
    int i2c_fd_;
};

#endif /* UTILITY_I2C_H_ */