
add_library(gcli_core INTERFACE)
target_sources(gcli_core INTERFACE
  "./update_pack.cpp"
)
target_link_libraries(gcli_core INTERFACE
  giraffe_common
  BoosterSeat
  # giraffe_debug_assert
  # SignalEasel
)

add_executable(giraffe_command_line_interface
  "./main.cpp"
  "./agent_data.cpp"
  "./ground_software_manager.cpp"
)
target_link_libraries(giraffe_command_line_interface
  giraffe_common
  BoosterSeat
  # giraffe_debug_assert
  # SignalEasel
)
target_include_directories(giraffe_command_line_interface PRIVATE
  ${THIRD_PARTY_SRC}/nlohmann_json/single_include
)

set_target_properties(giraffe_command_line_interface PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${GFS_BIN} OUTPUT_NAME gcli
  EXPORT_COMPILE_COMMANDS ON
)