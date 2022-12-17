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

#include "utility-configurables.h"

class OneWire {
    public:
        OneWire(std::string device_id);
        ~OneWire();
        std::string read();
        int checkDevice();

    private:
        std::string path_;
        std::ifstream file_;
};

#endif // UTILITY_ONE_WIRE_H_