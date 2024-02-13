#include <filesystem>

#include "data_module.hpp"
#include "to_string.hpp"

static modules::MetaData metadata("data_module",
                                  node::Identification::DATA_MODULE, 100);

modules::DataModule::DataModule(data::SharedData &shared_data,
                                cfg::Configuration &config)
    : modules::Module(metadata, shared_data, config),
      data_log_(shared_data, config), influxdb_(shared_data, config) {

  // check for GPS data source
  auto ext_cfg = configuration_.extensions.getExtensions();
  bool have_gps_source = false;
  for (auto &ext_meta : ext_cfg) {
    if (ext_meta.type == cfg::gEnum::ExtensionType::SIM_GPS ||
        ext_meta.type == cfg::gEnum::ExtensionType::SAM_M8Q) {
      have_gps_source = true;
      break;
    }
  }

  if (!have_gps_source) {
    error(DiagnosticId::DATA_MODULE_noGpsSource);
  }
}

void modules::DataModule::startup() {
  console_module_enabled_ = configuration_.console_module.getEnabled();
  /** @todo why the call to sleep()? */
  sleep(); // wait to start
}

void modules::DataModule::loop() {
  data_file_enabled_ = configuration_.data_module_data.getLogDataToFile();
  if (data_file_enabled_) {
    data_file_logging_strategy_ =
        configuration_.data_module_data.getLogStrategy();
  }
  log_file_enabled_ = configuration_.data_module_log.getLogToFile();
  if (log_file_enabled_) {
    error_file_logging_strategy_ =
        configuration_.data_module_log.getErrorLogStrategy();
  }

  processAllStreams();

  calculateCalculatedData();

  if (data_file_enabled_ &&
      (data_file_logging_strategy_ == cfg::gEnum::LogStrategy::INTERVAL ||
       data_file_logging_strategy_ ==
           cfg::gEnum::LogStrategy::SELECTION_INTERVAL)) {
    data_log_.logDataFrame(
        data_file_logging_strategy_); // timer taken care of inside of data_log_
  }

  if (log_file_enabled_ && error_file_logging_strategy_ ==
                               cfg::gEnum::ErrorLogStrategy::ERROR_FRAME) {
    data_log_.logErrorFrame();
  }

  influxdb_enabled_ = configuration_.data_module_influxdb.getInfluxEnabled();
  if (influxdb_enabled_) {
    influxdb_.writeFrames(); // timer taken care of inside of influxdb_
  }
}

void modules::DataModule::shutdown() {
  // If the logging strategy is all, then this will log the remaining packets.
  processAllStreams();

  // If the logging strategy is interval, then this will log the frame
  if (data_file_enabled_ &&
      data_file_logging_strategy_ == cfg::gEnum::LogStrategy::INTERVAL) {
    data_log_.logDataFrame(data_file_logging_strategy_);
  }

  // Same as above, but for error frames
  if (log_file_enabled_ && error_file_logging_strategy_ ==
                               cfg::gEnum::ErrorLogStrategy::ERROR_FRAME) {
    data_log_.logErrorFrame();
  }
}

void modules::DataModule::processCommand(const cmd::Command &command) {
  switch (command.command_id) {
  case cmd::CommandId::DATA_MODULE_clearAllErrors:
    shared_data_.frames.error_frame.clearAllErrors();
    break;
  case cmd::CommandId::DATA_MODULE_clearSpecificError:
    /** @todo Test this, this could end poorly **/
    shared_data_.frames.error_frame.clearError(
        static_cast<DiagnosticId>(command.int_arg));
    break;
  default:
    error(DiagnosticId::DATA_MODULE_unrecognizedCommand);
    break;
  }
}

