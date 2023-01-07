/**
 * @file bmp180.cpp
 * @author Joshua Jerred (https://joshuajer.red/)
 * @brief This file contains the implementation of the BMP180 extension.
 * 
 * @cite https://cdn-shop.adafruit.com/datasheets/BST-BMP180-DS000-09.pdf
 * @date 2022-11-01
 * @copyright Copyright (c) 2022
 * @version 0.1.0
 * 
 * @todo No need for the macros here
 */

#include <string>
#include <sstream>
#include <cmath>
#include <unistd.h>
#include <bitset>
#include <unordered_map>

#include "extensions.h"

// Configurable
#define SAMPLING_ACCURACY 1 // Refer to BMP180 data sheet "oversampling_setting", if this is changed BMP180_READPRESSURECMD must also 
#define BMP180_DEBUG 0 // If set to 1, the extension will push debug data to the data stream (not recommended for flight)

// BMP180 Registers Read Only
#define REG_CAL_AC1 0xAA // Calibration Data starting register (0xAA-0xBF)
#define REG_CAL_AC2 0xAC 
#define REG_CAL_AC3 0xAE 
#define REG_CAL_AC4 0xB0 
#define REG_CAL_AC5 0xB2 
#define REG_CAL_AC6 0xB4 
#define REG_CAL_B1 0xB6
#define REG_CAL_B2 0xB8
#define REG_CAL_MB 0xBA
#define REG_CAL_MC 0xBC
#define REG_CAL_MD 0xBE
#define REG_DATA 0xF6 // data for both Temp and Press
#define REG_ID 0xD0 // register contains constant 0x55 (used to check communication)
// BMP Registers Write
#define REG_CTRL 0xF4 // Control Register, see commands below - [<7:6> OSS - Oversampling Ratio][<5> SCO - Start of Conversation][<4:0> Measurement Control] RS: 0x00
#define REG_SOFT_RESET 0xE0 // [<7:0> Reset - If set to 0xB6 a soft reset will happen] RS:0x00
// Commands (Write these to REG_CTRL)
#define CMD_READTEMP 0x2E
#define CMD_READPRES 0x34
#define CMD_RESTART 0xB6

#define BMP180_ADDRESS 0x77

extension::BMP180::BMP180(DataStream *p_data_stream, ExtensionMetadata extension_metadata) :
    Extension(p_data_stream, extension_metadata), 
	bus_number_(extension_metadata.extra_args.I2C_bus),
	device_address_(BMP180_ADDRESS),
	i2c_bus_(I2C(bus_number_, device_address_, p_data_stream->getI2CBusLock()))
	//raw_calibration_data_(0) 
	{

}

extension::BMP180::~BMP180() {

}

int extension::BMP180::runner() {
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

	// Check that the device is connected and communicative
	result = handshake();
	if (result == 0) {// Handshake successful
		setStatus(ExtensionStatus::RUNNING);
	} if (result == -1) { // Handshake failed
		setStatus(ExtensionStatus::ERROR);
		error("HSK_F");
		return -1;
	} else if (result == -2) { // Value returned by device was not 0x55
		setStatus(ExtensionStatus::ERROR);
		std::bitset<8> value = result;
		error("HSK_U", value.to_string());
		return -1;
	}

	result = readCalibrationData();
	if (result != 0) { // Read calibration data failed
		setStatus(ExtensionStatus::ERROR);
		error("CD_R");
		return -1;
	} //else if (result == -2) { // Not yet implemented
	//	setStatus(ExtensionStatus::ERROR);
	//	error("CD_U");
	//	return -1;
	//}

    while (!stop_flag_) {
        if (readRawTemperature() == 0) {
			calculateTemperature();
			sendData("TEMP_C", temp_C_);
        	sendData("TEMP_F", temp_F_);
		} else {
			error("RT");
		}

        if (readRawPressure() == 0) {
			calculatePressure();
        	sendData("P_HPA", pressure_HPA_);
        	sendData("P_INHG", pressure_INHG_);
        	sendData("P_ALT_M", pressure_altitude_meters_);
        	sendData("P_ALT_F", pressure_altitude_feet_);
		} else {
			error("RP");
		}
		
		if (BMP180_DEBUG) {
			for (std::pair<std::string, int> e: raw_data_) {
				sendData(e.first, e.second);
			}
		}

		std::this_thread::sleep_for(
            std::chrono::milliseconds(getUpdateInterval())
        );
    }
    setStatus(ExtensionStatus::STOPPED);
    return 0;
}

