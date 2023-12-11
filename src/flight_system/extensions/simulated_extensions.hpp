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

#include "extension_base.hpp"

namespace extension {
class SimTemperatureSensor : public Extension {
public:
  SimTemperatureSensor(ExtensionResources &resources,
                       cfg::ExtensionMetadata metadata)
      : Extension(resources, metadata) {
  }

  void loop() override {
    data(data::DataId::ENVIRONMENTAL_temperature, temperature_);
    temperature_ += increasing_ ? 0.1 : -0.1;

    if (temperature_ > 30) {
      increasing_ = false;
    } else if (temperature_ < -30) {
      increasing_ = true;
    }
  }

private:
  double temperature_ = 20.0;
  bool increasing_ = false;
};

class SimPressureSensor : public Extension {
public:
  SimPressureSensor(ExtensionResources &resources,
                    cfg::ExtensionMetadata metadata)
      : Extension(resources, metadata) {
  }

  void loop() override {
    data(data::DataId::ENVIRONMENTAL_pressure, pressure_);
    pressure_ += increasing_ ? 0.5 : -0.5;

    if (pressure_ > 1000) {
      increasing_ = false;
    } else if (pressure_ < 100) {
      increasing_ = true;
    }
  }

private:
  double pressure_ = 1000.0;
  bool increasing_ = false;
};

class SimHumiditySensor : public Extension {
public:
  SimHumiditySensor(ExtensionResources &resources,
                    cfg::ExtensionMetadata metadata)
      : Extension(resources, metadata) {
  }

  void loop() override {
    data(data::DataId::ENVIRONMENTAL_humidity, rh_);
    rh_ += increasing_ ? 0.1 : -0.1;

    if (rh_ > 99) {
      increasing_ = false;
    } else if (rh_ < 2) {
      increasing_ = true;
    }
  }

private:
  double rh_ = 100.0;
  bool increasing_ = false;
};

class SimGpsSensor : public Extension {
public:
  SimGpsSensor(ExtensionResources &resources, cfg::ExtensionMetadata metadata)
      : Extension(resources, metadata) {
    gps_frame.gps_utc_time = BoosterSeat::clck::now();
    gps_frame.fix = data::GpsFix::FIX_3D;
    gps_frame.num_satellites = 10;
    gps_frame.latitude = 40.0;
    gps_frame.longitude = -80.0;
    gps_frame.horz_accuracy = 5.0;
    gps_frame.altitude = 1000.0;
    gps_frame.vert_accuracy = 5.0;
    gps_frame.vertical_speed = 0.0;
    gps_frame.horizontal_speed = 0.0;
    gps_frame.speed_accuracy = 0.0;
    gps_frame.heading_of_motion = 0.0;
    gps_frame.heading_accuracy = 0.0;
  }

  void loop() override {
    gps_frame.gps_utc_time = BoosterSeat::clck::now();
    gps_frame.latitude += increasing_ ? 0.0001 : -0.0001;
    gps_frame.longitude += increasing_ ? 0.0001 : -0.0001;
    gps_frame.altitude += increasing_ ? 0.1 : -0.1;
    data(gps_frame);
  }

private:
  data::GpsFrame gps_frame{};
  bool increasing_ = true;
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
} // namespace extension

#endif /* SIMULATED_EXTENSIONS_HPP_ */