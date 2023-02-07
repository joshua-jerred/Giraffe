/**
 * @file mcp3021.cpp
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief Driver for the MCP3021
 * @date 2023-02-07
 * @copyright Copyright (c) 2023
 * @version 0.1
 */

#include <bitset>
#include "extensions.h"

#define MCP3021_ADDRESS 0x4D

extension::MCP3021::MCP3021(
        DataStream *p_data_stream, 
        ExtensionMetadata extension_metadata) : 
        Extension(p_data_stream, extension_metadata),
        bus_number_(extension_metadata.extra_args.I2C_bus),
        device_address_(MCP3021_ADDRESS),
        i2c_bus_(I2C(bus_number_, device_address_, p_data_stream->getI2CBusLock())) {
    
}

extension::MCP3021::~MCP3021() {

}

int extension::MCP3021::runner() {
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
    handshake();
    return -1;
}

bool extension::MCP3021::handshake() {
    uint8_t data[2] = {0, 0};
    int result = i2c_bus_.readChunk(data, 2);
    std::cout << "bits read: " << result << std::endl;
    std::cout << std::bitset<8>(data[0]) << " " << std::bitset<8>(data[1]) << std::endl;
    if (result != 2) {
        return false;
    }
    return true;
}
