# This one is messy, it should be cleaned up eventually

file(MAKE_DIRECTORY ${GFS_BIN})

if(SSTV_ENABLED)
  message(STATUS "SSTV Enabled In Giraffe Flight System")
  add_compile_definitions(SSTV_ENABLED=1)
else()
  message(STATUS "SSTV Disabled In Giraffe Flight System")
  add_compile_definitions(SSTV_ENABLED=0)
endif()

set(EXE_NAME giraffe_flight_system)

add_library(module_base INTERFACE)
target_sources(module_base
               INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/modules/module.cpp)
target_include_directories(module_base INTERFACE ${GFS_SRC}/modules)

# The following directories contain interface libraries.
add_subdirectory(./shared_data)
add_subdirectory(./extensions)
add_subdirectory(./configuration)
add_subdirectory(./interface)
add_subdirectory(./control)
add_subdirectory(./utilities)
add_subdirectory(./modules/extension)
add_subdirectory(./modules/console)
add_subdirectory(./modules/data)
add_subdirectory(./modules/server)
add_subdirectory(./modules/system)
add_subdirectory(./modules/telemetry)

# -------- GFS Executable ---------
add_executable(${EXE_NAME}
  ${GFS_SRC}/main.cpp
  ${GFS_SRC}/flight_runner.cpp
  ${GFS_SRC}/flight_runner_data.cpp
  ${GFS_SRC}/flight_logic.cpp
)
target_include_directories(${EXE_NAME} PRIVATE ${GFS_SRC})
set_target_properties(
  ${EXE_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${GFS_BIN} OUTPUT_NAME gfs
                         EXPORT_COMPILE_COMMANDS ON)
target_link_libraries(
  ${EXE_NAME}
  # -------- First Party Libraries ---------
  # internal
  giraffe_data_link_core
  giraffe_common
    # gfs specific
  gfs_interface
  gfs_shared_data
  gfs_configuration
  gfs_extensions
  gfs_control
  gfs_utilities
  module_base
  data_module
  console_module
  extension_module
  server_module
  system_module
  telemetry_module
  # external
  BoosterSeat
  # -------- Third Party Libraries ---------
  i2c
  Threads::Threads
  ${CURSES_LIBRARIES}
  ${CURL_LIBRARIES}
)
target_include_directories(
  ${EXE_NAME}
  PRIVATE
  ${THIRD_PARTY_SRC}/nlohmann_json/single_include
)

if(DEFINED ENV{RUN_IN_SIMULATOR})
  if ($ENV{RUN_IN_SIMULATOR} EQUAL "1")
    message(STATUS "=========================")
    message(STATUS "=== Simulator Enabled ===")
    message(STATUS "=========================")
    target_compile_definitions(${EXE_NAME} PRIVATE RUN_IN_SIMULATOR=1)
    target_link_libraries(${EXE_NAME} gfs_simulator)
  endif()
endif()
