add_library(system_module INTERFACE)
target_sources(system_module INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}/system_module.cpp
)
target_include_directories(system_module INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}
)