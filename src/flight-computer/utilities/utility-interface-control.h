/**
 * @file utility-interface-control.h
 * @author Joshua Jerred (github.com/joshua-jerred)
 * @brief 
 * @date 2022-10-31
 * @copyright Copyright (c) 2022
 * @version 0.0.9
 */

#ifndef UTILITY_INTERFACE_CONTROL_H_
#define UTILITY_INTERFACE_CONTROL_H_

#include <mutex>

#include "utility-data-stream.h"

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

#endif /* UTILITY_INTERFACE_CONTROL_H_ */