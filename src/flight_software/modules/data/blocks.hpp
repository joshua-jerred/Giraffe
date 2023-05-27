#ifndef BLOCKS_HPP_
#define BLOCKS_HPP_

#include <mutex>

#include "time_types.hpp"
#include "node.h"

namespace data {

/**
 * @brief Simple Struct Container with a mutex.
 * The struct must have a valid default constructor.
 */
template <class T>
class Block {
 public:
  void set(T data) {
    std::lock_guard<std::mutex> lock(mutex_);
    data_ = data;
    last_updated_ = giraffe_time::now();
  }

  T get() {
    std::lock_guard<std::mutex> lock(mutex_);
    return data_;
  }

  int age_ms() {
    std::lock_guard<std::mutex> lock(mutex_);
    return giraffe_time::millisecondsElapsed(last_updated_);
  }

  int age_s() {
    std::lock_guard<std::mutex> lock(mutex_);
    return giraffe_time::secondsElapsed(last_updated_);
  }

 private:
  std::mutex mutex_ = std::mutex();
  T data_ = T();  // Must have default constructor
  giraffe_time::TimePoint last_updated_ = giraffe_time::now();
};

namespace blocks {

struct ConfigurationStats { // Set by the data module
  int load_errors = 0;
  int save_errors = 0;
};

struct ModulesStatuses { // Set by the data module
  node::Status data = node::Status::UNKNOWN;
  node::Status console = node::Status::UNKNOWN;
  node::Status server = node::Status::UNKNOWN;
  node::Status system = node::Status::UNKNOWN;
};

struct ServerModuleStats { // Set by the server module
  bool enabled = false;
  int total_packets_received = 0;
  int total_packets_sent = 0;
  int packet_receive_errors = 0;
  int packet_send_errors = 0;
  int packet_parse_errors = 0;
  std::string connection_status = "none";
};

struct StreamsStats {  // Set by the data module
  struct StreamStats {
    int current_packets = 0;
    int total_packets = 0;
    int processing_delay_ms = 0;
  };

  StreamStats data = StreamStats();
  StreamStats log = StreamStats();
};

struct SystemInfo { // Set by the system module
  // misc
  float uptime_hours = 0.0;

  // cpu
  float cpu_load_avg_1 = 0.0; // 1 minute
  float cpu_load_avg_5 = 0.0; // 5 minutes
  float cpu_load_avg_15 = 0.0; // 15 minutes
  float cpu_temp_c = 0.0;

  // mem
  float mem_total_gb = 0.0;
  float mem_free_gb = 0.0;
  float mem_used_percent = 0.0;

  // swap
  float swap_total_gb = 0.0;
  float swap_free_gb = 0.0;
  float swap_used_percent = 0.0;

  // disk
  float disk_total_gb = 0.0;
  float disk_free_gb = 0.0;
  float disk_used_percent = 0.0;

  // network (tbd)
};

}  // namespace blocks

}  // namespace data

#endif