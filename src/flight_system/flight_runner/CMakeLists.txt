add_library(gfs_flight_runner INTERFACE)
target_sources(gfs_flight_runner INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}/built_in_test/bit_types.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/flight_runner.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/flight_logic.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/flight_runner_data.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/flight_phase/prediction_parameters.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/flight_phase/flight_phase_predictor.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/flight_phase/prediction_model.cpp
)
target_include_directories(gfs_flight_runner INTERFACE 
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/flight_phase
  ${CMAKE_CURRENT_SOURCE_DIR}/built_in_test
)