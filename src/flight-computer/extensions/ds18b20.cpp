/**
 * @file ds18b20.cpp
 * @author Joshua Jerred (github.com/joshua-jerred)
 * @brief Implementation of the DSB18B20 class.
 * @date 2022-11-02
 * @copyright Copyright (c) 2022
 * @version 0.1.0
 */

#include "ds18b20.h"

DS18B20::DS18B20(DataStream *p_data_stream, 
                ExtensionMetadata extension_metadata)
			   : Extension(p_data_stream, extension_metadata),
			   one_wire_device_id_(extension_metadata.extra_args.one_wire_id),
               one_wire_device_(one_wire_device_id_) {
}

DS18B20::~DS18B20() {
	
}

int DS18B20::runner() {
    if (one_wire_device_.status() != ONEWIRE_STATUS::OK) {
        setStatus(ExtensionStatus::ERROR);
        return -1;
    } else {
        setStatus(ExtensionStatus::RUNNING);
    }
    while(!stop_flag_) {
        if (readData() == 0) {
            sendData("TEMP_C", temp_C_);
            sendData("TEMP_F", temp_F_);
        }
        std::this_thread::sleep_for(
            std::chrono::milliseconds(getUpdateInterval())
            );
    }
	return 0;
}

int DS18B20::readData() {
	raw_data_ = one_wire_device_.read_temperature(); // returns empty string on failure
	
	if (raw_data_.size() == 0) {
		return -1;
	}

	int tempC = std::stoi(one_wire_device_.read_temperature());
	temp_C_ = (float)tempC / 1000.0;
	temp_F_ = (temp_C_ * 9.0 / 5.0) + 32.0;
	return 0;
}


/**
 * @details
 * A lot of this code is unneeded as the Pi already has drivers for the DS18B20.
 * (Linux 1-wire drivers)
 * I trust this more than using the 1-wire protocol manually and there is
 * added protection when running this in a threaded environment.
 * 
 * I'm going to leave the code below but it will not be used.
 * Anything below is from the previous implementation.
 *------------------------------------------------------------------------------
 * PREVIOUS DOCUMENTATION BELOW
 *------------------------------------------------------------------------------
 * This is a thorough implementation that included the CRC check.
 *
 * Example Data Format when reading from 1-wire slave device on linux systems
 * "ca 01 4b 46 7f ff 06 10 65 : crc=65 YES\nca 01 4b 46 7f ff 06 10 65 t=28625"
 * There are two lines of output, both contain identical data.
 * Because we can we will calculate the temperature and check the CRC ourselves
 * so the only thing that needs to be pulled from this data is the first 
 * 26 characters which contains 9 bytes in hexadecimal format.
 * So, the data we want looks like this: "ca 01 4b 46 7f ff 06 10 65"
 *
 * Actual Format with bytes labeled just as they are in the data sheet:
 * [   0    ][   1    ][       2-4        ][    5    ][    6   ][    7    ][ 8 ] - Bytes
 * [   ca   ][   01   ][     4b 46 7f     ][    ff   ][    06  ][    10   ][ 65] - Example Data
 * [TEMP LSB][TEMP MSB][Th, Tl, config reg][const FFh][reserved][const 10h][CRC] - Format
 *
 * To get the temperature combine byte 0 (LSB) and 1 (MSB) in proper order 
 * (flip them) [1][0].
 * [0] = ca = 1100 1010
 * [1] = 01 = 0000 0001
 * [1][0] = 01 ca = 0000 0001 1100 1010
 * The following is only applicable if the sensor is configured to 12-bit
 * resolution! This is the default mode. Refer to the data sheet under 
 * "Operation—Measuring Temperature"
 *
 * Bit Num | 7 | 6 | 5 | 4 | 3  | 2  | 1  | 0  |
 *		  -------------------------------------
 * MS Byte | S | S | S | S | S  |2^6 |2^5 |2^4 |
 * LS Byte |2^3|2^2|2^1|2^0|2^-1|2^-2|2^-3|2^-4|
 * 
 * It's just a signed two's compliment number with sign extension.
 * Looking at bit 3-0 of the LSB we can see that we need to divide whatever
 * this number is by 16 to get the answer in degrees C.
 * Examples:
 * 0000 0111 1101 0000 = +2000 | +2000 / 16 = +125     degrees C
 * 0000 0101 0101 0000 = +1360 | +1360 / 16 = +85      degrees C <-- ~*~
 * 1111 1111 1111 1000 = -8    | -8    / 16 = -0.5     degrees C
 * 1111 1110 0110 1111 = -401  | -401  / 16 = -25.0625 degrees C
 * ~*~ --> +85C is the power on reset value! If you get this, read the sensor again.
*/
/*
int DS18B20::processData() {
	raw_data_ = raw_data_.substr(0, raw_data_.find(":")); // remove everything except for the 9 hex bytes
	if (raw_data_.size() != 27) { // Data Error, should be 27 chars long (18 hex chars + 9 spaces)
		error("DS18B20_DL", raw_data_);
		return -1;
	}
	int charCount = 0;
	int i, j;
	for (i = 0; i < 27; i++) { // Verify that they are hex values and that there are 18 of them
		if (raw_data_[i] == ' ') {
			continue;
		} else if ((raw_data_[i] >= 48) && (raw_data_[i] <= 57)) { // 0-9
			charCount++;
			continue;
		} else if ((raw_data_[i] >= 97) && (raw_data_[i] <= 102)) { // a-f
			charCount++;
			continue;
		} else if ((raw_data_[i] >= 65) && (raw_data_[i] <= 70)) { // A-F
			charCount++;
			continue;
		} else { // unknown char
			error("DS18B20_PE1", raw_data_ + " Char: " + raw_data_[i] + " charCount: " + std::to_string(charCount));
			return -1;
		}
	}
	if (charCount != 18) { // incorrect number of hex values
		error("DS18B20_PE2", raw_data_);
		return -1;
	}
	uint8_t convertedData[9]; // array that holds the data that has been converted from string hex values to numerical values.
	std::string slice; // holds single byte values (two hex chars)
	j = 0;
	for (i = 0; i <= 24; i += 3) { // iterates through each pair of hex values (each byte)
		slice = raw_data_.substr(i, 2); // pulls a hex byte at a time
		convertedData[j] = std::stoul(slice, nullptr, 16);
		j++;
	}
	if (calculateCRC(convertedData, 8) != convertedData[8]) {
		error("DS18B20_CRC", raw_data_);
		return -1;
	}
	raw_temp_ = ((uint16_t)convertedData[1] << 8) | (uint16_t)convertedData[0]; // Raw temperature value
	temp_C_ = (float) raw_temp_ / 16.0; // calculated celsius value
	temp_F_ = (temp_C_ * 1.8) + 32; // convert c to f
	return 0;
}

uint8_t DS18B20::calculateCRC(uint8_t *addr, uint8_t len) {
    uint8_t crc = 0;  
    while (len--) {
            uint8_t inbyte = *addr++;
            for (uint8_t i = 8; i; i--) {
                    uint8_t mix = (crc ^ inbyte) & 0x01;
                    crc >>= 1;
                    if (mix) crc ^= 0x8C;
                    inbyte >>= 1;
            }
    }
    return crc;
}
*/