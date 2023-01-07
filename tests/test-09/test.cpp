/**
 * @file test.cpp
 * @author Joshua Jerred (https://joshuajer.red/)
 * @brief u-blox UBX Manual Test
 * @date 2022-12-26
 * @copyright Copyright (c) 2022
 * @version 0.1.0
 */

#include <iostream>
#include <unordered_map>
#include <string>
#include <unistd.h>
#include <thread>
#include <chrono>
#include <mutex>

#include "ubx.h"

#define BUS_NUMBER 1
#define DEVICE_ADDRESS 0x42

int main() {
    std::mutex i2c_mutex = std::mutex();

    static const uint8_t kNavClass = 0x01;
    static const uint8_t kNavPvt = 0x07; // Position Velocity Time Solution ID

    I2C i2c(BUS_NUMBER, DEVICE_ADDRESS, i2c_mutex);
    i2c.connect();
    if (i2c.status() != I2C_STATUS::OK) {
        std::cout << "I2C connection failed";
        return 1;
    } 

    if (ubx::getStreamSize(i2c) != 0) {
        ubx::sendResetCommand(i2c);
        std::this_thread::sleep_for(std::chrono::milliseconds(1500));
        while (i2c.readByteFromReg(0xFF) != 0xFF) { /** @todo this needs a timeout*/
            if (i2c.status() != I2C_STATUS::OK) {
                std::cout << "I2C connection failed";
                return 1;
            }
        }
    }

    ubx::ACK ack = ubx::setProtocolDDC(i2c, false);
    if (ack != ubx::ACK::ACK) {
        std::cout << "Failed to set DDC protocol";
        return 1;
    }

    //ack = ubx::setMessageRate(i2c, kNavClass, kNavPvt, 1);
    //if (ack != ubx::ACK::ACK) {
    //    std::cout << "Failed to set message rate";
    //    return 1;
    //}

    ack = ubx::setMeasurementRate(i2c, 50);
    if (ack != ubx::ACK::ACK) {
        std::cout << "Failed to set measurement rate";
        return 1;
    }

    ubx::DYNAMIC_MODEL dynamic_model = ubx::DYNAMIC_MODEL::AIRBORNE_1G;
    ack = ubx::setDynamicModel(i2c, dynamic_model);
    if (ack != ubx::ACK::ACK) {
        std::cout << "Failed to set dynamic model";
        return 1;
    }

    ubx::UBXMessage msg;
    ubx::NAV_DATA pvt;
    int total_attempts = 0;
    int success_attempts = 0;
    while (true) {
        /*
        if (ubx::readNextUBX(i2c, msg)) {
            if (msg.mClass == kNavClass && msg.mID == kNavPvt) {
                if (msg.verifyChecksum() == false) {
                    std::cout << "-----ERROR START----- ss:";
                    std::cout << ubx::getStreamSize(i2c) << std::endl;
                    std::cout << msg << std::endl;
                    ubx::parsePVT(msg, pvt);
                    std::cout << pvt << std::endl;
                    std::cout << "------ERROR END-----" << std::endl;
                    continue;
                }
                ubx::parsePVT(msg, pvt);
                std::cout << pvt << std::endl;
            }
            else {
                std::cout << msg << std::endl;
            }
        } 
        */


        total_attempts++;
        if (ubx::pollMessage(i2c, msg, kNavClass, kNavPvt, 92 + 8, 2000)) {
            success_attempts++;
            ubx::parsePVT(msg, pvt);
            std::cout << pvt << std::endl;
        } else {
            std::cout << "poll failed" << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            ubx::flushStream(i2c);
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        //std::cout << (float)success_attempts / (float)total_attempts << std::endl;
    }

    return 0;
}