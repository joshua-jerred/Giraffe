add_library(gfs_shared_data INTERFACE)
target_sources(gfs_shared_data INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}/error_frame.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/to_json.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/to_string.cpp
)
target_include_directories(gfs_shared_data INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}
)