void modules::DataModule::processAllStreams() {
  data::blocks::StreamsStats stats{};

  auto data_packet_parser =
      std::bind(&DataModule::processDataPacket, this, std::placeholders::_1);
  processStream<data::DataPacket>(
      dynamic_cast<data::Stream<data::DataPacket> &>(shared_data_.streams.data),
      stats.data, data_packet_parser);

  auto log_packet_parser =
      std::bind(&DataModule::processLogPacket, this, std::placeholders::_1);
  processStream<data::LogPacket>(
      dynamic_cast<data::Stream<data::LogPacket> &>(shared_data_.streams.log),
      stats.log, log_packet_parser);

  auto gps_frame_packet_parser = std::bind(&DataModule::processGpsFramePacket,
                                           this, std::placeholders::_1);
  processStream<data::GpsFramePacket>(
      dynamic_cast<data::Stream<data::GpsFramePacket> &>(
          shared_data_.streams.gps),
      stats.gps, gps_frame_packet_parser);

  auto imu_frame_packet_parser = std::bind(&DataModule::processImuFramePacket,
                                           this, std::placeholders::_1);
  processStream<data::ImuFramePacket>(
      dynamic_cast<data::Stream<data::ImuFramePacket> &>(
          shared_data_.streams.imu),
      stats.imu, imu_frame_packet_parser);

  stats.command.current_packets = shared_data_.streams.command.getNumPackets();
  stats.command.total_packets = shared_data_.streams.command.getTotalPackets();

  // Each process function call updates this struct, so update the shared data
  // here
  shared_data_.blocks.stream_stats.set(stats);
}

// ------------------ Data Stream Parsing ------------------

void modules::DataModule::processDataPacket(const data::DataPacket &packet) {
  // Log data packet to file (if enabled)
  if (data_file_enabled_ &&
      data_file_logging_strategy_ == cfg::gEnum::LogStrategy::ALL) {
    data_log_.logDataPacket(packet);
  }

  // Log data packet to influxdb (if enabled)
  if (influxdb_enabled_) {
    influxdb_.logDataPacket(packet);
  }

  if (packet.source == node::Identification::EXTENSION) {
    parseExtensionDataPacket(packet);
  } else if (packet.type == data::DataPacket::Type::GENERIC) {
    parseGeneralDataPacket(packet);
  } else if (packet.type == data::DataPacket::Type::STATUS) {
    parseStatusDataPacket(packet);
  } else {
    error(DiagnosticId::DATA_MODULE_dataPacketUnknownType, (int)packet.type);
  }
}

void modules::DataModule::parseGeneralDataPacket(
    const data::DataPacket &packet) {
  (void)packet;
  /// @todo Implement general data packet parsing
  // Process packet here
}

void modules::DataModule::parseExtensionDataPacket(
    const data::DataPacket &packet) {
  auto ext_id = packet.secondary_identifier;
  auto type = packet.identifier;

  if (type == data::DataId::ENVIRONMENTAL_temperature) {
    shared_data_.frames.env_temp.insert(ext_id, packet);
  } else if (type == data::DataId::ENVIRONMENTAL_pressure) {
    shared_data_.frames.env_pres.insert(ext_id, packet);
  } else if (type == data::DataId::ENVIRONMENTAL_humidity) {
    shared_data_.frames.env_hum.insert(ext_id, packet);
  } else if (type == data::DataId::CAMERA_newImage) {
    parseCameraNewImageDataPacket(packet);
  } else {
    giraffe_assert(false);
  }
}

void modules::DataModule::parseStatusDataPacket(
    const data::DataPacket &packet) {
  data::blocks::ModulesStatuses statuses =
      shared_data_.blocks.modules_statuses.get();

  switch (packet.source) {
  case node::Identification::DATA_MODULE:
    statuses.data = packet.node_status;
    break;
  case node::Identification::CONSOLE_MODULE:
    statuses.console = packet.node_status;
    break;
  case node::Identification::SERVER_MODULE:
    statuses.server = packet.node_status;
    break;
  case node::Identification::SYSTEM_MODULE:
    statuses.system = packet.node_status;
    break;
  case node::Identification::EXTENSION_MODULE:
    statuses.extension = packet.node_status;
    break;
  case node::Identification::TELEMETRY_MODULE:
    statuses.telemetry = packet.node_status;
    break;
  default:
    error(DiagnosticId::DATA_MODULE_statusDataPacketUnknownSource,
          (int)packet.source);
    return;
  }

  shared_data_.blocks.modules_statuses.set(statuses);
}

