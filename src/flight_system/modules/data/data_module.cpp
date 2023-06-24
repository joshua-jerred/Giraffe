#include "data_module.h"

static modules::MetaData metadata("data_module",
                                  node::Identification::DATA_MODULE, 100);

modules::DataModule::DataModule(data::SharedData &shared_data,
                                cfg::Configuration &config)
    : modules::Module(metadata, shared_data, config),
      data_log_(shared_data, config), influxdb_(shared_data, config) {
}

void modules::DataModule::startup() {
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
  influxdb_enabled_ = configuration_.data_module_influxdb.getInfluxEnabled();

  processAllStreams();

  if (data_file_enabled_ &&
      (data_file_logging_strategy_ == cfg::gEnum::LogStrategy::INTERVAL ||
       data_file_logging_strategy_ ==
           cfg::gEnum::LogStrategy::SELECTION_INTERVAL)) {
    data_log_.logDataFrame(
        data_file_logging_strategy_); // timer taken care of inside of data_log_
  }
}

void modules::DataModule::shutdown() {
  processAllStreams();
}

void modules::DataModule::processCommand(const cmd::Command &command) {
  (void)command;
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
  if (packet.type == data::DataPacket::Type::GENERIC) {
    parseGeneralDataPacket(packet);
  } else if (packet.type == data::DataPacket::Type::STATUS) {
    parseStatusDataPacket(packet);
  } else {
    error(data::LogId::DATA_MODULE_dataPacketUnknownType, (int)packet.type);
  }
}

void modules::DataModule::parseGeneralDataPacket(
    const data::DataPacket &packet) {
  (void)packet;
  // Process packet here
}

void modules::DataModule::parseExtensionDataPacket(
    const data::DataPacket &packet) {
  (void)packet;
  // Process packet here
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
  default:
    error(data::LogId::DATA_MODULE_statusDataPacketUnknownSource,
          (int)packet.source);
    return;
  }

  shared_data_.blocks.modules_statuses.set(statuses);
}

// ------------------ Log Stream Parsing ------------------

void modules::DataModule::processLogPacket(const data::LogPacket &packet) {
  // Log log packet to file (if enabled)
  if (log_file_enabled_) {
    data_log_.logLogPacket(packet);
  }

  // Log log packet to influxdb (if enabled)
  if (influxdb_enabled_) {
    influxdb_.logLogPacket(packet);
  }

  // Process packet here
}

// ------------------ GPS Stream Parsing ------------------
void modules::DataModule::processGpsFramePacket(
    const data::GpsFramePacket &packet) {
  data::blocks::LocationData location_data =
      shared_data_.blocks.location_data.get();

  location_data.last_gps_frame = packet.frame;
  location_data.current_gps_fix = packet.frame.fix;

  if (isGpsFrameValid(packet.frame)) {
    location_data.last_valid_gps_frame = packet.frame;
    location_data.last_valid_gps_fix = packet.frame.fix;
  }
}

// ------------------ IMU Stream Parsing ------------------
void modules::DataModule::processImuFramePacket(
    const data::ImuFramePacket &packet) {
  (void)packet;
}
