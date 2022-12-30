/**
 * @file bme280.cpp
 * @author Joshua Jerred (github.com/joshua-jerred)
 * @brief This file contains the implementation of the BME280 extension.
 * @date 2022-11-01
 * @copyright Copyright (c) 2022
 * @version 0.1.0
 */

#include "extensions.h"

#define BME280_ADDRESS 0x76

BME280::BME280(DataStream *p_data_stream, ExtensionMetadata extension_metadata) :
    Extension(p_data_stream, extension_metadata), 
	bus_number_(extension_metadata.extra_args.I2C_bus),
	device_address_(BME280_ADDRESS),
	i2c_bus_(I2C(bus_number_, device_address_))
	//raw_calibration_data_(0) 
	{

}

BME280::~BME280() {

}

int BME280::runner() {
	if (handshake() != 0) {
		error("BME280: Handshake failed");
	}
	return 0;
}

/**
 * @brief Reads the 'ID' register on the BME280, which should return 0x60.
 * 
 * @return int 0 on success, -1 on I2C failure, 
 * or the value of the register on failure.
 */
int BME280::handshake() {
	const uint8_t kReg_Id = 0xD0;
	const uint8_t kExpectedResult = 0x60;

	int i = i2c_bus_.readByteFromReg(kReg_Id);
	if (i == kExpectedResult) {
		return 0; // Good handshake
	} else if (i == 0) {
		return -1; // I2C failure
	} else {
		return i; // unknown failure
	}
}

int BME280::reset() {
	const uint8_t kReg_Reset = 0xE0;
	const uint8_t kResetCommand = 0xB6;

	return i2c_bus_.writeByteToReg(kReg_Reset, kResetCommand);
}

/**
 * @brief Configures the BME280. (Weather Monitoring config, Section
 * 3.5.1)
 * @details
 * Sensor Mode: Normal
 * Oversampling: 1x on all
 * IIR Filter: Off
 * Standby Time: 125ms
 * 
 * @return int 
 */
int BME280::configure() {
	const uint8_t kReg_Ctrl_Hum = 0xF2; // Humidity oversampling (Written to before Ctrl_Meas)
	const uint8_t kReg_Ctrl_Meas = 0xF4; // Temp and pressure oversampling
	const uint8_t kReg_Config = 0xF5; // Rate, filter, interface

	// The following can be changed
	const uint8_t kCtrlHumSetting = 0x01; // 1x oversampling 
	const uint8_t kTempAndPressureSettings = 0b00100111; // 1x oversampling for temp and pressure, normal mode
	const uint8_t kConfigSettings = 0b01000000; // 125ms standby time

	int write_result = 0;
	write_result = i2c_bus_.writeByteToReg(kReg_Ctrl_Hum, kCtrlHumSetting);
	if (write_result != 0) {
		return -1;
	}

	write_result = i2c_bus_.writeByteToReg(kReg_Ctrl_Meas, kTempAndPressureSettings);
	if (write_result != 0) {
		return -1;
	}

	write_result = i2c_bus_.writeByteToReg(kReg_Config, kConfigSettings);
	if (write_result != 0) {
		return -1;
	}

	return 0;
}

