add_library(gfs_control INTERFACE)
target_sources(gfs_control INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}/command_processor.cpp
)
target_include_directories(gfs_control INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}
)
