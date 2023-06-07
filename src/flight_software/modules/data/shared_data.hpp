#ifndef SHARED_DATA_HPP_
#define SHARED_DATA_HPP_

#include "blocks.hpp"
#include "frame.hpp"
#include "streams.hpp"
#include "time_types.hpp"

#include <BoosterSeat/clock.hpp>
#include <BoosterSeat/time.hpp>

namespace data {

struct Streams {
  DataStream data = DataStream();
  LogStream log = LogStream();
};

struct Frames {
  Frame<std::string, DataPacket> data = Frame<std::string, DataPacket>();

  Frame<std::string, LogPacket> log = Frame<std::string, LogPacket>();
};

struct Blocks {
  Block<blocks::StreamsStats> stream_stats = Block<blocks::StreamsStats>();
  Block<blocks::ModulesStatuses> modules_statuses =
      Block<blocks::ModulesStatuses>();
  Block<blocks::DataLogStats> data_log_stats = Block<blocks::DataLogStats>();
  Block<blocks::SystemInfo> system_info = Block<blocks::SystemInfo>();
};

struct Misc {
  double getUptimeHours() const {
    return BoosterSeat::clck::secondsElapsed(start_time) / 3600.0;
  }

  std::string getUptimeString() const {
    return BoosterSeat::time::elapsedAsciiClock(start_time);
  }

  const BoosterSeat::clck::TimePoint start_time = giraffe_time::now();
};

struct SharedData {
  Streams streams = Streams();
  Frames frames = Frames();
  Blocks blocks = Blocks();
  Misc misc = Misc();
};

} // namespace data

#endif