int BME280::readCompensationData() {
	const uint8_t kCalibrationDataHead_sec1 = 0x88; // Head of calibration data
	const uint8_t kCalibrationDataLength_sec1 = 26; // Length of calibration data (section 1)

	const uint8_t kCalibrationDataHead_sec2 = 0xE1; // Head of calibration data
	const uint8_t kCalibrationDataLength_sec2 = 7; // Length of calibration data (section 2)

	uint8_t data_section1[kCalibrationDataLength_sec1] = {0};
	uint8_t data_section2[kCalibrationDataLength_sec2] = {0};

	int bytes_read = 0;

	bytes_read = i2c_bus_.readChunkFromReg(data_section1, 
										   kCalibrationDataLength_sec1, 
										   kCalibrationDataHead_sec1);
	if (bytes_read != kCalibrationDataLength_sec1) {
		return -1;
	}

	bytes_read = i2c_bus_.readChunkFromReg(data_section2, 
										   kCalibrationDataLength_sec2, 
										   kCalibrationDataHead_sec2);
	if (bytes_read != kCalibrationDataLength_sec2) {
		return -1;
	}

	temp_comp_data_.dig_T1 = (data_section1[1] << 8) | data_section1[0];
	temp_comp_data_.dig_T2 = (data_section1[3] << 8) | data_section1[2];
	temp_comp_data_.dig_T3 = (data_section1[5] << 8) | data_section1[4];

	press_comp_data_.dig_P1 = (data_section1[7] << 8) | data_section1[6];
	press_comp_data_.dig_P2 = (data_section1[9] << 8) | data_section1[8];
	press_comp_data_.dig_P3 = (data_section1[11] << 8) | data_section1[10];
	press_comp_data_.dig_P4 = (data_section1[13] << 8) | data_section1[12];
	press_comp_data_.dig_P5 = (data_section1[15] << 8) | data_section1[14];
	press_comp_data_.dig_P6 = (data_section1[17] << 8) | data_section1[16];
	press_comp_data_.dig_P7 = (data_section1[19] << 8) | data_section1[18];
	press_comp_data_.dig_P8 = (data_section1[21] << 8) | data_section1[20];
	press_comp_data_.dig_P9 = (data_section1[23] << 8) | data_section1[22];

	hum_comp_data_.dig_H1 = data_section1[25];
	hum_comp_data_.dig_H2 = (data_section2[1] << 8) | data_section2[0];
	hum_comp_data_.dig_H3 = data_section2[2];
	hum_comp_data_.dig_H4 = (data_section2[3] << 4) | (0x0F & data_section2[4]);
	hum_comp_data_.dig_H5 = (data_section2[5] << 4) | ((data_section2[4] >> 4) & 0x0F);

	return 0;
}

/**
 * @brief Reads the raw environmental data from the BME280.
 * 
 * The raw data (pressure, temperature, humidity) is stored in the
 * follow registers:
 * 
 * Pressure: 0xF7, 0xF8, 0xF9
 * Temperature: 0xFA, 0xFB, 0xFC
 * Humidity: 0xFD, 0xFE
 * 
 * You'll notice that all of these registers are sequential. This means that
 * all of the data can be read in a single I2C transaction.
 * (As recommended in section 4 of the datasheet)
 * 
 * Pressure:
 * 0xF7 data[0] 7:0 MSB - 8 bits 
 * 0xF8 data[1] 7:0 LSB - 8 bits 
 * 0xF9 data[2] 7:4 XLSB - 4 bits
 * [19:12] [11:4] [3:0]
 * 
 * Temperature:
 * 0xFA data[3] 7:0 MSB - 8 bits
 * 0xFB data[4] 7:0 LSB - 8 bits
 * 0xFC data[5] 7:4 XLSB - 4 bits
 * [19:12] [11:4] [3:0]
 * 
 * Humidity:
 * 0xFD data[6] 7:0 MSB - 8 bits
 * 0xFE data[7] 7:0 LSB - 8 bits
 * [15:8] [7:0]
 * 
 * @param raw_data 
 * @return int 0 on success, -1 on failure
 */
int BME280::readData(RawEnvironmentData &raw_data) {
	const uint8_t kDataRegisterHead = 0xF7; 

	int bytes_read = 0;
	uint8_t data[8] = {0};

	bytes_read = i2c_bus_.readChunkFromReg(data, 8, kDataRegisterHead);
	if (bytes_read != 8) {
		return -1;
	}

	raw_data.pressure = (data[0] << 12) | (data[1] << 4) | (data[2] >> 4);
	raw_data.temperature = (data[3] << 12) | (data[4] << 4) | (data[5] >> 4);
	raw_data.humidity = (data[6] << 8) | data[7];

	return 0;
}