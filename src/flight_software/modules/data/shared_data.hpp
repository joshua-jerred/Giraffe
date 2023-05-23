#ifndef SHARED_DATA_HPP_
#define SHARED_DATA_HPP_

#include "blocks.hpp"
#include "frame.hpp"
#include "streams.hpp"

namespace data {

struct Streams {
  data::DataStream data = data::DataStream();
  data::ErrorStream error = data::ErrorStream();
  data::StatusStream status = data::StatusStream();
  data::SystemInfoStream system_info = data::SystemInfoStream();
};

struct Frames {
  data::Frame<std::string, data::DataStreamPacket> data =
      data::Frame<std::string, data::DataStreamPacket>();

  data::Frame<std::string, data::ErrorStreamPacket> error =
      data::Frame<std::string, data::ErrorStreamPacket>();
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