/**
 * @file config_structure.cpp
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief
 * @date 2023-05-09
 * @copyright Copyright (c) 2023
 */

#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_

#include <mutex>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>
#include <atomic>

#include "node.h"
#include "streams.h"

namespace cfg {
struct Procedure {
  enum class Type { TESTING, ASCENT, DESCENT, RECOVERY, FAILSAFE };

  // struct Services {
  //   bool data_logging;

  //   bool telem_data_log;
  //   bool telem_data_packet;
  //   bool telem_sstv;
  //   bool telem_aprs_location;
  //   bool telem_aprs_telemetry;

  //   bool image_capture;
  //   bool health_check;
  // };

  // bool enabled;
  // Type type;
  // std::string name;
};

struct General {
  enum class MainBoard {
    OTHER,
    PI_ZERO_W_2,
    PI_4,
  };

  std::string project_name = "Giraffe Flight 1";
  MainBoard main_board_type = cfg::General::MainBoard::OTHER;
  Procedure::Type starting_procedure = cfg::Procedure::Type::FAILSAFE;
};

struct DataModuleGeneral {
  int frame_purge_time = 30000;
};

enum class ArchivalMethod { PLAIN_TEXT };

struct DataModuleDataLog {
  enum class LogStrategy { INTERVAL, ALL, SELECTION_INTERVAL, SELECTION_ALL };
  enum class LogDetail { FULL, PARTIAL };

  bool log_data_to_file = true;
  LogStrategy log_strategy = LogStrategy::INTERVAL;
  LogDetail log_detail = LogDetail::FULL;
  int log_interval_ms = 5000;
  int max_data_log_file_size_mb = 10;
  int max_data_archive_size_mb = 100;
  ArchivalMethod archival_method = ArchivalMethod::PLAIN_TEXT;
};

struct DataItem {
  // node::Identifier source;
  std::string identifier = "none";
};
typedef std::vector<DataItem> DataSelection;

struct DataModuleInfluxDb {
  enum class RetentionPolicy { HOUR, DAY, WEEK, MONTH, YEAR, INF };

  bool influx_enabled = false;
  bool log_errors = false;
  std::string url = "localhost";
  std::string token = "none";
  std::string organization = "giraffe";
  std::string data_bucket = "gfs_data";
  std::string error_bucket = "gfs_errors";
  RetentionPolicy retention_policy = RetentionPolicy::INF;
};

struct DataModuleErrorLog {
  bool log_errors_to_file = true;
  int max_error_log_file_size_mb = 10;
  int max_error_archive_size_mb = 50;
  ArchivalMethod archival_method = ArchivalMethod::PLAIN_TEXT;
};

struct DataModuleDebug {
  enum class LogLevel { INFO, WARN, ERROR };

  bool debug_enabled = false;
  LogLevel log_level = LogLevel::INFO;
};

struct ConsoleModule {
  bool enabled = false;
  int update_interval = 1000;  // in ms
};

struct ServerModule {
  bool enabled = true;
  int tcp_socket_port = 7893;
};

struct SystemModule {
  bool enabled = true;
  int system_info_poll_rate_ms = 1000;
};

/**
 * @todo to be expanded, as needed.
 */
struct Frequency {
  Frequency() : frequency("000.0000") {
  }
  Frequency(std::string freq) : frequency(freq) {
  }

  std::string frequency = "000.0000";

  /**
   * @todo need to do validation
   */
  void setFrequency(std::string freq) {
    frequency = freq;
  }
  std::string getFrequency() const {
    return frequency;
  }
};

struct Telemetry {
  bool telemetry_enabled = false;
  std::string call_sign = "N0CALL";
};

struct Aprs {
  enum class SymbolTable { PRIMARY, ALTERNATE };

  bool telemetry_packets = false;
  bool position_packets = false;
  cfg::Frequency frequency = cfg::Frequency("144.3900");
  int ssid = 0;
  std::string destination_address = "APRS";
  int destination_ssid = 0;
  cfg::Aprs::SymbolTable symbol_table = cfg::Aprs::SymbolTable::PRIMARY;
  char symbol = '/';
  std::string comment = "Giraffe Flight Software";
};

struct Sstv {
  enum class Mode { ROBOT_36 };

