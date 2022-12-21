/**
 * @file utility-one-wire.h
 * @author Joshua Jerred (github.com/joshua-jerred)
 * @brief Header for the one wire utility class.
 * @date 2022-11-02
 * @copyright Copyright (c) 2022
 * @version 0.1.0
 */

#ifndef UTILITY_ONE_WIRE_H_
#define UTILITY_ONE_WIRE_H_

#include <fstream>
#include <string>
#include <filesystem>
#include <sstream>

#include "utility-configurables.h"
#include "utility-status.h"

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

#endif // UTILITY_ONE_WIRE_H_