add_library(server_module INTERFACE)
target_sources(server_module INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}/server_module.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/request_handler.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/request_handler_settings.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/request_handler_data.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/request_handler_image.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/request_handler_bit_test.cpp
)
target_include_directories(server_module INTERFACE 
  ${CMAKE_CURRENT_SOURCE_DIR}
)