void modules::DataModule::parseCameraNewImageDataPacket(
    const data::DataPacket &packet) {
  if (packet.source != node::Identification::EXTENSION) {
    error(DiagnosticId::DATA_MODULE_cameraNewImagePacketInvalidFields,
          "source");
    return;
  }
  std::string path = packet.value;
  std::filesystem::path p(path);
  if (!std::filesystem::exists(p) || !std::filesystem::is_regular_file(p)) {
    error(DiagnosticId::DATA_MODULE_cameraNewImagePacketInvalidPath, path);
    return;
  }

  auto camera_block = shared_data_.blocks.camera.get();
  camera_block.have_camera_source = true;
  camera_block.last_valid_image_path = path;
  camera_block.num_images++;
  shared_data_.blocks.camera.set(camera_block);
}

// ------------------ Log Stream Parsing ------------------

void modules::DataModule::processLogPacket(const data::LogPacket &packet) {

  if (console_module_enabled_) {
    shared_data_.log_container.add(util::to_string(packet));
  }

  // Log log packet to file (if enabled)
  if (log_file_enabled_ &&
      error_file_logging_strategy_ == cfg::gEnum::ErrorLogStrategy::ALL) {
    data_log_.logLogPacket(packet);
  }

  // Log log packet to influxdb (if enabled)
  if (influxdb_enabled_) {
    influxdb_.logLogPacket(packet);
  }

  // Add to the error frame if it is an error
  if (packet.level == data::LogPacket::Level::ERROR) {
    shared_data_.frames.error_frame.addError(packet);
  }

  /**
   * @todo debug/log packets should be processed here
   */
}

// ------------------ GPS Stream Parsing ------------------
void modules::DataModule::processGpsFramePacket(
    const data::GpsFramePacket &packet) {
  data::blocks::LocationData location_data =
      shared_data_.blocks.location_data.get();
  location_data.have_gps_source = true;

  location_data.last_gps_frame = packet.frame;
  location_data.current_gps_fix = packet.frame.fix;

  if (isGpsFrameValid(packet.frame)) {
    location_data.last_valid_gps_frame = packet.frame;
    location_data.last_valid_gps_fix = packet.frame.fix;
  }

  shared_data_.blocks.location_data.set(location_data);
}

// ------------------ IMU Stream Parsing ------------------
void modules::DataModule::processImuFramePacket(
    const data::ImuFramePacket &packet) {
  auto data_block = shared_data_.blocks.imu_data.get();
  data_block.have_imu_source = true;

  data_block.most_recent_imu_frame = packet.frame;
  if (isImuFrameValid(packet.frame)) {
    data_block.last_valid_imu_frame = packet.frame;
  }

  shared_data_.blocks.imu_data.set(data_block);
}

/**
 * @brief Calculates the altitude in meters from the pressure in hPa.
 * @param pressure The pressure in hPa.
 * @return double The altitude in meters.
 *
 * @todo Detect initial launch (movement from the launch point).
 */
inline double calculatePressureAltitude(double pressure) {
  return (1 - std::pow(pressure / 1013.25, 0.190284)) * 145366.45 * 0.3048;
}

