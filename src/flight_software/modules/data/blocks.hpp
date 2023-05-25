#ifndef BLOCKS_HPP_
#define BLOCKS_HPP_

#include <mutex>

#include "time_types.h"
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

struct ConfigurationStats {
  int load_errors = 0;
  int save_errors = 0;
};

struct ModulesStatuses {
  node::Status data = node::Status::UNKNOWN;
  //modules::Status console = modules::Status::UNKNOWN;
  //modules::Status server = modules::Status::UNKNOWN;
  //modules::Status system = modules::Status::UNKNOWN;
};

struct ServerModuleStats {
  bool enabled = false;
  int total_packets_received = 0;
  int total_packets_sent = 0;
  int packet_receive_errors = 0;
  int packet_send_errors = 0;
  int packet_parse_errors = 0;
  std::string connection_status = "none";
};

struct StreamsStats {
  struct StreamStats {
    int current = 0;
    int total = 0;
    int processing_delay_ms = 0;
  };

  StreamStats data = StreamStats();
  StreamStats error = StreamStats();
  StreamStats status = StreamStats();
  StreamStats system_info = StreamStats();
};

}  // namespace blocks

}  // namespace data

#endif