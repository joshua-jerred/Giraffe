add_library(extension_module INTERFACE)
target_sources(extension_module INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}/extension_module.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/preconfigured_extensions.cpp
)
target_include_directories(extension_module INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}
)