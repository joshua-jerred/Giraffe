add_library(giraffe_data_link_core STATIC
  "./giraffe_data_link.cpp"
  "./message_queue.cpp"
  "./network_layer.cpp"
  "./transport_layer.cpp"
)
target_include_directories(giraffe_data_link_core PUBLIC include)
target_include_directories(giraffe_data_link_core PRIVATE "./")
target_link_libraries(giraffe_data_link_core
  giraffe_common
  giraffe_debug_assert
  BoosterSeat
  SignalEasel
)
target_include_directories(
  giraffe_data_link_core
  PRIVATE
  ${THIRD_PARTY_SRC}/nlohmann_json/single_include
)
if(SSTV_ENABLED)
  target_compile_definitions(giraffe_data_link_core PRIVATE SSTV_ENABLED)
  add_definitions(-DMAGICKCORE_QUANTUM_DEPTH=8)
  add_definitions(-DMAGICKCORE_HDRI_ENABLE=1)
  find_package(ImageMagick COMPONENTS Magick++)
  target_include_directories(giraffe_data_link_core SYSTEM PRIVATE ${ImageMagick_INCLUDE_DIRS})
endif()

add_executable(gdl_server
  "server/main.cpp"
  "server/gdl_server.cpp"
  "server/gdl_server_request_handlers.cpp"
  "server/gdl_server_set_handlers.cpp"
)
target_link_libraries(gdl_server
  giraffe_data_link_core
  SQLiteCpp
  sqlite3
  fmt::fmt
)
target_include_directories(gdl_server PRIVATE "./server")
set_target_properties(gdl_server
  PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${GDL_BIN}
)
target_include_directories(
  gdl_server
  PRIVATE
  ${THIRD_PARTY_SRC}/nlohmann_json/single_include
)

add_executable(gdl_terminal
  "./gdl_terminal.cpp"
)
target_link_libraries(gdl_terminal
  giraffe_data_link_core
)
set_target_properties(gdl_terminal
  PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${GDL_BIN}
)
target_include_directories(
  gdl_terminal
  PRIVATE
  ${THIRD_PARTY_SRC}/nlohmann_json/single_include
)
