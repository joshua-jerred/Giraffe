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
#include <bitset>
#include <unordered_map>

#include "extension-interface.h"

#include "extension.h"

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

    int bus_number_;
    int device_address_;
    I2C i2c_bus_;

    // Environmental Data:
    float temp_F_ = 0.0;
    float temp_C_ = 0.0;
    float pressure_HPA_ = 0.0;
    float pressure_INHG_ = 0.0;
    float pressure_altitude_meters_ = 0.0;
    float pressure_altitude_feet_ = 0.0;
    std::unordered_map<std::string, int> raw_data_ = {}; 
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