  bool enabled = false;
  cfg::Frequency frequency = cfg::Frequency("145.5100");
  cfg::Sstv::Mode mode = cfg::Sstv::Mode::ROBOT_36;
  bool overlay_data = true;
};

struct DataPackets {
  enum class Mode {
    BPSK125,
    BPSK250,
    BPSK500,
    BPSK1000,
    QPSK125,
    QPSK250,
    QPSK500,
    AFSK_AX25
  };

  bool enabled = false;
  cfg::Frequency frequency = cfg::Frequency("145.5100");
  cfg::DataPackets::Mode mode = cfg::DataPackets::Mode::BPSK250;
  bool morse_call_sign = true;
  std::string comment = "Giraffe Flight Software";
};

class Configuration {
 public:
  using json = nlohmann::ordered_json;

  Configuration(data::Streams &streams);
  ~Configuration();

  bool setGeneral(const cfg::General &);
  cfg::General getGeneral();

  bool setDataModuleGeneral(const cfg::DataModuleGeneral &);
  cfg::DataModuleGeneral getDataModuleGeneral();

  bool setDataModuleDataLog(const cfg::DataModuleDataLog &);
  cfg::DataModuleDataLog getDataModuleDataLog();

  bool setDataModuleInfluxDb(const cfg::DataModuleInfluxDb &);
  cfg::DataModuleInfluxDb getDataModuleInfluxDb();

  bool setDataModuleErrorLog(const cfg::DataModuleErrorLog &);
  cfg::DataModuleErrorLog getDataModuleErrorLog();

  bool setDataModuleDebug(const cfg::DataModuleDebug &);
  cfg::DataModuleDebug getDataModuleDebug();

  bool setConsoleModule(const cfg::ConsoleModule &);
  cfg::ConsoleModule getConsoleModule();

  bool setServerModule(const cfg::ServerModule &);
  cfg::ServerModule getServerModule();

  bool setSystemModule(const cfg::SystemModule &);
  cfg::SystemModule getSystemModule();

  bool setTelemetry(const cfg::Telemetry &telemetry);
  cfg::Telemetry getTelemetry();

  bool setAprs(const cfg::Aprs &aprs);
  cfg::Aprs getAprs();

  bool setSstv(const cfg::Sstv &sstv);
  cfg::Sstv getSstv();

  bool setDataPackets(const cfg::DataPackets &data_packets);
  cfg::DataPackets getDataPackets();

 private:
  void saveConfig();
  void loadConfig();

  void reportError(std::string code, std::string info = "");

  std::atomic<bool> load_mode_ = false;

  const std::string kConfigurationPath = "config.json";

  cfg::General general_ = cfg::General();
  cfg::DataModuleGeneral dm_general_ = cfg::DataModuleGeneral();
  cfg::DataModuleDataLog dm_data_log_ =
      cfg::DataModuleDataLog();
  cfg::DataModuleInfluxDb dm_influxdb_ =
      cfg::DataModuleInfluxDb();
  cfg::DataModuleErrorLog dm_error_log_ =
      cfg::DataModuleErrorLog();
  cfg::DataModuleDebug dm_debug_ = cfg::DataModuleDebug();
  cfg::ConsoleModule console_module_ = cfg::ConsoleModule();
  cfg::SystemModule system_module_ = cfg::SystemModule();
  cfg::ServerModule server_module_ = cfg::ServerModule();

  cfg::Telemetry telemetry_ = cfg::Telemetry();
  cfg::Aprs aprs_ = cfg::Aprs();
  cfg::Sstv sstv_ = cfg::Sstv();
  cfg::DataPackets data_packets_ = cfg::DataPackets();

  data::Streams &streams_;
  std::mutex config_lock_ = std::mutex();
};

};  // namespace cfg

#endif