add_library(gfs_flight_runner INTERFACE)
target_sources(gfs_flight_runner INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}/flight_runner.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/flight_logic.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/flight_runner_data.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/phase_detection/detection_data.cpp
)
target_include_directories(gfs_flight_runner INTERFACE 
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/phase_detection
)