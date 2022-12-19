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

    unsigned short readShortUnsigned(int registerAddress);
    short readShort(int registerAddress);

    int handshake();

    uint8_t bus_number_;
    uint8_t device_address_;
    I2C i2c_bus_;

    // Environmental Data:
    float temp_F_ = 0.0;
    float temp_C_ = 0.0;
    float pressure_HPA_ = 0.0;
    float pressure_INHG_ = 0.0;
    float pressure_altitude_meters_ = 0.0;
    float pressure_altitude_feet_ = 0.0;
    std::vector<unsigned short> raw_calibration_data_; // [tempMSB, tempLSB, pressMSB, pressLSB, pressXLSB]
    int raw_temperature_data_ = 0;
    int raw_pressure_data_ = 0;

    // Calibration Data
    volatile short AC1_ = 0;
    volatile short AC2_ = 0;
    volatile short AC3_ = 0;
    volatile unsigned short AC4_ = 0;
    volatile unsigned short AC5_ = 0;
    volatile unsigned short AC6_ = 0;
    volatile short B1_ = 0;
    volatile short B2_ = 0;
    volatile short MB_ = 0;
    volatile short MC_ = 0;
    volatile short MD_ = 0;

    // Data
    int UT_ = 0;
    int UP_ = 0;
    int X1_ = 0;
    int X2_ = 0;
    int X3_ = 0;
    int B3_ = 0;
    unsigned int B4_ = 0;
    int B5_ = 0;
    int B6_ = 0;
    int B7_ = 0;
    int T_ = 0;
    int P_ = 0;
};

#endif // BMP180_H_