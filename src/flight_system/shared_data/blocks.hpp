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

  int getAgeMs() {
    std::lock_guard<std::mutex> lock(mutex_);
    return BoosterSeat::clck::millisecondsElapsed(last_updated_);
  }

  int getAgeSec() {
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

  Json toJson() const {
    Json json_data;
    json_data["load_errors"] = load_errors;
    json_data["save_errors"] = save_errors;
    return json_data;
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
  node::Status telemetry = node::Status::UNKNOWN;

  Json toJson() const {
    Json json_data;
    json_data["data"] = node::K_STATUS_TO_STRING_MAP.at(data);
    json_data["console"] = node::K_STATUS_TO_STRING_MAP.at(console);
    json_data["server"] = node::K_STATUS_TO_STRING_MAP.at(server);
    json_data["system"] = node::K_STATUS_TO_STRING_MAP.at(system);
    json_data["extension"] = node::K_STATUS_TO_STRING_MAP.at(extension);
    json_data["telemetry"] = node::K_STATUS_TO_STRING_MAP.at(telemetry);
    return json_data;
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

  Json toJson() {
    Json json_data;
    json_data["num_messages_received"] = num_messages_received;
    json_data["num_invalid_received"] = num_invalid_received;
    json_data["bytes_per_second_down"] = rnd(bytes_per_second_down);
    json_data["bytes_per_second_up"] = rnd(bytes_per_second_up);
    json_data["connection_status"] = is_connected;
    return json_data;
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

  Json toJson() {
    Json json_data;
    json_data["data_current_packets"] = data.current_packets;
    json_data["data_total_packets"] = data.total_packets;
    json_data["data_processing_delay_ms"] = data.processing_delay_ms;
    json_data["log_current_packets"] = log.current_packets;
    json_data["log_total_packets"] = log.total_packets;
    json_data["log_processing_delay_ms"] = log.processing_delay_ms;
    json_data["gps_current_packets"] = gps.current_packets;
    json_data["gps_total_packets"] = gps.total_packets;
    json_data["gps_processing_delay_ms"] = gps.processing_delay_ms;
    json_data["imu_current_packets"] = imu.current_packets;
    json_data["imu_total_packets"] = imu.total_packets;
    json_data["imu_processing_delay_ms"] = imu.processing_delay_ms;
    return json_data;
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
  std::string data_file_path{};

  // Log Files
  bool log_dir = false;
  bool log_archive_dir = false;
  bool log_file = false;

  FileSizeType log_file_size = 0.0;
  FileSizeType log_archive_dir_size = 0.0;
  std::string log_file_path{};

  std::vector<std::string> archived_data_files_list{};
  std::vector<std::string> archived_log_files_list{};

  Json toJson() {
    Json json_data;
    json_data["data_dir"] = data_dir;
    json_data["data_archive_dir"] = data_archive_dir;
    json_data["data_file"] = data_file;
    json_data["data_file_size"] = rnd(data_file_size);
    json_data["data_dir_size"] = rnd(data_archive_dir_size);
    json_data["data_file_path"] = data_file_path;
    json_data["log_dir"] = log_dir;
    json_data["log_archive_dir"] = log_archive_dir;
    json_data["log_file"] = log_file;
    json_data["log_file_size"] = rnd(log_file_size);
    json_data["log_archive_dir_size"] = rnd(log_archive_dir_size);
    json_data["log_file_path"] = log_file_path;
    return json_data;
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

  Json toJson() {
    Json json_data;
    json_data["uptime_hours"] = rnd(uptime_hours);
    json_data["cpu_load_avg_1"] = rnd(cpu_load_avg_1);
    json_data["cpu_load_avg_5"] = rnd(cpu_load_avg_5);
    json_data["cpu_load_avg_15"] = rnd(cpu_load_avg_15);
    json_data["cpu_temp_c"] = rnd(cpu_temp_c);
    json_data["mem_total_gb"] = rnd(mem_total_gb);
    json_data["mem_free_gb"] = rnd(mem_free_gb);
    json_data["mem_used_percent"] = rnd(mem_used_percent);
    json_data["swap_total_gb"] = rnd(swap_total_gb);
    json_data["swap_free_gb"] = rnd(swap_free_gb);
    json_data["swap_used_percent"] = rnd(swap_used_percent);
    json_data["disk_total_gb"] = rnd(disk_total_gb);
    json_data["disk_free_gb"] = rnd(disk_free_gb);
    json_data["disk_used_percent"] = rnd(disk_used_percent);
    return json_data;
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

  Json toJson() {
    Json json_data;
    json_data["num_configured"] = num_configured;
    json_data["num_active"] = num_active;
    json_data["num_inactive"] = num_inactive;
    return json_data;
  }
};

/**
 * @brief Struct containing location data (gps).
 */
struct LocationData {
  /**
   * @brief Whether or not we have a gps source.
   * @details Set to true if we have had a single gps frame come through the
   * stream.
   */
  bool have_gps_source = false;

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

struct ImuData {
  /**
   * @brief Whether or not we have an imu source.
   * @details Set to true if we have had a single imu frame comes through the
   * stream.
   */
  bool have_imu_source = false;

  /**
   * @brief The most recent imu frame.
   */
  data::ImuFrame most_recent_imu_frame{};

  /**
   * @brief Last valid imu frame.
   */
  data::ImuFrame last_valid_imu_frame{};
};

/**
 * @brief Struct containing image data from a camera.
 */
struct CameraImages {
  bool have_camera_source = false;
  std::string last_valid_image_path = "";
  int num_images = 0;
};

struct TelemetryModuleStats {
  int exchange_queue_size = 0;
  int broadcast_queue_size = 0;
  int received_queue_size = 0;
  int network_layer_latency_ms = 0;

  Json toJson() {
    Json json_data;
    json_data["exchange_queue_size"] = exchange_queue_size;
    json_data["broadcast_queue_size"] = broadcast_queue_size;
    json_data["network_layer_latency_ms"] = network_layer_latency_ms;
    json_data["received_queue_size"] = received_queue_size;
    return json_data;
  }
};

} // namespace blocks

} // namespace data

#endif