/**
 * @file bmp180.cpp
 * @author Joshua Jerred (github.com/joshua-jerred)
 * @brief This file contains the implementation of the BMP180 extension.
 * @date 2022-11-01
 * @copyright Copyright (c) 2022
 * @version 0.1.0
 */

#include "bmp180.h"

BMP180::BMP180(DataStream *p_data_stream, ExtensionMetadata extension_metadata) :
    Extension(p_data_stream, extension_metadata) {

    i2c_bus_ = I2C();

    bus_number_ = (uint8_t) extension_metadata.extra_args.I2C_bus;

    std::stringstream strs;
    strs << std::hex << extension_metadata.extra_args.I2C_device_address;
    strs >> device_address_;
}

BMP180::~BMP180() {

}

int BMP180::runner() {
    setStatus(ExtensionStatus::RUNNING);
    try {
        i2c_bus_.connect(bus_number_, device_address_);
        handshake();
    } catch (const std::exception& e) {
        error("BMP180_CNCT", e.what());
        setStatus(ExtensionStatus::ERROR);
        return -1;
    }
    while (!stop_flag_) {
        std::this_thread::sleep_for(
            std::chrono::milliseconds(getUpdateInterval())
            );
        if (readRawTemperature() != 0) {
            continue;
        }
        if (readRawPressure() != 0) {
            continue;
        }
        if (calculateTemperature() != 0) {
            continue;
        }
        if (calculatePressure() != 0) {
            continue;
        }

        sendData("TEMP_C", temp_C_);
        sendData("TEMP_F", temp_F_);
        sendData("P_HPA", pressure_HPA_);
        sendData("P_INHG", pressure_INHG_);
        sendData("P_ALT_M", pressure_altitude_meters_);
        sendData("P_ALT_F", pressure_altitude_feet_);

    }
    setStatus(ExtensionStatus::STOPPED);
    return 0;
}

int BMP180::readCalibrationData() {
    try {
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
	    raw_calibration_data_.clear();
	    raw_calibration_data_.push_back(AC1_);
	    raw_calibration_data_.push_back(AC2_);
	    raw_calibration_data_.push_back(AC3_);
	    raw_calibration_data_.push_back((uint16_t) AC4_);
	    raw_calibration_data_.push_back((uint16_t) AC5_);
	    raw_calibration_data_.push_back((uint16_t) AC6_);
	    raw_calibration_data_.push_back(B1_);
	    raw_calibration_data_.push_back(B2_);
	    raw_calibration_data_.push_back(MB_);
	    raw_calibration_data_.push_back(MC_);
	    raw_calibration_data_.push_back(MD_);
	    if (AC1_ == -1 || AC2_ == -1 || AC3_ == -1 || AC4_ == 0 || AC5_ == 0 // any i2c errors
	    	|| AC6_ == 0 || B1_ == -1 || B2_ == -1 || MB_ == -1 || MC_ == -1
	    	|| MD_ == -1) {
                throw ExtensionException("Error reading calibration data");
	    }
    } catch (ExtensionException &e) {
        error("BMP180", e.description());
        setStatus(ExtensionStatus::ERROR);
        return -1;
    } catch (std::exception &e) {
        error("BMP180", e.what());
        setStatus(ExtensionStatus::ERROR);
        return -1;
    }
	return 0;
}

int BMP180::readRawTemperature() {
    try {
        i2c_bus_.writeByteToReg(REG_CTRL, CMD_READTEMP); // write 0x2E into reg 0xF4
	    usleep(5000); // wait 4.5ms
	    int MSB = i2c_bus_.readByteFromReg(REG_DATA);
	    int LSB = i2c_bus_.readByteFromReg(REG_DATA + 1);
	    UT_ = (MSB << 8) + LSB;
	    raw_temperature_data_ = UT_;
	    return 0;
    } catch (std::exception &e) {
        error("BMP180", e.what());
        setStatus(ExtensionStatus::ERROR);
    }
    return -1;
}