int extension::BMP180::readCalibrationData() {
	AC1_ = readShort(REG_CAL_AC1);
	AC2_ = readShort(REG_CAL_AC2);
	AC3_ = readShort(REG_CAL_AC3);
	AC4_ = readShortUnsigned(REG_CAL_AC4);
	AC5_ = readShortUnsigned(REG_CAL_AC5);
	AC6_ = readShortUnsigned(REG_CAL_AC6);
	B1_ = readShort(REG_CAL_B1);
	B2_ = readShort(REG_CAL_B2);
	MB_ = readShort(REG_CAL_MB);
	MC_ = readShort(REG_CAL_MC);
	MD_ = readShort(REG_CAL_MD);

	if (BMP180_DEBUG) {
		raw_data_.insert_or_assign("B1", B1_);
		raw_data_.insert_or_assign("B2", B2_);
		raw_data_.insert_or_assign("MB", MB_);
		raw_data_.insert_or_assign("MC", MC_);
		raw_data_.insert_or_assign("MD", MD_);
		raw_data_.insert_or_assign("AC1", AC1_);
		raw_data_.insert_or_assign("AC2", AC2_);
		raw_data_.insert_or_assign("AC3", AC3_);
		raw_data_.insert_or_assign("AC4", AC4_);
		raw_data_.insert_or_assign("AC5", AC5_);
		raw_data_.insert_or_assign("AC6", AC6_);
	}
	
	if (AC1_ == -1 || AC2_ == -1 || AC3_ == -1 || AC4_ == 0 || AC5_ == 0 // any i2c errors
		|| AC6_ == 0 || B1_ == -1 || B2_ == -1 || MB_ == -1 || MC_ == -1
		|| MD_ == -1) {
		return -1;
	}

	return 0;
}

int extension::BMP180::readRawTemperature() {
    if (i2c_bus_.writeByteToReg(REG_CTRL, CMD_READTEMP) != 0) { // write 0x2E into reg 0xF4
		return -1;
	}

	if (usleep(5000) != 0) { error("P_SLP"); } // wait 5ms (4.5ms minimum according to data sheet)

	int MSB = i2c_bus_.readByteFromReg(REG_DATA);
	int LSB = i2c_bus_.readByteFromReg(REG_DATA + 1);

	if (BMP180_DEBUG) {
		raw_data_.insert_or_assign("T_MSB", MSB);
		raw_data_.insert_or_assign("T_LSB", LSB);
	}

	if (MSB == -1 || LSB == -1) {
		return -1;
	}
	
	UT_ = (MSB << 8) + LSB;
	raw_temperature_data_ = UT_;

	return 0;
}

int extension::BMP180::readRawPressure() {
    volatile int MSB;
	volatile int LSB;
	volatile int XLSB;
	
	int result = i2c_bus_.writeByteToReg(REG_CTRL, CMD_READPRES+(SAMPLING_ACCURACY<<6)); // write 0x34+(oss<<6) into reg 0xF4
	if (result != 0) {
		return -1;
	}

	int sleep_result = -1;

	switch (SAMPLING_ACCURACY) // wait 4.5ms to 25.5ms (refer to table 8 of bmp180 documentation)
	{
	case 0:
		sleep_result = usleep(5000);
		break;
	case 1:
		sleep_result = usleep(8000);
		break;
	case 2:
		sleep_result = usleep(14000);
		break;
	case 3:
		sleep_result = usleep(26000);
		break;
	}

	if (sleep_result != 0) {
		error("P_SLP");
		return -1;
	}

	MSB = i2c_bus_.readByteFromReg(REG_DATA);
	LSB = i2c_bus_.readByteFromReg(REG_DATA + 1);
	XLSB = i2c_bus_.readByteFromReg(REG_DATA + 2);

	if (BMP180_DEBUG) {
		raw_data_.insert_or_assign("P_MSB", MSB);
		raw_data_.insert_or_assign("P_LSB", LSB);
		raw_data_.insert_or_assign("P_XLSB", XLSB);
	}

	if (MSB == -1 || LSB == -1 || XLSB == -1) {
		return -1;
	}
	
	UP_ = ((MSB<<16) + (LSB<<8) + XLSB) >> (8-SAMPLING_ACCURACY); // read reg 0xF6 (MSB), 0xF7 (LSB), 0xF8 (XLSB)
	return 0;
}

