add_library(keystone INTERFACE)
target_sources(keystone INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}/ubx.cpp
)

target_include_directories(keystone INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}/
)

