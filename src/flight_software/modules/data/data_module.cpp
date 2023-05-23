#include "data_module.h"

#include "time_types.h"

static modules::MetaData metadata("data_module",
                                  node::Identification::DATA_MODULE);

modules::DataModule::DataModule(data::SharedData &shared_data,
                                cfg::Configuration &config)
    : modules::Module(metadata, shared_data, config) {
}

modules::DataModule::~DataModule() {
}

void modules::DataModule::startup() {
  sleep();  // wait to start
}

void modules::DataModule::loop() {
  parseStreams();
  data<std::string>("test", "t1");
  error("code");
}

void modules::DataModule::shutdown() {
}

void modules::DataModule::processCommand(const command::Command &command) {
  (void)command;
}

template <class PKT>
void modules::DataModule::parseStream(data::Stream<PKT> &stream,
                                      std::function<void(PKT &)> callback) {
  int packet_count = stream.getNumPackets();
  PKT packet;
  bool first = true;
  int delay_ms = 0;
  for (int i = 0; i < packet_count; i++) {
    bool got_packet = stream.getPacket(packet);
    if (!got_packet) return;
    if (first) {
      delay_ms = giraffe_time::millisecondsElapsed(packet.created_time);
      first = false;
    }
    callback(packet);
  }

  // detect it's stream type, set it's stats.
  data::blocks::StreamsStats streams_statuses_ =
      shared_data_.blocks.stream_stats.get();

  data::blocks::StreamsStats::StreamStats stats_;
  stats_.current = packet_count;
  stats_.total = stream.getTotalPackets();
  stats_.processing_delay_ms = delay_ms;
  // this could be easier by passing in a reference to the values, but
  // I wanted to play around with if constexpr
  if constexpr (std::is_same<PKT, data::DataStreamPacket>::value) {
    streams_statuses_.data = stats_;
  } else if (std::is_same<PKT, data::ErrorStreamPacket>::value) {
    streams_statuses_.error = stats_;
  } else if (std::is_same<PKT, data::StatusStreamPacket>::value) {
    streams_statuses_.status = stats_;
  } else if (std::is_same<PKT, data::SysInfoPacket>::value) {
    streams_statuses_.system_info = stats_;
  }
  shared_data_.blocks.stream_stats.set(streams_statuses_);
}

void modules::DataModule::parseStreams() {



  parseStream<data::DataStreamPacket>(shared_data_.streams.data,
                                      [](data::DataStreamPacket &pkt) {});
  parseStream<data::ErrorStreamPacket>(
      shared_data_.streams.error, [](data::ErrorStreamPacket &pkt) {
        std::string source = node::identification_to_string.at(pkt.source);
        std::cout << source << ":" << pkt.code << " - " << pkt.info
                  << std::endl;
      });
  parseStream<data::SysInfoPacket>(shared_data_.streams.system_info,
                                   [](data::SysInfoPacket &pkt) { (void)pkt; });
  parseStream<data::StatusStreamPacket>(
      shared_data_.streams.status,
      [](data::StatusStreamPacket &pkt) { (void)pkt; });
}