void modules::DataModule::calculateCalculatedData() {
  // Calculate pressure altitude
  auto pres_data = shared_data_.frames.env_pres.getAll();

  if (pres_data.size() == 0) {
    calculated_data_.pressure_altitude_valid = false;
  } else {
    std::string pres_string = pres_data.begin()->second.value;
    try {
      double pres = std::stod(pres_string);
      calculated_data_.pressure_altitude_m = calculatePressureAltitude(pres);
      calculated_data_.pressure_altitude_valid = true;
    } catch (std::invalid_argument &e) {
      calculated_data_.pressure_altitude_valid = false;
    }
  }

  // Calculate distance traveled and the distance from the launch point
  /// @brief The distance that must be traveled for the distance traveled to be
  /// updated.
  constexpr double MOVEMENT_THRESHOLD_KM = 0.25;

  auto gps_data = shared_data_.blocks.location_data.get();

  if (!launch_gps_point_set_) { // No launch point set yet

    if (gps_data.last_valid_gps_frame.fix == data::GpsFix::FIX_2D ||
        gps_data.last_valid_gps_frame.fix == data::GpsFix::FIX_3D) {
      try {
        launch_gps_point_ =
            bst::geo::Point(gps_data.last_valid_gps_frame.latitude,
                            gps_data.last_valid_gps_frame.longitude);
        last_gps_point_ = launch_gps_point_;
        launch_gps_point_set_ = true;
        info("Launch point set to: " +
             std::to_string(launch_gps_point_.latitude()) + ", " +
             std::to_string(launch_gps_point_.longitude()));
        /// @todo Report the launch position as a data point
      } catch (const bst::BoosterSeatException &e) {
        /// @todo Report this error
      }
    }

  } else if (gps_data.last_valid_gps_frame.fix == data::GpsFix::FIX_2D ||
             gps_data.last_valid_gps_frame.fix == data::GpsFix::FIX_3D) {
    try {
      bst::geo::Point current_point(gps_data.last_valid_gps_frame.latitude,
                                    gps_data.last_valid_gps_frame.longitude);
      double new_distance = bst::geo::distance(last_gps_point_, current_point);
      if (new_distance >= MOVEMENT_THRESHOLD_KM) {
        distance_traveled_ += new_distance;
        last_gps_point_ = current_point;

        calculated_data_.distance_traveled_m = distance_traveled_ * 1000;
        calculated_data_.distance_traveled_valid = true;
      }

      // distance from launch point
      calculated_data_.distance_from_launch_m =
          bst::geo::distance(launch_gps_point_, current_point) *
          1000; // convert to meters
      calculated_data_.distance_from_launch_valid = true;
    } catch (const bst::BoosterSeatException &e) {
      /// @todo Report this error
    }

    // Maximum speeds
    if (gps_data.last_valid_gps_frame.horizontal_speed >
        calculated_data_.max_horizontal_speed_mps) {
      calculated_data_.max_horizontal_speed_mps =
          gps_data.last_valid_gps_frame.horizontal_speed;
      calculated_data_.max_speed_valid = true;
    }

    if (gps_data.last_valid_gps_frame.vertical_speed >
        calculated_data_.max_vertical_speed_mps) {
      calculated_data_.max_vertical_speed_mps =
          gps_data.last_valid_gps_frame.vertical_speed;
      calculated_data_.max_speed_valid = true;
    }

    // Average speed
    if (average_speed_timer_.isDone()) {
      calculated_data_.average_speed_valid = true;
      average_speed_timer_.reset();

      average_horizontal_speed_.addValue(
          gps_data.last_valid_gps_frame.horizontal_speed);
      average_vertical_speed_.addValue(
          gps_data.last_valid_gps_frame.vertical_speed);
    }

    calculated_data_.average_horiz_speed_mps_5min =
        average_horizontal_speed_.getAverage();
    calculated_data_.average_vert_speed_mps_5min =
        average_vertical_speed_.getAverage();

  } else {
    /// @todo put this on a timer instead. Otherwise it will be reported as
    /// invalid if there is a single invalid gps frame. Reset the timer if
    /// there is a valid frame.

    calculated_data_.distance_traveled_valid = false;
    calculated_data_.distance_from_launch_valid = false;
    calculated_data_.average_speed_valid = false;
  }

  shared_data_.blocks.calculated_data.set(calculated_data_);
}