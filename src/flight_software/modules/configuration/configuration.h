/**
 * @file config_structure.cpp
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief
 * @date 2023-05-09
 * @copyright Copyright (c) 2023
 */

#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_

#include <string>
#include <unordered_map>
#include <vector>
#include <mutex>

namespace cfg {

struct Procedure {
 enum class Type {
   TESTING,
   ASCENT,
   DESCENT,
   RECOVERY,
   FAILSAFE
};

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

class Configuration {
public:
  Configuration();
  ~Configuration();

  void setGeneral(cfg::General &general);
  cfg::General getGeneral();

  void setDebug(cfg::Debug &interface);
  cfg::Debug getDebug();

  void setServer(cfg::Server &server);
  cfg::Server getServer();

private:
  cfg::General general_;
  cfg::Debug debug_;
  cfg::Server server_;

  std::mutex config_lock_;
};
};  // namespace cfg

#endif