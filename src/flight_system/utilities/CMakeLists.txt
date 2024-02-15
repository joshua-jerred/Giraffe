add_library(gfs_utilities INTERFACE)
target_sources(gfs_utilities INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}/flight_computer_info.cpp
)
target_include_directories(gfs_utilities INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}
)