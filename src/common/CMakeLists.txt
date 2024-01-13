add_library(giraffe_common INTERFACE)
target_sources(giraffe_common INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}/giraffe_exception.cpp
  # diagnostic/giraffe_debug_assert.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/data_ids_string_map.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/diagnostic_id_strings.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/command_ids_strings.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/protocol/protocol.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/socket.cpp
)
target_include_directories(giraffe_common INTERFACE
  ./
  ./protocol
)

add_library(giraffe_debug_assert INTERFACE)
target_sources(giraffe_debug_assert INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}/giraffe_debug_assert.cpp
)

add_library(curses_utilities INTERFACE)
target_sources(curses_utilities INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}/curses/ncurses_utilities.cpp
)
target_include_directories(curses_utilities INTERFACE
  ./curses
)
target_link_libraries(curses_utilities INTERFACE
  ${CURSES_LIBRARIES}
  BoosterSeat
)