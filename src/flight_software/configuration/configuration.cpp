// * * * AUTOMATICALLY GENERATED WITH CMake/Python * * *

#include "configuration.hpp"

using json = nlohmann::ordered_json;

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

void cfg::General::setFromJson(const json &json_data) {
  const std::lock_guard<std::mutex> lock(cfg_lock_);

}

json cfg::General::getJson() const {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  return json({
    {"project_name", project_name_},
    {"main_board", cfg::gEnum::MainBoardToKey(main_board_)},
    {"starting_procedure", cfg::gEnum::StartingProcedureToKey(starting_procedure_)}
  });
}

// * * * AUTOMATICALLY GENERATED WITH CMake/Python * * *
// configuration.cpp
