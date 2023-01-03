#ifndef EXTENSION_INTERFACE_H_
#define EXTENSION_INTERFACE_H_

#include <string>
#include <mutex>

#include "utility-status.h"
#include "utility-configurables.h"


/*
class InterfaceControl {
public:
    InterfaceControl() {
        i2c_mutex_.unlock();
        spi_mutex_.unlock();
        uart_mutex_.unlock();
    };
    ~InterfaceControl() {
        i2c_mutex_.lock();
        spi_mutex_.lock();
        uart_mutex_.lock();
    };

    int lock_i2c() {
        i2c_mutex_.lock();
        return 0;
    };
    int unlock_i2c() {
        i2c_mutex_.unlock();
        return 0;
    };

    int lock_spi() {
        spi_mutex_.lock();
        return 0;
    };
    int unlock_spi() {
        spi_mutex_.unlock();
        return 0;
    };

    int lock_uart() {
        uart_mutex_.lock();
        return 0;
    };
    int unlock_uart() {
        uart_mutex_.unlock();
        return 0;
    };

private:
    std::mutex i2c_mutex_;
    std::mutex spi_mutex_;
    std::mutex uart_mutex_;
    std::mutex one_wire_mutex_;
};
*/


class I2C {
public:
    I2C(int bus_number, int address, std::mutex &bus_lock);
    ~I2C();

    int connect();
    int disconnect();

    I2C_STATUS status();

    uint8_t getAddress();

    int writeByte(uint8_t data);
    int writeByteToReg(uint8_t reg_address, uint8_t data);
    int writeChunk(uint8_t* data, uint8_t length);

    int readByte();
    int readByteFromReg(uint8_t reg_address);
    int readChunkFromReg(uint8_t* data, int length, uint8_t reg_address);

private:
    I2C_STATUS status_;

    int bus_number_;
    int address_;
    char file_name_[20];
    int i2c_fd_;
    std::mutex &bus_lock_;
};

class OneWire {
    public:
        OneWire(std::string device_id);
        ~OneWire();

        ONEWIRE_STATUS status();

        std::string read_w1_slave();
        std::string read_temperature();


    private:
        int checkDevice();
        std::string path_; // path to the device directory
        ONEWIRE_STATUS status_;
};

#endif /* UTILITY_INTERFACE_CONTROL_H_ */