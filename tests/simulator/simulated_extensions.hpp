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

#include <BoosterSeat/exception.hpp>
#include <BoosterSeat/filesystem.hpp>

#include "gfs_simulator.hpp"
#include "giraffe_file_paths.hpp"

#include "extension_base.hpp"

namespace extension {

/// @brief If RUN_IN_SIMULATOR is defined, the simulated extensions listed in
/// this vector will be added to the extension module on startup.
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
    cfg::ExtensionMetadata{"sim_camera", true,
                           cfg::gEnum::ExtensionType::SIM_CAMERA, 10000, false,
                           ""},
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

/// @brief Simulated ADC sensor with battery voltage, battery balance tap
/// voltage, and current sense voltage.
/// @details
/// Battery voltage:
///  - Real range: 0.0 to 9.0 volts
///  - ADC Voltage divider: 10k/5.6k - 9.0 volts = 3.231 volts
///  - ADC resolution: 12 bits
///  - 0.0 volts = 0 ADC count
///  - 9.0 volts = 4009 ADC count
class SimAdcSensor : public Extension {
public:
  SimAdcSensor(ExtensionResources &resources, cfg::ExtensionMetadata metadata)
      : Extension(resources, metadata) {
  }

  void loop() override {
    const double battery_voltage = g_GFS_SIMULATOR.getBatteryVoltage();

    // Calculate the ADC count
    constexpr double K_BATTERY_VOLTAGE_R1 = 10000.0;
    constexpr double K_BATTERY_VOLTAGE_R2 = 5600.0;
    const double adc_voltage_battery_voltage =
        (battery_voltage * K_BATTERY_VOLTAGE_R2) /
        (K_BATTERY_VOLTAGE_R1 + K_BATTERY_VOLTAGE_R2);
    const uint32_t adc_count_battery_voltage =
        static_cast<uint32_t>((adc_voltage_battery_voltage / 3.3) * 4095);

    // Create the data packet
    constexpr uint32_t K_BATTERY_VOLTAGE_ADC_CHANNEL = 0;
    dataWithSecondaryIdentifier(data::DataId::ADC_count,
                                adc_count_battery_voltage,
                                std::to_string(K_BATTERY_VOLTAGE_ADC_CHANNEL));
  }

private:
  // uint32_t battery_ballance_adc_count_{0};
  // uint32_t current_sense_adc_count_{0};
};

class SimCamera : public Extension {
public:
  SimCamera(ExtensionResources &resources, cfg::ExtensionMetadata metadata)
      : Extension(resources, metadata) {
    image_dir_ = giraffe::file_paths::getGfsImageDirPath();
  }

  void loop() override {
    copySampleImage();
  }

  void copySampleImage() {
    const std::string TEST_IMAGE_PATH = "./simulated_camera_image.png";
    const std::string TEST_IMAGE_OUTPUT_PATH =
        image_dir_ + "/" + TEST_IMAGE_PATH;
    // std::string new_image_name =
    // giraffe::file_paths::generateFileNameWithTimestamp(".png");

    if (image_created_) {
      data(data::DataId::CAMERA_newImage, TEST_IMAGE_OUTPUT_PATH);
      return;
    }

    try {
      bst::filesystem::copyFile(TEST_IMAGE_PATH, TEST_IMAGE_OUTPUT_PATH, true);
    } catch (bst::BoosterSeatException &e) {
      error(DiagnosticId::EXTENSION_cameraCaptureFail, e.what());
    }
    image_created_ = true;
  }

private:
  std::string image_dir_{};
  bool image_created_{false};
};

} // namespace extension

#endif /* SIMULATED_EXTENSIONS_HPP_ */