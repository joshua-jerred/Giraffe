/**
 * @file ds18b20.h
 * @author Joshua Jerred (github.com/joshua-jerred)
 * @brief Declaration of the DSB18B20 class.
 * @date 2022-11-02
 * @copyright Copyright (c) 2022
 * @version 0.1.0
 */

#ifndef DS18B20_H_
#define DS18B20_H_

#include "utility-data-stream.h"
#include "utility-one-wire.h"

#include "extension.h"

class DS18B20: public Extension {
public:
    DS18B20(DataStream *p_data_stream, ExtensionMetadata extension_metadata);
    ~DS18B20();
    int runner();

private:
    int readData();
    int processData();
    uint8_t calculateCRC(uint8_t *addr, uint8_t len);

    std::string one_wire_device_id_;
    std::string raw_data_;
    int16_t raw_temp_;
    float temp_F_;
    float temp_C_;

    OneWire one_wire_device_;
};

#endif // DS18B20_H_