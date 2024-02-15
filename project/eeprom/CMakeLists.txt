add_executable(eepmake eepmake.c)
target_include_directories(eepmake
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_executable(eepdump eepdump.c)
target_include_directories(eepdump
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)
