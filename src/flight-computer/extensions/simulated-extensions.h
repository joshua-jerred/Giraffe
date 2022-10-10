/**
 * @file simulated-extensions.h
 * @author Joshua Jerred (github.com/joshua-jerred)
 * @brief This file contains the declarations of the simulated extensions.
 * 
 * @version 0.0.9
 * @date 2022-10-09
 * @copyright Copyright (c) 2022
 */

#ifndef SIMULATED_EXTENSIONS_H_
#define SIMULATED_EXTENSIONS_H_

#include <iostream>
#include <thread>
#include <syncstream>
#include <atomic>

#include "utility-config-types.h"
#include "utility-data-stream.h"

#include "extension.h"

#include "extension.h"

/**
 * @brief The TestExtension is a data stream test extension.
 * @details The TestExtension sends data to the data stream every cycle.
 * It sends 'unit' and 'value' as the unit and value respectively.
 * @todo Implement shutdown
 */
class TestExtension : public Extension {
 public:
  TestExtension(DataStream *p_data_stream, ExtensionMetadata extension_metadata);
  ~TestExtension();
  int runner();
};

/**
 * @brief The BMP180_SIM is a simulated BMP180 extension.
 * @details The BMP180_SIM sends simulated data to the data stream every cycle.
 * It sends the temperature in Fahrenheit and the pressure in millibars with the 
 * units 'TEMP_F' and 'PRES_M'.
 * @todo Implement shutdown and address in the configuration
 */
class BMP180_SIM : public Extension {
 public:
  BMP180_SIM(DataStream *p_data_stream, ExtensionMetadata extension_metadata);
  ~BMP180_SIM();
  int runner();
};

/**
 * @brief The SAMM8Q_SIM is a simulated SAM-M8Q extension.
 * @details The SAMM8Q_SIM sends simulated data to the data stream every cycle.
 * It sends the following data unit pairs: <GPS_LAT, lattitude>, 
 * <GPS_LON, longitude>, <GPS_ALT_M, altitude in meters>, 
 * <GPS_QUAL, quality 2, 1, 0>, <HORZ_SPEED, horizontal speed in m/s>,
 * <VERT_SPEED, vertical speed in m/s>.
 * 
 * Vertical speed will only be sent one time, which will result in stale data.
 * This is to test the data module.
 * Lat, lon, alt, and horizontal speed will change every cycle.
 */
class SAMM8Q_SIM : public Extension {
 public:
  SAMM8Q_SIM(DataStream *p_data_stream, ExtensionMetadata extension_metadata);
  ~SAMM8Q_SIM();
  int runner();
};

/**
 * @brief The DS18B20_SIM is a simulated DS18B20 extension.
 * @details The DS18B20_SIM sends simulated data to the data stream every cycle.
 * It send the temperature in Celsius with the unit 'TEMP_C'.
 * TThe data will change each cycle.
 */
class DS18B20_SIM :public Extension {
  public:
  DS18B20_SIM(DataStream *p_data_stream, ExtensionMetadata extension_metadata);
  ~DS18B20_SIM();
  int runner();
};

/**
 * @brief Currently not implemented.
 * @details The DRA818V_SIM will be a simulated DRA818V extension.
 * @todo Implement the DRA818V_SIM after the telemetry module is implemented.
 */
class DRA818V_SIM :public Extension {
  public:
  DRA818V_SIM(DataStream *p_data_stream, ExtensionMetadata extension_metadata);
  ~DRA818V_SIM();
  int runner();
};

#endif // SIMULATED_EXTENSIONS_H_