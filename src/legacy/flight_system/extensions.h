/**
 * @file extensions.h
 * @author Joshua Jerred (https://joshuajer.red/)
 * @brief This file contains the definitions for the classes related to
 * extensions.
 *
 * @version 0.3
 * @date 2023-01-06
 * @copyright Copyright (c) 2023
 */

#ifndef EXTENSION_H_
#define EXTENSION_H_

#include <atomic>
#include <thread>

#include "data-stream.h"
#include "interface.h"

namespace extension {

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
  Extension(const Extension &other) = delete;             // No copy constructor
  Extension &operator=(const Extension &other) = delete;  // No copy assignment
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
  //  std::vector<std::string> getExtraArgs(); /** @todo May be able to remove
  //  this */

  // The following are used by the extensions module.
  std::atomic<int> restart_attempts_ = 0;

 protected:
  virtual int runner();  // Must be overridden
  void setStatus(ExtensionStatus status);

  void sendData(std::string unit, std::string value);
  void sendData(std::string unit, int value);
  void sendData(std::string unit, float value, int precision = 1);
  void sendData(GPSFrame gps_frame);

  // template <typename T>
  // void error(std::string error_code, T info);
  void error(std::string error_code, std::string info);
  void error(std::string error_code);

  std::atomic<int> stop_flag_ = 1;  // 0 = continue, 1 = stop
  int data_expiration_time_ = 1000;

 private:
  void setID(int num);
  void setName(std::string name);
  void setType(std::string type);
  void setCategory(ExtensionMetadata::Category category);
  void setInterface(ExtensionMetadata::Interface interface);
  void setUpdateInterval(int interval);
  void setCritical(int critical);
  void setExtraArgs(
      std::vector<std::string> extra_args); /** @todo Implement this. */

  virtual void spawnRunner();

  DataStream *const p_data_stream_;

  std::thread runner_thread_ = std::thread();
  std::atomic<ExtensionStatus> status_;

  std::atomic<int> id_ = 0;
  std::string name_ = "";
  std::string type_ = "";
  ExtensionMetadata::Category category_ = ExtensionMetadata::Category::ERROR;
  ExtensionMetadata::Interface interface_ = ExtensionMetadata::Interface::ERROR;
  int update_interval_ = 0;
  std::atomic<int> critical_ = 0;
};

class ExtensionException {
 public:
  ExtensionException(std::string s) : m_s(s){};
  ~ExtensionException(){};

  std::string description() { return m_s; }

 private:
  std::string m_s;
};

/**
 * @brief BMP180 I2C temperature and pressure sensor driver/extension.
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
  interface::I2C i2c_bus_;

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

/**
 * @brief DS18B20 1Wire temperature sensor driver/extension.
 */
class DS18B20 : public Extension {
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

  interface::OneWire one_wire_device_;
};

/**
 * @brief SAM-M8Q I2C GPS sensor driver/extension.
 */
class SAMM8Q : public Extension {
 public:
  SAMM8Q(DataStream *p_data_stream, ExtensionMetadata extension_metadata);
  ~SAMM8Q();
  int runner();

 private:
  bool configure();

  int bus_number_;
  int device_address_;
  interface::I2C i2c_;
  bool configured_ = false;
};

class BME280 : public Extension {
 public:
  BME280(DataStream *p_data_stream, ExtensionMetadata extension_metadata);
  ~BME280();
  int runner();

 private:
  struct RawEnvironmentData {
    uint32_t pressure = 0;
    uint32_t temperature = 0;
    uint16_t humidity = 0;
  };

  struct TempeCompData {
    uint16_t dig_T1 = 0;
    int16_t dig_T2 = 0;
    int16_t dig_T3 = 0;
  };

  struct PressCompData {
    uint16_t dig_P1 = 0;
    int16_t dig_P2 = 0;
    int16_t dig_P3 = 0;
    int16_t dig_P4 = 0;
    int16_t dig_P5 = 0;
    int16_t dig_P6 = 0;
    int16_t dig_P7 = 0;
    int16_t dig_P8 = 0;
    int16_t dig_P9 = 0;
  };

  struct HumCompData {
    uint8_t dig_H1 = 0;
    int16_t dig_H2 = 0;
    uint8_t dig_H3 = 0;
    int16_t dig_H4 = 0;
    int16_t dig_H5 = 0;
    int8_t dig_H6 = 0;
  };

  int handshake();
  int reset();
  int configure();

  int readCompensationData();
  int readData();
  int processData();

  int bus_number_;
  int device_address_;
  interface::I2C i2c_bus_;

