// * * * AUTOMATICALLY GENERATED WITH CMake/Python * * *

#include <filesystem>
#include "configuration.hpp"
#include "validation.hpp"

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
  validation::setValidValue<std::string>(
        streams_.log,
        json_data,
        "general",
        "project_name",
        project_name_,
        1,
        20,
        "^(?!\\s)[a-zA-Z0-9_ -]{0,19}[^\\s]$"
  );
  validation::setValidEnum<cfg::gEnum::MainBoard>(
        streams_.log,
        json_data,
        "general",
        "main_board",
        main_board_,
        cfg::gEnum::KeyToMainBoard
  );
  validation::setValidEnum<cfg::gEnum::ProcedureType>(
        streams_.log,
        json_data,
        "general",
        "starting_procedure",
        starting_procedure_,
        cfg::gEnum::KeyToProcedureType
  );
}

json cfg::General::getJson() const {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  return json({
    {"project_name", project_name_},
    {"main_board", cfg::gEnum::MainBoardToKey(main_board_)},
    {"starting_procedure", cfg::gEnum::ProcedureTypeToKey(starting_procedure_)}
  });
}

void cfg::Configuration::save(std::string file_path) {
  const std::lock_guard<std::mutex> lock(file_lock_);
  
  std::ofstream out(file_path);
  
  if (out.fail()) {
    error(data::logId::Config_failedToSaveToPath, file_path);
    return;
  }
  
  json config_json = {
    {"general", general.getJson()}
  };
  
  constexpr int json_indent = 2;
  std::string data = config_json.dump(json_indent);
  out << data;
}

inline bool sectionExists(const json &all_data, const std::string &section_key) {
  return all_data.contains(section_key);
}

void cfg::Configuration::load(std::string file_path) {
  const std::lock_guard<std::mutex> lock(file_lock_);

  if (!std::filesystem::exists(file_path)) {
    error(data::logId::Config_failedToLoadFromPathDoesNotExist, file_path);
    return; 
  }

  std::ifstream in(file_path);
  
  if (in.fail()) {
    error(data::logId::Config_failedToLoadFromPathFileOpenFailure, file_path);
    return;
  }
  
  json parsed;
  try {
    parsed = json::parse(in);
  } catch (json::parse_error &e) {
    return;
  }

  if (sectionExists(parsed, "general")) {
    general.setFromJson(parsed["general"]);
  } else {
    error(data::logId::Config_load_sectionNotFound, "general");
  }
}


// * * * AUTOMATICALLY GENERATED WITH CMake/Python * * *
// configuration.cpp