int extension::BMP180::calculateTemperature() { /** @todo POSSIBLE ARITHMETIC ERRORS*/
	if (X1_ + MD_ == 0) { // prevent divide by zero
		error("DIV0");
		return -1;
	}
	X1_ = ((UT_ - AC6_) * AC5_) >> 15;
	X2_ = (MC_ << 11) / (X1_ + MD_);
	B5_ = X1_ + X2_;
	T_ = (B5_ + 8) >> 4; // Temperature in 0.1c (temp c = mT * 0.1)
	temp_C_ = (float)T_ / 10.0; // Cast to float to get decimal precision
	temp_F_ = (temp_C_ * 1.8) + 32;
	if (temp_C_ < -40) {
		error("TBAR");
	} else if (temp_C_ > 85) {
		error("TAAR");
	}
	return 0;
}

int extension::BMP180::calculatePressure() {
	B6_ = B5_ - 4000;

	// calculate B3
	X1_ = (B6_ * B6_) >> 12;
	X1_ *= B2_;
	X1_ >>= 11;
	X2_ = AC2_ * B6_;
	X2_ >>= 11;
	X3_ = X1_ + X2_;
	B3_ = ((((AC1_<<2) + X3_) << SAMPLING_ACCURACY) + 2) >> 2;

	// calculate B4
	X1_ = (AC3_ * B6_) >> 13;
	X2_ = (B1_ * ((B6_ * B6_) >> 12)) >> 16;
	X3_ = ((X1_ + X2_) + 2) >> 2;
	B4_ = (AC4_ * (X3_ + 32768)) >> 15;
	B7_ = ((UP_ - B3_) * (50000 >> SAMPLING_ACCURACY));

	if (B4_ == 0) { // prevent divide by zero
		error("DIV0");
		return -1;
	}

	if ((unsigned) B7_ < 0x80000000) {
		P_ = (B7_ * 2) / B4_;
	} else {
		P_ = ((B7_ / B4_) * 2);
	}

	X1_ = P_ >> 8;
	X1_ *= X1_;
	X1_ = (X1_ * 3038) >> 16;
	X2_ = (P_ * -7357) >> 16;
	P_ += (X1_ + X2_ + 3791) >> 4; // Pressure in Pa
	pressure_HPA_ = (float)P_ / (float)100; // Cast to float to get decimal precision
	pressure_INHG_ = pressure_HPA_ / 33.864;

	if (pressure_HPA_ > 0.1 || pressure_HPA_ < 0.1) { // prevent divide by zero without float == comparison
		pressure_altitude_meters_ = ((pow((1013.25 / pressure_HPA_), 1/5.257)-1) * (temp_C_ + 273.15)) / 0.0065;
		pressure_altitude_feet_ = pressure_altitude_meters_ * 3.281;
	} else {
		pressure_altitude_meters_ = 0;
		pressure_altitude_feet_ = 0;
	}
	if (pressure_HPA_ <= 305) {
		error("PBAR");
	}
	else if (pressure_HPA_ > 1100) {
		error("PAAR");
	}
	return 0;
}

int extension::BMP180::handshake() {
	int i = i2c_bus_.readByteFromReg(REG_ID);
	if (i == 0x55) {
		return 0; // Good handshake
	} else if (i == 0) {
		return -1; // I2C failure
	} else {
		return i; // unknown failure
	}
}

unsigned short extension::BMP180::readShortUnsigned(int register_address) { // pass in the lower register number
	int MSB, LSB;
	MSB = i2c_bus_.readByteFromReg(register_address);
	LSB = i2c_bus_.readByteFromReg(register_address + 1);

	if (MSB == -1 || LSB == -1) {
		return 0; // I2C failure
	}

	int value = (MSB << 8) + LSB;
	return (unsigned short)value;
}

short extension::BMP180::readShort(int register_address) { // Used readShortUnsigned() and casts to short
	int value; 
	value = readShortUnsigned(register_address);

	if (value == 0) {
		return -1; // I2C failure
	}

	if (value > 32767) {
		value -= 65536;
	}
	return (short)value;
}