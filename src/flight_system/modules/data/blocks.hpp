#ifndef BLOCKS_HPP_
#define BLOCKS_HPP_

#include <mutex>

#include "node.h"
#include <BoosterSeat/clock.hpp>
#include <BoosterSeat/numbers.hpp>
#include <BoosterSeat/time.hpp>

#include "json.hpp"

namespace data {
inline auto rnd = BoosterSeat::doubleToPrecisionTwo;

/**
 * @brief Simple Struct Container with a mutex.
 * The struct must have a valid default constructor.
 */
template <class T> class Block {
public:
  void set(T data) {
    std::lock_guard<std::mutex> lock(mutex_);
    data_ = data;
    last_updated_ = BoosterSeat::clck::now();
  }

  T get() {
    std::lock_guard<std::mutex> lock(mutex_);
    return data_;
  }

  int age_ms() {
    std::lock_guard<std::mutex> lock(mutex_);
    return BoosterSeat::clck::millisecondsElapsed(last_updated_);
  }

  int age_s() {
    std::lock_guard<std::mutex> lock(mutex_);
    return BoosterSeat::clck::secondsElapsed(last_updated_);
  }

private:
  std::mutex mutex_ = std::mutex();
  T data_ = T(); // Must have default constructor
  BoosterSeat::clck::TimePoint last_updated_ = BoosterSeat::clck::now();
};

namespace blocks {
/**
 * @todo implement this
 */
struct ConfigurationStats { // Set by the data module
  int load_errors = 0;
  int save_errors = 0;

  json toJson() {
    json j;
    j["load_errors"] = load_errors;
    j["save_errors"] = save_errors;
    return j;
  }
};

/**
 * @brief Struct containing the status of each module.
 */
struct ModulesStatuses { // Set by the data module
  node::Status data = node::Status::UNKNOWN;
  node::Status console = node::Status::UNKNOWN;
  node::Status server = node::Status::UNKNOWN;
  node::Status system = node::Status::UNKNOWN;

  json toJson() {
    json j;
    j["data"] = node::status_to_string.at(data);
    j["console"] = node::status_to_string.at(console);
    j["server"] = node::status_to_string.at(server);
    j["system"] = node::status_to_string.at(system);
    return j;
  }
};

/**
 * @todo implement this
 */
struct ServerModuleStats { // Set by the server module
  int num_messages_received = 0;
  int num_invalid_received = 0;
  double bytes_per_second_down = 0.0;
  double bytes_per_second_up = 0.0;
  bool is_connected = false;

  json toJson() {
    json j;
    j["num_messages_received"] = num_messages_received;
    j["num_invalid_received"] = num_invalid_received;
    j["bytes_per_second_down"] = rnd(bytes_per_second_down);
    j["bytes_per_second_up"] = rnd(bytes_per_second_up);
    j["connection_status"] = is_connected;
    return j;
  }
};

/**
 * @brief Struct containing statistics about the data and log streams.
 */
struct StreamsStats { // Set by the data module
  struct StreamStats {
    int current_packets = 0;
    int total_packets = 0;
    int processing_delay_ms = 0;

    json toJson() {
      json j;
      j["current_packets"] = current_packets;
      j["total_packets"] = total_packets;
      j["processing_delay_ms"] = processing_delay_ms;
      return j;
    }
  };

  StreamStats data = StreamStats();
  StreamStats log = StreamStats();

  json toJson() {
    json j;
    j["data"] = data.toJson();
    j["log"] = log.toJson();
    return j;
  }
};

/**
 * @brief Struct containing statistics about the data file log.
 */
struct DataLogStats {
  typedef double FileSizeType;

  // Data Files
  bool data_dir = false;
  bool data_archive_dir = false;
  bool data_file = false;

  FileSizeType data_file_size = 0.0;
  FileSizeType data_archive_dir_size = 0.0;
  std::string data_file_path = "";

  // Log Files
  bool log_dir = false;
  bool log_archive_dir = false;
  bool log_file = false;

  FileSizeType log_file_size = 0.0;
  FileSizeType log_archive_dir_size = 0.0;
  std::string log_file_path = "";

  json toJson() {
    json j;
    j["data_dir"] = data_dir;
    j["data_archive_dir"] = data_archive_dir;
    j["data_file"] = data_file;
    j["data_file_size"] = rnd(data_file_size);
    j["data_dir_size"] = rnd(data_archive_dir_size);
    j["data_file_path"] = data_file_path;
    j["log_dir"] = log_dir;
    j["log_archive_dir"] = log_archive_dir;
    j["log_file"] = log_file;
    j["log_file_size"] = rnd(log_file_size);
    j["log_archive_dir_size"] = rnd(log_archive_dir_size);
    j["log_file_path"] = log_file_path;
    return j;
  }
};

/**
 * @brief Struct containing system information related to the system module.
 */
struct SystemInfo { // Set by the system module
  // misc
  float uptime_hours = 0.0;

  // cpu
  float cpu_load_avg_1 = 0.0;  // 1 minute
  float cpu_load_avg_5 = 0.0;  // 5 minutes
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

  json toJson() {
    json j;
    j["uptime_hours"] = rnd(uptime_hours);
    j["cpu_load_avg_1"] = rnd(cpu_load_avg_1);
    j["cpu_load_avg_5"] = rnd(cpu_load_avg_5);
    j["cpu_load_avg_15"] = rnd(cpu_load_avg_15);
    j["cpu_temp_c"] = rnd(cpu_temp_c);
    j["mem_total_gb"] = rnd(mem_total_gb);
    j["mem_free_gb"] = rnd(mem_free_gb);
    j["mem_used_percent"] = rnd(mem_used_percent);
    j["swap_total_gb"] = rnd(swap_total_gb);
    j["swap_free_gb"] = rnd(swap_free_gb);
    j["swap_used_percent"] = rnd(swap_used_percent);
    j["disk_total_gb"] = rnd(disk_total_gb);
    j["disk_free_gb"] = rnd(disk_free_gb);
    j["disk_used_percent"] = rnd(disk_used_percent);
    return j;
  }
};

} // namespace blocks

} // namespace data

#endif