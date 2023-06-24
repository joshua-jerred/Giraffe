# Unit Tests

# --- Common ---
file(GLOB common_test_sources
     "${COMMON_TEST_SRC}/test_*.cpp"
)

add_executable(unit_test_common
  ${common_test_sources}
)
target_link_libraries(unit_test_common 
  gtest_main
  # Common
  giraffe_common
)
set_target_properties(
	unit_test_common PROPERTIES 
	RUNTIME_OUTPUT_DIRECTORY ${COMMON_TEST_BIN}
)
target_link_libraries(unit_test_common nlohmann_json::nlohmann_json)

add_test(
  NAME common_tests
  COMMAND unit_test_common
)

# --- Flight Software ---
file(GLOB gfs_test_sources
     "${GFS_TEST_SRC}/test_*.cpp"
)

add_executable(unit_test_gfs
  ${gfs_test_sources}
  ${gfs_sources}
)
target_include_directories(unit_test_gfs PRIVATE ${gfs_include_dirs})
target_link_libraries(unit_test_gfs 
  gtest_main
  BoosterSeat
  ${CURSES_LIBRARIES}
  nlohmann_json::nlohmann_json
)
set_target_properties(
	unit_test_gfs PROPERTIES 
	RUNTIME_OUTPUT_DIRECTORY ${GFS_TEST_BIN}
)

add_test(
  NAME flight_system_tests
  COMMAND unit_test_gfs
)