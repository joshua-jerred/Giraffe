add_library(data_module INTERFACE)
target_sources(data_module INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}/data_module.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/data_formatting.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/data_log.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/influxdb.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/positional.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/adc_interpolation.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/calculated_data.cpp
)
target_include_directories(data_module INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(data_module INTERFACE
	bzip2-cpp-lib
)
