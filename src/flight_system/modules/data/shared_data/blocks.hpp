#ifndef BLOCKS_HPP_
#define BLOCKS_HPP_

#include <mutex>

#include <BoosterSeat/clock.hpp>
#include <BoosterSeat/numbers.hpp>
#include <BoosterSeat/time.hpp>

#include "blocks.hpp"
#include "json.hpp"
#include "node.hpp"
#include "positional.hpp"

namespace data {
inline auto rnd = BoosterSeat::doubleToPrecisionTwo;

namespace blocks {
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
  node::Status extension = node::Status::UNKNOWN;

  json toJson() {
    json j;
    j["data"] = node::status_to_string.at(data);
    j["console"] = node::status_to_string.at(console);
    j["server"] = node::status_to_string.at(server);
    j["system"] = node::status_to_string.at(system);
    j["extension"] = node::status_to_string.at(extension);
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
  };

  StreamStats data = StreamStats();
  StreamStats log = StreamStats();
  StreamStats gps = StreamStats();
  StreamStats imu = StreamStats();

  json toJson() {
    json j;
    j["data_current_packets"] = data.current_packets;
    j["data_total_packets"] = data.total_packets;
    j["data_processing_delay_ms"] = data.processing_delay_ms;
    j["log_current_packets"] = log.current_packets;
    j["log_total_packets"] = log.total_packets;
    j["log_processing_delay_ms"] = log.processing_delay_ms;
    j["gps_current_packets"] = gps.current_packets;
    j["gps_total_packets"] = gps.total_packets;
    j["gps_processing_delay_ms"] = gps.processing_delay_ms;
    j["imu_current_packets"] = imu.current_packets;
    j["imu_total_packets"] = imu.total_packets;
    j["imu_processing_delay_ms"] = imu.processing_delay_ms;
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

/**
 * @brief Struct containing statistics about the extension module and its
 * extensions.
 */
struct ExtensionModuleStats {
  /**
   * @brief The number of extensions that are in the configuration.
   */
  int num_configured = 0;
  int num_active = 0;
  int num_inactive = 0;

  json toJson() {
    json j;
    j["num_configured"] = num_configured;
    j["num_active"] = num_active;
    j["num_inactive"] = num_inactive;
    return j;
  }
};

/**
 * @brief Struct containing location data (gps).
 */
struct LocationData {
  /**
   * @brief The current gps fix.
   */
  data::GpsFix current_gps_fix = data::GpsFix::NO_FIX;

  /**
   * @brief The last valid gps fix. If it is NO_FIX, there never was a valid
   * fix.
   */
  data::GpsFix last_valid_gps_fix = data::GpsFix::NO_FIX;

  /**
   * @brief The last valid gps fix.
   */
  data::GpsFrame last_valid_gps_frame{};

  /**
   * @brief The most recent gps frame.
   */
  data::GpsFrame last_gps_frame{};
};

} // namespace blocks

} // namespace data

#endif