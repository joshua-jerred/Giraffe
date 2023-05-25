// * * * AUTOMATICALLY GENERATED WITH CMake/Python * * *

#ifndef CONFIGURATION_JSON_HPP_
#define CONFIGURATION_JSON_HPP_

#include <string>

namespace cfg {

namespace json {

class General {
public:
  std::string getProjectName() const;
  cfg::gEnum::MainBoard getMainBoard() const;
  cfg::gEnum::ProcedureType getStartingProcedure() const;

  void setProjectName(std::string);
  void setMainBoard(cfg::gEnum::MainBoard);
  void setStartingProcedure(cfg::gEnum::ProcedureType);

private:
  std::string project_name_ = "Giraffe Flight 1";
  cfg::gEnum::MainBoard main_board_ = cfg::gEnum::MainBoard::OTHER;
  cfg::gEnum::ProcedureType starting_procedure_ = cfg::gEnum::ProcedureType::FAILSAFE;
  mutable std::mutex cfg_lock_ = std::mutex();
};

class DataModuleGeneral {
public:
  int getFramePurgeTime() const;

  void setFramePurgeTime(int);

private:
  int frame_purge_time_ = 30000;
  mutable std::mutex cfg_lock_ = std::mutex();
};

} // namespace cfg

#endif // CONFIGURATION_JSON_HPP_

// * * * AUTOMATICALLY GENERATED WITH CMake/Python * * *
