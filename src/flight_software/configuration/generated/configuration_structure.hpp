// * * * AUTOMATICALLY GENERATED WITH CMake/Python * * *

#ifndef CONFIGURATION_STRUCTURE_HPP_
#define CONFIGURATION_STRUCTURE_HPP_

#include <string>
#include <mutex>

namespace cfg {

namespace gEnum {


enum class MainBoard {
  OTHER,
  PI_ZERO_W2,
  PI_4
};

enum class ProcedureType {
  TESTING,
  PRE_LAUNCH,
  ASCENT,
  DESCENT,
  RECOVERY,
  FAILSAFE
};

} // namespace gEnum

class General {
public:
  std::string getProjectName() const;
  cfg::gEnum::MainBoard getMainBoard() const;
  cfg::gEnum::ProcedureType getStartingProcedure() const;

  void setProjectName(std::string);
  void setMainBoard(cfg::gEnum::MainBoard);
  void setStartingProcedure(cfg::gEnum::ProcedureType);

private:
  std::string project_name = "Giraffe Flight 1";
  cfg::gEnum::MainBoard main_board = cfg::gEnum::MainBoard::OTHER;
  cfg::gEnum::ProcedureType starting_procedure = cfg::gEnum::ProcedureType::FAILSAFE;
  mutable std::mutex cfg_lock_ = std::mutex();
};

class DataModuleGeneral {
public:
  int getFramePurgeTime() const;

  void setFramePurgeTime(int);

private:
  int frame_purge_time = 30000;
  mutable std::mutex cfg_lock_ = std::mutex();
};

} // namespace cfg

#endif // CONFIGURATION_STRUCTURE_HPP_

// * * * AUTOMATICALLY GENERATED WITH CMake/Python * * *
