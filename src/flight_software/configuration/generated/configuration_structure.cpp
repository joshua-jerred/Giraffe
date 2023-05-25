// * * * AUTOMATICALLY GENERATED WITH CMake/Python * * *

#include "configuration_structure.hpp"

std::string cfg::General::getProjectName() const {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  return project_name;
}

cfg::gEnum::MainBoard cfg::General::getMainBoard() const {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  return main_board;
}

cfg::gEnum::ProcedureType cfg::General::getStartingProcedure() const {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  return starting_procedure;
}

int cfg::DataModuleGeneral::getFramePurgeTime() const {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  return frame_purge_time;
}

void cfg::General::setProjectName(std::string val) {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  project_name = val;
}

void cfg::General::setMainBoard(cfg::gEnum::MainBoard val) {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  main_board = val;
}

void cfg::General::setStartingProcedure(cfg::gEnum::ProcedureType val) {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  starting_procedure = val;
}

void cfg::DataModuleGeneral::setFramePurgeTime(int val) {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  frame_purge_time = val;
}

