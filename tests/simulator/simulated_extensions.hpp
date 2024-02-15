/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   simulated_extensions.hpp
 * @brief  This contains the simulated extensions for testing the
 * flight system.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-06-13
 * @copyright  2023 (license to be defined)
 */

#ifndef SIMULATED_EXTENSIONS_HPP_
#define SIMULATED_EXTENSIONS_HPP_

#include "gfs_simulator.hpp"

#include "extension_base.hpp"

namespace extension {

static const std::vector<cfg::ExtensionMetadata> K_SIMULATED_EXTENSIONS_VEC = {
    cfg::ExtensionMetadata{
        "sim_temp", true, cfg::gEnum::ExtensionType::SIM_TEMP, 1000, false, ""},
    cfg::ExtensionMetadata{
        "sim_pres", true, cfg::gEnum::ExtensionType::SIM_PRES, 1000, false, ""},
    cfg::ExtensionMetadata{"sim_hum", true, cfg::gEnum::ExtensionType::SIM_HUM,
                           1000, false, ""},
    cfg::ExtensionMetadata{"sim_gps", true, cfg::gEnum::ExtensionType::SIM_GPS,
                           1000, false, ""},
    cfg::ExtensionMetadata{"sim_imu", true, cfg::gEnum::ExtensionType::SIM_IMU,
                           1000, false, ""},
    cfg::ExtensionMetadata{"sim_adc", true, cfg::gEnum::ExtensionType::SIM_ADC,
                           1000, false, ""},
};

class SimTemperatureSensor : public Extension {
public:
  SimTemperatureSensor(ExtensionResources &resources,
                       cfg::ExtensionMetadata metadata)
      : Extension(resources, metadata) {
  }
  void loop() override {
    double temp = g_GFS_SIMULATOR.getTemperatureC();
    data(data::DataId::ENVIRONMENTAL_temperature, temp);
  }
};

class SimPressureSensor : public Extension {
public:
  SimPressureSensor(ExtensionResources &resources,
                    cfg::ExtensionMetadata metadata)
      : Extension(resources, metadata) {
  }
  void loop() override {
    double pressure = g_GFS_SIMULATOR.getPressureMbar();
    data(data::DataId::ENVIRONMENTAL_pressure, pressure);
  }
};

class SimHumiditySensor : public Extension {
public:
  SimHumiditySensor(ExtensionResources &resources,
                    cfg::ExtensionMetadata metadata)
      : Extension(resources, metadata) {
  }

  void loop() override {
    double rh = g_GFS_SIMULATOR.getHumidityPercent();
    data(data::DataId::ENVIRONMENTAL_humidity, rh);
  }
};

class SimGpsSensor : public Extension {
public:
  SimGpsSensor(ExtensionResources &resources, cfg::ExtensionMetadata metadata)
      : Extension(resources, metadata) {
  }

  void loop() override {
    data::GpsFrame gps_frame{};

    gps_frame.gps_utc_time = bst::clck::now();
    gps_frame.is_valid = true;
    gps_frame.fix = data::GpsFix::FIX_3D;
    gps_frame.num_satellites = 10;
    gps_frame.latitude = g_GFS_SIMULATOR.getLatitudeDeg();
    gps_frame.longitude = g_GFS_SIMULATOR.getLongitudeDeg();
    gps_frame.horz_accuracy = 5.0;
    gps_frame.altitude = g_GFS_SIMULATOR.getAltitudeM();
    gps_frame.vert_accuracy = 1.0;
    gps_frame.vertical_speed = g_GFS_SIMULATOR.getVertVelocityMps();
    gps_frame.horizontal_speed = g_GFS_SIMULATOR.getHorzVelocityMps();
    gps_frame.speed_accuracy = 1.0;
    gps_frame.heading_of_motion = g_GFS_SIMULATOR.getHeadingDeg();
    gps_frame.heading_accuracy = 1.0;
    data(gps_frame);
  }
};

class SimImuSensor : public Extension {
public:
  SimImuSensor(ExtensionResources &resources, cfg::ExtensionMetadata metadata)
      : Extension(resources, metadata) {
  }

  void loop() override {
    data::ImuFrame frame;
    frame.x_acceleration = 0.0;
    frame.y_acceleration = 0.0;
    frame.z_acceleration = 0.0;
    frame.x_angular_velocity = 0.0;
    frame.y_angular_velocity = 0.0;
    frame.z_angular_velocity = 0.0;
    data(frame);
  }

private:
  bool increasing_ = false;
};

class SimAdcSensor : public Extension {
public:
  SimAdcSensor(ExtensionResources &resources, cfg::ExtensionMetadata metadata)
      : Extension(resources, metadata) {
  }

  void loop() override {
  }

private:
};

} // namespace extension

#endif /* SIMULATED_EXTENSIONS_HPP_ */