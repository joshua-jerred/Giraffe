#include <filesystem>

#include "data_module.hpp"
#include "to_string.hpp"

static modules::MetaData metadata("data_module",
                                  node::Identification::DATA_MODULE, 100);

modules::DataModule::DataModule(data::SharedData &shared_data,
                                cfg::Configuration &config)
    : modules::Module(metadata, shared_data, config),
      data_log_(shared_data, config), influxdb_(shared_data, config) {
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

  influxdb_enabled_ = configuration_.data_module_influxdb.getInfluxEnabled();

  processAllStreams();

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
  } else if (type == data::DataId::CAMERA_newImagePath) {
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
