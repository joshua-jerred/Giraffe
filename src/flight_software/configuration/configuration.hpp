// * * * AUTOMATICALLY GENERATED WITH CMake/Python * * *

#ifndef CONFIGURATION_HPP_
#define CONFIGURATION_HPP_

#include <string>
#include <fstream>
#include <mutex>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include "shared_data.hpp"

using json = nlohmann::ordered_json;

namespace cfg {
namespace gEnum {

enum class MainBoard {
  OTHER,
  PI_ZERO_W2,
  PI_4
};
static std::unordered_map<std::string, cfg::gEnum::MainBoard> const KeyToMainBoard = {
  {"other", cfg::gEnum::MainBoard::OTHER},
  {"pi_zero_w2", cfg::gEnum::MainBoard::PI_ZERO_W2},
  {"pi_4", cfg::gEnum::MainBoard::PI_4}
};
constexpr const char* MainBoardToKey(cfg::gEnum::MainBoard val) throw() {
  switch (val) {
    case cfg::gEnum::MainBoard::OTHER: return "other";
    case cfg::gEnum::MainBoard::PI_ZERO_W2: return "pi_zero_w2";
    case cfg::gEnum::MainBoard::PI_4: return "pi_4";
  }
  __builtin_unreachable();
}

enum class ProcedureType {
  TESTING,
  PRE_LAUNCH,
  ASCENT,
  DESCENT,
  RECOVERY,
  FAILSAFE
};
static std::unordered_map<std::string, cfg::gEnum::ProcedureType> const KeyToProcedureType = {
  {"testing", cfg::gEnum::ProcedureType::TESTING},
  {"pre_launch", cfg::gEnum::ProcedureType::PRE_LAUNCH},
  {"ascent", cfg::gEnum::ProcedureType::ASCENT},
  {"descent", cfg::gEnum::ProcedureType::DESCENT},
  {"recovery", cfg::gEnum::ProcedureType::RECOVERY},
  {"failsafe", cfg::gEnum::ProcedureType::FAILSAFE}
};
constexpr const char* ProcedureTypeToKey(cfg::gEnum::ProcedureType val) throw() {
  switch (val) {
    case cfg::gEnum::ProcedureType::TESTING: return "testing";
    case cfg::gEnum::ProcedureType::PRE_LAUNCH: return "pre_launch";
    case cfg::gEnum::ProcedureType::ASCENT: return "ascent";
    case cfg::gEnum::ProcedureType::DESCENT: return "descent";
    case cfg::gEnum::ProcedureType::RECOVERY: return "recovery";
    case cfg::gEnum::ProcedureType::FAILSAFE: return "failsafe";
  }
  __builtin_unreachable();
}

} // namespace gEnum

class CfgSection {
 public:
  CfgSection(data::Streams &streams): streams_(streams){}
  
 protected:
  mutable std::mutex cfg_lock_ = std::mutex();
  data::Streams &streams_;
};

class General : public cfg::CfgSection {
public:
  General(data::Streams &streams): cfg::CfgSection(streams){}

  std::string getProjectName() const;
  cfg::gEnum::MainBoard getMainBoard() const;
  cfg::gEnum::ProcedureType getStartingProcedure() const;

  void setProjectName(std::string);
  void setMainBoard(cfg::gEnum::MainBoard);
  void setStartingProcedure(cfg::gEnum::ProcedureType);

  void setFromJson(const json&);
  json getJson() const;

private:
  std::string project_name_ = "Giraffe Flight 1";
  cfg::gEnum::MainBoard main_board_ = cfg::gEnum::MainBoard::OTHER;
  cfg::gEnum::ProcedureType starting_procedure_ = cfg::gEnum::ProcedureType::FAILSAFE;
};

class Configuration {
 public:
  Configuration(data::Streams &streams):
    general(streams),
    streams_(streams){}
    
    void save(std::string file_path = "");
    void load(std::string file_path = "");

  cfg::General general;
 
 private:
  void error(data::logId error_code, std::string info = "") {
      streams_.log.error(node::Identification::CONFIGURATION, error_code, info);
  }
 
  data::Streams &streams_;
  std::mutex file_lock_ = std::mutex();
};

} // namespace cfg

#endif // CONFIGURATION_HPP_

// * * * AUTOMATICALLY GENERATED WITH CMake/Python * * *
