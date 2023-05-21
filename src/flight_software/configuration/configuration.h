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
#include <unordered_map>
#include <vector>

#include "streams.h"

namespace cfg {
const std::string kDefaultConfigPath = "gfs_config.json";

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
  void saveConfig(std::string file_path = "");
  void loadConfig(std::string file_path = "");

  void reportError(std::string code, std::string info = "");


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

static const std::unordered_map<cfg::Procedure::Type, std::string>
    kProcedureTypeToString = {{cfg::Procedure::Type::TESTING, "testing"},
                              {cfg::Procedure::Type::ASCENT, "ascent"},
                              {cfg::Procedure::Type::DESCENT, "descent"},
                              {cfg::Procedure::Type::RECOVERY, "recovery"},
                              {cfg::Procedure::Type::FAILSAFE, "failsafe"}};

static const std::unordered_map<std::string, cfg::Procedure::Type>
    kStringToProcedureType = {{"testing", cfg::Procedure::Type::TESTING},
                              {"ascent", cfg::Procedure::Type::ASCENT},
                              {"descent", cfg::Procedure::Type::DESCENT},
                              {"recovery", cfg::Procedure::Type::RECOVERY},
                              {"failsafe", cfg::Procedure::Type::FAILSAFE}};

static const std::unordered_map<cfg::General::MainBoard, std::string>
    kMainBoardToString = {
        {cfg::General::MainBoard::OTHER, "other"},
        {cfg::General::MainBoard::PI_4, "pi_4"},
        {cfg::General::MainBoard::PI_ZERO_W_2, "pi_zero_w_2"}};

static const std::unordered_map<std::string, cfg::General::MainBoard>
    kStringToMainBoard = {
        {"other", cfg::General::MainBoard::OTHER},
        {"pi_4", cfg::General::MainBoard::PI_4},
        {"pi_zero_w_2", cfg::General::MainBoard::PI_ZERO_W_2}};

static const std::unordered_map<cfg::Aprs::SymbolTable, std::string>
    kAprsSymbolTableToString = {
        {cfg::Aprs::SymbolTable::PRIMARY, "primary"},
        {cfg::Aprs::SymbolTable::ALTERNATE, "alternate"}};

static const std::unordered_map<std::string, cfg::Aprs::SymbolTable>
    kStringToAprsSymbolTable = {
        {"primary", cfg::Aprs::SymbolTable::PRIMARY},
        {"alternate", cfg::Aprs::SymbolTable::ALTERNATE}};

static const std::unordered_map<cfg::Sstv::Mode, std::string>
    kSstvModeToString = {{cfg::Sstv::Mode::ROBOT_36, "robot36"}};

static const std::unordered_map<std::string, cfg::Sstv::Mode>
    kStringToSstvMode = {{"robot36", cfg::Sstv::Mode::ROBOT_36}};

static const std::unordered_map<cfg::DataPackets::Mode, std::string>
    kDataPacketsModeToString = {
        {cfg::DataPackets::Mode::BPSK125, "bpsk125"},
        {cfg::DataPackets::Mode::BPSK250, "bpsk250"},
        {cfg::DataPackets::Mode::BPSK500, "bpsk500"},
        {cfg::DataPackets::Mode::BPSK1000, "bpsk1000"},
        {cfg::DataPackets::Mode::QPSK125, "qpsk125"},
        {cfg::DataPackets::Mode::QPSK250, "qpsk250"},
        {cfg::DataPackets::Mode::QPSK500, "qpsk500"},
        {cfg::DataPackets::Mode::AFSK_AX25, "afsk_ax25"}};

static const std::unordered_map<std::string, cfg::DataPackets::Mode>
    kStringToDataPacketsMode = {
        {"bpsk125", cfg::DataPackets::Mode::BPSK125},
        {"bpsk250", cfg::DataPackets::Mode::BPSK250},
        {"bpsk500", cfg::DataPackets::Mode::BPSK500},
        {"bpsk1000", cfg::DataPackets::Mode::BPSK1000},
        {"qpsk125", cfg::DataPackets::Mode::QPSK125},
        {"qpsk250", cfg::DataPackets::Mode::QPSK250},
        {"qpsk500", cfg::DataPackets::Mode::QPSK500},
        {"afsk_ax25", cfg::DataPackets::Mode::AFSK_AX25}};

inline void reportError(data::ErrorStream &es, const std::string code,
                        const std::string info) {
  es.addError(node::Identification::CONFIGURATION, code, info);
}

};  // namespace cfg

#endif