int BMP180::readRawPressure() {
    try {
        volatile int MSB;
	    volatile int LSB;
	    volatile int XLSB;
	    i2c_bus_.writeByteToReg(REG_CTRL, CMD_READPRES+(SAMPLING_ACCURACY<<6)); // write 0x34+(oss<<6) into reg 0xF4
	    switch (SAMPLING_ACCURACY) // wait 4.5ms to 25.5ms (refer to table 8 of bmp180 documentation)
	    {
	    case 0:
	    	usleep(5000);
	    	break;
	    case 1:
	    	usleep(8000);
	    	break;
	    case 2:
	    	usleep(14000);
	    	break;
	    case 3:
	    	usleep(26000);
	    	break;
	    }
	    MSB = i2c_bus_.readByteFromReg(REG_DATA);
	    LSB = i2c_bus_.readByteFromReg(REG_DATA + 1);
	    XLSB = i2c_bus_.readByteFromReg(REG_DATA + 2);
	    UP_ = ((MSB<<16) + (LSB<<8) + XLSB) >> (8-SAMPLING_ACCURACY); // read reg 0xF6 (MSB), 0xF7 (LSB), 0xF8 (XLSB)
	    return 0;
    } catch (std::exception &e) {
        error("BMP180", e.what());
        setStatus(ExtensionStatus::ERROR);
    }
    return -1;
}

int BMP180::calculateTemperature() {
    try {
	    X1_ = ((UT_ - AC6_) * AC5_) >> 15;
	    X2_ = (MC_ << 11) / (X1_ + MD_);
	    B5_ = X1_ + X2_;
	    T_ = (B5_ + 8) >> 4; // Temperature in 0.1c (temp c = mT * 0.1)
	    temp_C_ = (float)T_ / 10.0; // Cast to float to get decimal precision
	    temp_F_ = (temp_C_ * 1.8) + 32;
	    if (temp_C_ < -40) {
	    	error("BMP180", "Temp below accurate range.");
	    } else if (temp_C_ > 85) {
	    	error("BMP180", "Temp above accurate range.");
	    }
	    return 0;
    } catch (std::exception &e) {
        error("BMP180", e.what());
        setStatus(ExtensionStatus::ERROR);
    }
    return -1;
}

int BMP180::calculatePressure() {
    try {
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
	    pressure_altitude_meters_ = ((pow((1013.25 / pressure_HPA_), 1/5.257)-1) * (temp_C_ + 273.15)) / 0.0065;
	    pressure_altitude_feet_ = pressure_altitude_meters_ * 3.281;
	    if (pressure_HPA_ <= 305) {
	    	error("BMP180", "Pres below accurate range.");
	    }
	    else if (pressure_HPA_ > 1100) {
	    	error("BMP180", "Pres above accurate range.");
	    }
	    return 0;
    } catch (std::exception &e) {
        error("BMP180", e.what());
        setStatus(ExtensionStatus::ERROR);
    }
    return -1;
}

int BMP180::handshake() {
    try {
	    int i = readShortUnsigned(REG_ID);
	    if (i == 0x55) {
	    	return 0; // Good handshake
	    } else if (i == -1) {
	    	return -1; // I2C failure
	    } else {
	    	return -2; // unknown failure
	    }
    } catch (std::exception &e) {
        error("BMP180", e.what());
        setStatus(ExtensionStatus::ERROR);
		return -3; // Exception
    }
}

unsigned short BMP180::readShortUnsigned(int register_address) { // pass in the lower register number
	int MSB, LSB;
	MSB = i2c_bus_.readByteFromReg(register_address);
	LSB = i2c_bus_.readByteFromReg(register_address + 1);
	int value = (MSB << 8) + LSB;
	return (unsigned short)value;
}

short BMP180::readShort(int register_address) { // Used readShortUnsigned() and converts to signed.
	int value; 
	value = readShortUnsigned(register_address); 
	if (value > 32767) {
		value -= 65536;
	}
	return (short)value;
}