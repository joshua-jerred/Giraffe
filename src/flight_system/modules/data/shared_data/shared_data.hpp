/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   shared_data.hpp
 * @brief  Contains the SharedData struct, used across all modules and
 * extensions.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-06-07
 * @copyright  2023 (license to be defined)
 */

#ifndef SHARED_DATA_HPP_
#define SHARED_DATA_HPP_

#include "blocks.hpp"
#include "frames.hpp"
#include "streams.hpp"

#include <BoosterSeat/clock.hpp>
#include <BoosterSeat/time.hpp>

namespace data {
struct Streams {
  DataStream data = DataStream();
  LogStream log = LogStream();
  GpsFrameStream gps = GpsFrameStream();
  ImuFrameStream imu = ImuFrameStream();
};

struct SharedBlocks {
  blocks::Block<blocks::StreamsStats> stream_stats{};
  blocks::Block<blocks::ModulesStatuses> modules_statuses{};
  blocks::Block<blocks::ServerModuleStats> server_module_stats{};
  blocks::Block<blocks::DataLogStats> data_log_stats{};
  blocks::Block<blocks::SystemInfo> system_info{};
  blocks::Block<blocks::ExtensionModuleStats> extension_module_stats{};
};

struct Frames {
  Frame<std::string, DataPacket> data = Frame<std::string, DataPacket>();

  Frame<std::string, LogPacket> log = Frame<std::string, LogPacket>();
};

struct Misc {
  double getUptimeHours() const {
    return BoosterSeat::clck::secondsElapsed(start_time) / 3600.0;
  }

  std::string getUptimeString() const {
    return BoosterSeat::time::elapsedAsciiClock(start_time);
  }

  const BoosterSeat::clck::TimePoint start_time = BoosterSeat::clck::now();
};

struct SharedData {
  Streams streams = Streams();
  Frames frames = Frames();
  SharedBlocks blocks{};
  Misc misc = Misc();

#ifndef DNDEBUG // Defined by CMake
  /**
   * @brief Used to log debug messages when the program is not in release mode.
   *
   * @param id - Node identification
   * @param info - Debug message
   */
  void debugLog(const node::Identification id, const std::string &info) {
    streams.log.debug(id, info);
  }
#else
  /**
   * @brief This function does nothing when the program is in release mode.
   * @param id
   * @param info
   */
  void debugLog(const node::Identification id, const std::string &info) {
    (void)id;
    (void)info;
  }
#endif
};

} // namespace data

#endif