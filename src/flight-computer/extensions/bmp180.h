/**
 * @file bmp180.h
 * @author Joshua Jerred (github.com/joshua-jerred)
 * @brief This file defines the extension for the BMP180.
 * @date 2022-11-01
 * @copyright Copyright (c) 2022
 * @version 0.0.9
 */

#ifndef BMP180_H_
#define BMP180_H_

#include <string>
#include <sstream>
#include <cmath>
#include <unistd.h>

#include "utility-i2c.h"

#include "extension.h"

// Configurable
#define SAMPLING_ACCURACY 1 // Refer to BMP180 data sheet "oversampling_setting", if this is changed BMP180_READPRESSURECMD must also 
#define DEVICE_ADDRESS 0x77

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

class BMP180 : public Extension {
public:
    BMP180(DataStream *p_data_stream, ExtensionMetadata extension_metadata);
    ~BMP180();
    int runner();

private:
    int readCalibrationData();
    int readRawTemperature();
    int readRawPressure();

    int calculateTemperature();
    int calculatePressure();

    unsigned short BMP180::readShortUnsigned(int registerAddress);
    short BMP180::readShort(int registerAddress);

    int handshake();

    uint8_t bus_number_;
    uint8_t device_address_;
    I2C i2c_bus_;

    // Environmental Data:
    float temp_F_;
    float temp_C_;
    float pressure_HPA_;
    float pressure_INHG_;
    float pressure_altitude_meters_;
    float pressure_altitude_feet_;
    std::vector<unsigned short> raw_calibration_data_; // [tempMSB, tempLSB, pressMSB, pressLSB, pressXLSB]
    int raw_temperature_data_;
    int raw_pressure_data_;

    // Calibration Data
    volatile short AC1_;
    volatile short AC2_;
    volatile short AC3_;
    volatile unsigned short AC4_;
    volatile unsigned short AC5_;
    volatile unsigned short AC6_;
    volatile short B1_;
    volatile short B2_;
    volatile short MB_;
    volatile short MC_;
    volatile short MD_;

    // Data
    int UT_;
    int UP_;
    int X1_;
    int X2_;
    int X3_;
    int B3_;
    unsigned int B4_;
    int B5_;
    int B6_;
    int B7_;
    int T_;
    int P_;
};

#endif // BMP180_H_