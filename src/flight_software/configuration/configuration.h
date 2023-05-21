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
  Procedure::Type starting_procedure =
      cfg::Procedure::Type::FAILSAFE;
};

struct Debug {
  bool print_errors = false;
  bool console_enabled = false;
  int console_update_interval = 1000; // in ms
};

struct Server {
  int tcp_socket_port = 7893;
};

/**
 * @todo to be expanded, as needed.
 */
struct Frequency {
  Frequency() : frequency("000.0000") {}
  Frequency(std::string freq) : frequency(freq) {}

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

  bool setGeneral(const cfg::General &general);
  cfg::General getGeneral();

  bool setDebug(const cfg::Debug &interface);
  cfg::Debug getDebug();

  bool setServer(const cfg::Server &server);
  cfg::Server getServer();

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

  const std::string kConfigurationPath = "config.json";

  cfg::General general_ = cfg::General();
  cfg::Debug debug_ = cfg::Debug();
  cfg::Server server_ = cfg::Server();
  cfg::Telemetry telemetry_ = cfg::Telemetry();
  cfg::Aprs aprs_ = cfg::Aprs();
  cfg::Sstv sstv_ = cfg::Sstv();
  cfg::DataPackets data_packets_ = cfg::DataPackets();

  data::Streams &streams_;
  std::mutex config_lock_ = std::mutex();
};

};  // namespace cfg

#endif