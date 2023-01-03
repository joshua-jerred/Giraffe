/**
 * @file max17049.cpp
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief Driver for the MAX17049 fuel gauge IC
 * @date 2023-01-01
 * @copyright Copyright (c) 2023
 * @version 0.3
 */

#include <iostream> // DEBUG
#include <bitset> // DEBUG

#include "extensions.h"

#define MAX17049_ADDRESS 0x36

extension::MAX17049::MAX17049(
        DataStream *p_data_stream, 
        ExtensionMetadata extension_metadata) : 
        Extension(p_data_stream, extension_metadata),
        bus_number_(extension_metadata.extra_args.I2C_bus),
        device_address_(MAX17049_ADDRESS),
        i2c_bus_(I2C(bus_number_, device_address_, p_data_stream->getI2CBusLock())) {
    
}

extension::MAX17049::~MAX17049() {

}

int extension::MAX17049::runner() {
    // Connect to the I2C bus and configure the device address
	int result = i2c_bus_.connect();
	if (result != 0 || i2c_bus_.status() != I2C_STATUS::OK) {
		setStatus(ExtensionStatus::ERROR);

		I2C_STATUS status = i2c_bus_.status();
		if (status == I2C_STATUS::CONFIG_ERROR_BUS) {
			error("I2C_CB");
		} else if (status == I2C_STATUS::CONFIG_ERROR_ADDRESS) {
			error("I2C_CA");
		} else if (status == I2C_STATUS::BUS_ERROR) {
			error("I2C_BE");
		} else if (status == I2C_STATUS::ADDRESS_ERROR) {
			error("I2C_AE");
		} else {
			error("I2C_CU");
		}
		return -1;
	}

    if (handshake()) { // Good handshake
        setStatus(ExtensionStatus::RUNNING);
    } else {
        setStatus(ExtensionStatus::ERROR);
        error("HSK_F");
        return -1;	
    }
    setStatus(ExtensionStatus::STOPPED);
    return 0;
}

bool extension::MAX17049::handshake() {
    if (i2c_bus_.status() != I2C_STATUS::OK) {
        return false;
    }
    
    std::cout << "0x02: " << std::bitset<8>(i2c_bus_.readByteFromReg(0x02)) << std::endl;
    std::cout << "0x04: " << std::bitset<8>(i2c_bus_.readByteFromReg(0x04)) << std::endl;
    std::cout << "0x08: " << std::bitset<8>(i2c_bus_.readByteFromReg(0x08)) << std::endl;
    std::cout << "0x0A: " << std::bitset<8>(i2c_bus_.readByteFromReg(0x0A)) << std::endl;
    std::cout << "0x0C: " << std::bitset<8>(i2c_bus_.readByteFromReg(0x0C)) << std::endl;
    std::cout << "0x14: " << std::bitset<8>(i2c_bus_.readByteFromReg(0x14)) << std::endl;
    std::cout << "0x16: " << std::bitset<8>(i2c_bus_.readByteFromReg(0x16)) << std::endl;
    std::cout << "0x18: " << std::bitset<8>(i2c_bus_.readByteFromReg(0x18)) << std::endl;
    std::cout << "0x1A: " << std::bitset<8>(i2c_bus_.readByteFromReg(0x1A)) << std::endl;
    std::cout << "0xFE: " << std::bitset<8>(i2c_bus_.readByteFromReg(0xFE)) << std::endl;


    return true;
}