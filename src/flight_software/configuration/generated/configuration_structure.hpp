// * * * AUTOMATICALLY GENERATED WITH CMake/Python * * *

#ifndef CONFIGURATION_STRUCTURE_HPP_
#define CONFIGURATION_STRUCTURE_HPP_

#include <string>

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

namespace sections {

struct General {
  std::string project_name = "Giraffe Flight 1";
  cfg::gEnum::MainBoard main_board = cfg::gEnum::MainBoard::OTHER;
  cfg::gEnum::ProcedureType starting_procedure = cfg::gEnum::ProcedureType::FAILSAFE;
};

struct DataModGen {
  int frame_purge_time = 30000;
};

} // namespace sections

} // namespace cfg

#endif // CONFIGURATION_STRUCTURE_HPP_

// * * * AUTOMATICALLY GENERATED WITH CMake/Python * * *
