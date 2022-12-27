/**
 * @file samm8q.cpp
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief Extension for the SAM M8Q GPS module
 * @date 2022-12-25
 * @copyright Copyright (c) 2022
 * @version 0.1
 */

#include "extensions.h"

#define SAMM8Q_I2C_ADDRESS 0x42

SAMM8Q::SAMM8Q(DataStream *p_data_stream, ExtensionMetadata extension_metadata) :
    Extension(p_data_stream, extension_metadata),
    bus_number_(extension_metadata.extra_args.I2C_bus),
    device_address_(SAMM8Q_I2C_ADDRESS),
    i2c_bus_(I2C(bus_number_, device_address_)) {
    
}


SAMM8Q::~SAMM8Q() {

}

int SAMM8Q::runner() {
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

    // Configure the device to be UBX only and check for ACK

    // Set the frequency of the messages

    // Set the internal measurement rate

    // Change the dynamic model (high altitude), this provides for high altitude
    // altitude data, as long as we stay below 1G of acceleration
    return 0;
}

bool SAMM8Q::configure() {
    // Configure the device to be UBX only, check for ACK

    // Set the frequency of the messages, check for ACK

    // Set the internal measurement rate, check for ACK

    // Change the dynamic model (high altitude), this provides for high altitude
    // altitude data, as long as we stay below 1G of acceleration, check for ACK
    
    // Read the configuration data and verify
    return true;
}

