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
#include <string>
#include <unordered_map>
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

  std::string project_name;
  MainBoard main_board_type;
  Procedure::Type starting_procedure;
};

struct Debug {
  bool print_errors;
  bool console_enabled;
  int console_update_interval;
};

struct Server {
  int tcp_socket_port;
};

/**
 * @todo to be expanded, as needed.
 */
struct Frequency {
  std::string frequency;

  void setFrequency(std::string freq) {
    frequency = freq;
  }
  std::string getFrequency() const {
    return frequency;
  }
};

struct Telemetry {
  bool telemetry_enabled;
  std::string call_sign;
};

struct Aprs {
  enum class SymbolTable { PRIMARY, ALTERNATE };

  bool telemetry_packets;
  bool position_packets;
  cfg::Frequency frequency;
  int ssid;
  std::string destination_address;
  int destination_ssid;
  cfg::Aprs::SymbolTable symbol_table;
  char symbol;
  std::string comment;
};

struct Sstv {
  enum class Mode { ROBOT_36 };

  bool enabled;
  cfg::Frequency frequency;
  cfg::Sstv::Mode mode;
  bool overlay_data;
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

  bool enabled;
  cfg::Frequency frequency;
  cfg::DataPackets::Mode mode;
  bool morse_call_sign;
  std::string comment;
};

class Configuration {
 public:
  Configuration(data::ErrorStream &es);
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
  void reportError(std::string code, std::string info = "");

  data::ErrorStream &es_;

  cfg::General general_;
  cfg::Debug debug_;
  cfg::Server server_;
  cfg::Telemetry telemetry_;
  cfg::Aprs aprs_;
  cfg::Sstv sstv_;
  cfg::DataPackets data_packets_;

  std::mutex config_lock_;
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
  es.addError(data::Source::CONFIGURATION_MODULE, code, info);
}

};  // namespace cfg

#endif