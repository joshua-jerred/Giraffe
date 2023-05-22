#include "data_module.h"

static modules::MetaData metadata("data_module",
                                  node::Identification::DATA_MODULE);

modules::DataModule::DataModule(data::Streams &streams,
                                cfg::Configuration &config)
    : modules::Module(metadata, streams, config) {
}

modules::DataModule::~DataModule() {
}

void modules::DataModule::startup() {
  sleep(); // wait to start
}

void modules::DataModule::loop() {
  parseStreams();
}

void modules::DataModule::shutdown() {
}

void modules::DataModule::processCommand(const command::Command &command) {
  (void)command;
}

void modules::DataModule::parseStreams() {
  int data_packet_count = streams_.data_stream.getNumPackets();
  data::DataStreamPacket ds_packet;
  for (int i = 0; i < data_packet_count; i++) {
    bool got_packet = streams_.data_stream.getPacket(ds_packet);
    if (!got_packet) return;
    std::string source = node::identification_to_string.at(ds_packet.source);
    std::cout << source << ":" << ds_packet.identifier << ":" << ds_packet.value
              << std::endl;
  }

  int error_packet_count = streams_.error_stream.getNumPackets();
  data::ErrorStreamPacket es_packet;
  for (int i = 0; i < error_packet_count; i++) {
    bool got_packet = streams_.error_stream.getPacket(es_packet);
    if (!got_packet) return;
    std::string source = node::identification_to_string.at(es_packet.source);

    std::cout << source << ":" << es_packet.code << " - " << es_packet.info
              << std::endl;
  }
}
