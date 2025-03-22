
add_library(giraffe_daemon_core INTERFACE)
target_sources(giraffe_daemon_core INTERFACE
  "./update_pack.cpp"
)

target_include_directories(giraffe_daemon_core INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/software_update
)

target_link_libraries(giraffe_daemon_core INTERFACE
  giraffe_common
  BoosterSeat
  # giraffe_debug_assert
  # SignalEasel
)

add_executable(giraffe_daemon
  "./main.cpp"
  "./agent_data.cpp"
  "./ground_software_manager.cpp"
)
target_link_libraries(giraffe_daemon
  giraffe_common
  BoosterSeat
  # giraffe_debug_assert
  # SignalEasel
)
target_include_directories(giraffe_daemon PRIVATE
  ${THIRD_PARTY_SRC}/nlohmann_json/single_include
)

set_target_properties(giraffe_daemon PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${GFS_BIN} OUTPUT_NAME giraffe_daemon
  EXPORT_COMPILE_COMMANDS ON
)