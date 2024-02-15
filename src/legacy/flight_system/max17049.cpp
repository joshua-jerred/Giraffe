/**
 * @file max17049.cpp
 * @author Joshua Jerred (https://joshuajer.red/)
 * @brief Driver for the MAX17049 fuel gauge, currently unused.
 *
 * @version 0.3
 * @date 2023-01-06
 * @copyright Copyright (c) 2023
 *
 * @todo consider removing this extension all together
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
        i2c_bus_(interface::I2C(bus_number_, device_address_, p_data_stream->getI2CBusLock())) {

}

extension::MAX17049::~MAX17049() {

}

int extension::MAX17049::runner() {
    // Connect to the I2C bus and configure the device address
	int result = i2c_bus_.connect();
	if (result != 0 || i2c_bus_.status() != interface::I2C_STATUS::OK) {
		setStatus(ExtensionStatus::ERROR);

		interface::I2C_STATUS status = i2c_bus_.status();
		if (status == interface::I2C_STATUS::CONFIG_ERROR_BUS) {
			error("I2C_CB");
		} else if (status == interface::I2C_STATUS::CONFIG_ERROR_ADDRESS) {
			error("I2C_CA");
		} else if (status == interface::I2C_STATUS::BUS_ERROR) {
			error("I2C_BE");
		} else if (status == interface::I2C_STATUS::ADDRESS_ERROR) {
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
    while (!stop_flag_) {
        if (readData()) {

        }
        std::this_thread::sleep_for(std::chrono::milliseconds(getUpdateInterval()));
    }
    setStatus(ExtensionStatus::STOPPED);
    return 0;
}

bool extension::MAX17049::handshake() {
    if (i2c_bus_.status() != interface::I2C_STATUS::OK) {
        return false;
    }

    uint8_t data[2];
    if (i2c_bus_.readChunkFromReg(data, 2, 0x08) != 2) {// Version Register
        return false;
    }

    if (i2c_bus_.status() != interface::I2C_STATUS::OK) {
        return false;
    } else if (data[0] != 0x00 || data[1] >> 4 != 0x01) { // Version - 0x001_
        return false;
    }

    return true;
}

bool extension::MAX17049::readData() {
    if (i2c_bus_.status() != interface::I2C_STATUS::OK) {
        return false;
    }

    uint8_t buffer[2];
    if (i2c_bus_.readChunkFromReg(buffer, 2, 0x02) != 2) { // Read the voltage (VCELL, 0x02)
        return false;
    }

    uint16_t v = (buffer[0] << 8 | buffer[1]) >> 4;
    float voltage = (float)v * 78.125 / 1000000; // 78.125 uV per bit
    std::cout << "V: " << v << " " << voltage << std::endl;
    return true;
}