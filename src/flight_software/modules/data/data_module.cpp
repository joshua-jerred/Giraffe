#include "data_module.h"

#include "time_types.hpp"

static modules::MetaData metadata("data_module",
                                  node::Identification::DATA_MODULE, 100);

modules::DataModule::DataModule(data::SharedData &shared_data,
                                cfg::Configuration &config)
    : modules::Module(metadata, shared_data, config) {}

modules::DataModule::~DataModule() {}

void modules::DataModule::startup() {
  sleep(); // wait to start
}

void modules::DataModule::loop() {
  parseDataStream();
  parseLogStream();
}

void modules::DataModule::shutdown() {}

void modules::DataModule::processCommand(const command::Command &command) {
  (void)command;
}

// ------------------ Data Stream Parsing ------------------

void modules::DataModule::parseDataStream() {
  int packet_count = shared_data_.streams.data.getNumPackets();
  data::DataPacket packet;
  bool first = true;
  int delay_ms = 0;
  for (int i = 0; i < packet_count; i++) {
    bool got_packet = shared_data_.streams.data.getPacket(packet);
    if (!got_packet)
      return;
    if (first) {
      delay_ms = giraffe_time::millisecondsElapsed(packet.created_time);
      first = false;
    }

    if (packet.type == data::DataPacket::Type::GENERIC) {
      parseGeneralDataPacket(packet);
    } else if (packet.type == data::DataPacket::Type::STATUS) {
      parseStatusDataPacket(packet);
    } else {
      error(data::LogId::DATA_MODULE_dataPacketUnknownType, (int)packet.type);
    }
  }

  data::blocks::StreamsStats::StreamStats stats_;
  stats_.current_packets = packet_count;
  stats_.total_packets = shared_data_.streams.data.getTotalPackets();
  stats_.processing_delay_ms = delay_ms;

  data::blocks::StreamsStats streams_statuses_ =
      shared_data_.blocks.stream_stats.get();
  streams_statuses_.data = stats_;
  shared_data_.blocks.stream_stats.set(streams_statuses_);
}

void modules::DataModule::parseGeneralDataPacket(
    const data::DataPacket &packet) {
  (void) packet;
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
  default:
    error(data::LogId::DATA_MODULE_statusDataPacketUnknownSource,
          (int)packet.source);
    return;
  }

  shared_data_.blocks.modules_statuses.set(statuses);
}

// ------------------ Log Stream Parsing ------------------

void modules::DataModule::parseLogStream() {
  int packet_count = shared_data_.streams.log.getNumPackets();
  data::LogPacket packet;
  bool first = true;
  int delay_ms = 0;
  for (int i = 0; i < packet_count; i++) {
    bool got_packet = shared_data_.streams.log.getPacket(packet);
    if (!got_packet)
      return;
    if (first) {
      delay_ms = giraffe_time::millisecondsElapsed(packet.created_time);
      first = false;
    }

    // Process packet here
  }

  data::blocks::StreamsStats::StreamStats stats_;
  stats_.current_packets = packet_count;
  stats_.total_packets = shared_data_.streams.log.getTotalPackets();
  stats_.processing_delay_ms = delay_ms;

  data::blocks::StreamsStats streams_statuses_ =
      shared_data_.blocks.stream_stats.get();
  streams_statuses_.log = stats_;
  shared_data_.blocks.stream_stats.set(streams_statuses_);
}