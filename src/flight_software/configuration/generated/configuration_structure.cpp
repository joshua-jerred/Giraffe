// * * * AUTOMATICALLY GENERATED WITH CMake/Python * * *

#include "configuration_structure.hpp"

std::string cfg::General::getProjectName() const {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  return project_name_;
}

cfg::gEnum::MainBoard cfg::General::getMainBoard() const {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  return main_board_;
}

cfg::gEnum::ProcedureType cfg::General::getStartingProcedure() const {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  return starting_procedure_;
}

int cfg::DataModuleGeneral::getFramePurgeTime() const {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  return frame_purge_time_;
}

void cfg::General::setProjectName(std::string val) {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  project_name_ = val;
}

void cfg::General::setMainBoard(cfg::gEnum::MainBoard val) {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  main_board_ = val;
}

void cfg::General::setStartingProcedure(cfg::gEnum::ProcedureType val) {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  starting_procedure_ = val;
}

void cfg::DataModuleGeneral::setFramePurgeTime(int val) {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  frame_purge_time_ = val;
}

