/**
 * @file extensions.h
 * @author Joshua Jerred (github.com/joshua-jerred)
 * @brief This file declares the Extension class.
 * 
 * @version 0.0.9
 * @date 2022-10-09
 * @copyright Copyright (c) 2022
 */

#ifndef EXTENSION_H_
#define EXTENSION_H_

#include <atomic>
#include <thread>

#include "utility-data-stream.h"
#include "extension-interface.h"

/**
 * @brief This class is the base class for all extensions. Extensions include
 * all sensors, actuators, radios, internal data collection tools, etc.
 * 
 * This class is responsible for setting up the extension. The extension
 * will have a runner function that will be called in it's own thread.
 * 
 * The runner function is responsible for reading data from sensors and
 * date collection tools and writing that data to the data stream.
 * @see simple-extension.cpp for example usage.
 */
class Extension {
public:
    Extension(DataStream *p_data_stream, ExtensionMetadata extension_metadata);
    Extension(const Extension& other) = delete; // No copy constructor
    Extension& operator=(const Extension& other) = delete; // No copy assignment
    virtual ~Extension();

    void start();
    void stop();
    void restart();
    ExtensionStatus getStatus();

    int getID();
    std::string getName();
    std::string getType();
    ExtensionMetadata::Category getCategory();
    ExtensionMetadata::Interface getInterface();
    int getUpdateInterval();
    int getCritical();
    std::vector<std::string> getExtraArgs(); /** @todo Implement this. */

protected:
    virtual int runner(); // Must be overridden
    void setStatus(ExtensionStatus status);

    void sendData(std::string unit, std::string value);
    void sendData(std::string unit, int value);
    void sendData(std::string unit, float value);

    //template <typename T>
    //void error(std::string error_code, T info);
    void error(std::string error_code, std::string info);
    void error(std::string error_code);

    std::atomic<int> stop_flag_ = 1; // 0 = continue, 1 = stop

private:
    void setID(int num);
    void setName(std::string name);
    void setType(std::string type);
    void setCategory(ExtensionMetadata::Category category);
    void setInterface(ExtensionMetadata::Interface interface);
    void setUpdateInterval(int interval);
    void setCritical(int critical);
    void setExtraArgs(std::vector<std::string> extra_args); /** @todo Implement this. */


    virtual void spawnRunner();

    DataStream * const p_data_stream_;

    std::thread runner_thread_ = std::thread();
    std::atomic<ExtensionStatus> status_;

    int id_ = 0;
    std::string name_ = "";
    std::string type_ = "";
    ExtensionMetadata::Category category_ = ExtensionMetadata::Category::ERROR;
    ExtensionMetadata::Interface interface_ = ExtensionMetadata::Interface::ERROR;
    int update_interval_ = 0;
    int data_expiration_time_ = 1000;
    int critical_ = 0;
};

class ExtensionException {
public:
    ExtensionException(std::string s) : m_s(s) {};
    ~ExtensionException() {};

    std::string description() { return m_s; }

private:
    std::string m_s;
};


/**
 * @brief BMP180 sensor extension.
 * 
 */
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
    std::string raw_data_ = "";
    int16_t raw_temp_ = 0;
    float temp_F_ = 0;
    float temp_C_ = 0;

    OneWire one_wire_device_;
};

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
-------------------SIMULATED EXTENSIONS----------------------------------------
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
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

#endif // EXTENSION_H_