  TempeCompData temp_comp_data_ = {};
  PressCompData press_comp_data_ = {};
  HumCompData hum_comp_data_ = {};

  RawEnvironmentData raw_data_ = {};

  float temp_c_ = 0.0;
  float press_mbar_ = 0.0;
  float rh_ = 0.0;
};

/**
 * @brief SYSINFO extension for reading system information.
 * @details Functional on Linux machines. Includes CPU, RAM, and Disk 
 * Information.
 */
class SYSINFO : public Extension {
 public:
  SYSINFO(DataStream *p_data_stream, ExtensionMetadata extension_metadata);
  ~SYSINFO();
  int runner();
};

/**
 * @brief MAX17049 I2C Fuel Gauge driver/extension.
 * @details This was removed from the hardware, it will probably be removed.
 * @todo Remove this extension.
 */
class MAX17049 : public Extension {
 public:
  MAX17049(DataStream *p_data_stream, ExtensionMetadata extension_metadata);
  ~MAX17049();
  int runner();

 private:
  bool handshake();
  bool readData();

  int bus_number_;
  int device_address_;
  interface::I2C i2c_bus_;

  float voltage_ = 0.0;
  float soc_ = 0.0;
};

/**
 * @brief MCP3021 I2C ADC driver/extension for reading voltage.
 * @todo Resistor values should be read from the config file.
 */
class MCP3021 : public Extension {
 public:
  MCP3021(DataStream *p_data_stream, ExtensionMetadata extension_metadata);
  ~MCP3021();
  int runner();

 private:
  int handshake();
  bool ReadVoltage();

  int bus_number_;
  int device_address_;
  interface::I2C i2c_bus_;

  float voltage_ = 0.0;

  const float voltage_reference_;
  float resistor_1_;
  float resistor_2_;
};

/**
 * @brief Raspberry Pi Camera extension.
 * @todo Needs a major overhaul, temporary implementation.
 */
class RaspPiCamera : public Extension {
 public:
  RaspPiCamera(DataStream *p_data_stream, ExtensionMetadata extension_metadata);
  ~RaspPiCamera();
  int runner();

 private:
  std::string CaptureImage();
  // bool CaptureVideo();
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
 * @brief Simulated pressure sensor extension.
 * @details Simulates pressure readings, not based on real data/physics.
 */
class PRESS_SENSOR_SIM : public Extension {
 public:
  PRESS_SENSOR_SIM(DataStream *p_data_stream,
                   ExtensionMetadata extension_metadata);
  ~PRESS_SENSOR_SIM();
  int runner();

 private:
  float pressure_ = 1012.00;

  std::time_t start_time_ = 0;
  const float seconds_to_apogee_ = 3600;
  const float seconds_to_landing_ = 1000;

  const float apogee_pressure_ = 0.0;
  const float landing_pressure_ = 1012.00;

  float pressure_delta_up_ =
      (landing_pressure_ - apogee_pressure_) / seconds_to_apogee_;
  float pressure_delta_down_ =
      (apogee_pressure_ - landing_pressure_) / seconds_to_landing_;
};

/**
 * @brief Simulated temperature sensor extension.
 * @details Not based on real data/physics.
 */
class TEMP_SENSOR_SIM : public Extension {
 public:
  TEMP_SENSOR_SIM(DataStream *p_data_stream,
                  ExtensionMetadata extension_metadata);
  ~TEMP_SENSOR_SIM();
  int runner();
};

/**
 * @brief No longer used, to be removed.
 * @todo Remove this extension.
 */
class RADIO_SIM : public Extension {
 public:
  RADIO_SIM(DataStream *p_data_stream, ExtensionMetadata extension_metadata);
  ~RADIO_SIM();
  int runner();
};

/**
 * @brief Simulated GPS extension, not based on real data/physics.
 * @todo Needs to be updated to use the new GPSFrame data structure.
 */
class GPS_SIM : public Extension {
 public:
  GPS_SIM(DataStream *p_data_stream, ExtensionMetadata extension_metadata);
  ~GPS_SIM();
  int runner();

 private:
  float latitude_ = 38.686823;
  float longitude_ = -110.989557;
  float altitude_ = 0.0;
};

/**
 * @brief Simulated battery sensor extension.
 * @todo Verify compatibility with critical data structures.
 */
class BATT_SENSOR_SIM : public Extension {
 public:
  BATT_SENSOR_SIM(DataStream *p_data_stream,
                  ExtensionMetadata extension_metadata);
  ~BATT_SENSOR_SIM();
  int runner();
};

}  // namespace extension

#endif  // EXTENSION_H_