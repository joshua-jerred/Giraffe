add_library(console_module INTERFACE)
target_sources(console_module INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}/console_module.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/console_pages.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/curses_utilities.cpp
)
target_include_directories(console_module INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}
)
