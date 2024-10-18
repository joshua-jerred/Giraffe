add_library(telemetry_module INTERFACE)

target_sources(telemetry_module INTERFACE 
  ${CMAKE_CURRENT_SOURCE_DIR}/telemetry_module.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/aprs_telemetry_data_manager.cpp
)
target_include_directories(telemetry_module INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}
)
