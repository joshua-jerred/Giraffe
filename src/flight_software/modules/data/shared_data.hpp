#ifndef SHARED_DATA_HPP_
#define SHARED_DATA_HPP_

#include "blocks.hpp"
#include "frame.hpp"
#include "streams.hpp"

namespace data {

struct Streams {
  data::DataStream data = data::DataStream();
  data::LogStream log = data::LogStream();
};

struct Frames {
  data::Frame<std::string, data::DataPacket> data =
      data::Frame<std::string, data::DataPacket>();

  data::Frame<std::string, data::LogPacket> log =
      data::Frame<std::string, data::LogPacket>();
};

struct Blocks {
  data::Block<data::blocks::StreamsStats> stream_stats =
      data::Block<data::blocks::StreamsStats>();
  data::Block<data::blocks::ModulesStatuses> modules_statuses =
      data::Block<data::blocks::ModulesStatuses>();
};

struct SharedData {
  data::Streams streams = data::Streams();
  data::Frames frames = data::Frames();
  data::Blocks blocks = data::Blocks();
};

}  // namespace data

#endif