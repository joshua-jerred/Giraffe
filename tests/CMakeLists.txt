# Unit Tests
add_library(unit_test INTERFACE)
target_include_directories(unit_test INTERFACE "./")
target_link_libraries(unit_test INTERFACE gtest_main)

# --- Common ---
file(GLOB common_test_sources
     "${COMMON_TEST_SRC}/test_*.cpp"
)
add_executable(unit_test_common
  ${common_test_sources}
  ${common_sources}
)
target_link_libraries(unit_test_common 
  # internal
  giraffe_common
  unit_test

  # external
  gtest_main
  BoosterSeat
  nlohmann_json::nlohmann_json
)
target_include_directories(unit_test_common PRIVATE ${common_include_dirs})
set_target_properties(
	unit_test_common PROPERTIES 
	RUNTIME_OUTPUT_DIRECTORY ${COMMON_TEST_BIN}
)
add_test(
  NAME common_tests
  COMMAND unit_test_common
)

# --- Data Link ---
file(GLOB data_link_test_sources
     "${GDL_TEST_SRC}/test_*.cpp"
)
add_executable(unit_test_gdl
  ${data_link_test_sources}
)
target_link_libraries(unit_test_gdl
  # internal
  giraffe_data_link
  giraffe_common
  ax25_state_machine
  unit_test

  # external
  BoosterSeat
)
target_include_directories(unit_test_gdl PRIVATE ${GDL_SRC})
set_target_properties(
	unit_test_gdl PROPERTIES 
	RUNTIME_OUTPUT_DIRECTORY ${GDL_TEST_BIN}
)
add_test(
  NAME data_link_tests
  COMMAND unit_test_gdl
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
  # internal  
  giraffe_common
  gfs_module_base
  gfs_interface
  gfs_extensions
  gfs_configuration
  gfs_data_module

  # external
  unit_test
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

# --- Flight Software : Host Tests ---
file(GLOB gfs_host_test_sources
     "${GFS_TEST_SRC}/host_tests/test_*.cpp"
)
add_executable(unit_test_gfs_host
  ${gfs_host_test_sources}
)
target_link_libraries(unit_test_gfs_host 
  giraffe_common
  giraffe_debug_assert

  gfs_module_base
  gfs_interface
  gfs_extensions
  gfs_configuration
  gfs_data_module
  
  BoosterSeat
  unit_test

  gtest_main
  nlohmann_json::nlohmann_json
)
set_target_properties(
	unit_test_gfs_host PROPERTIES 
	RUNTIME_OUTPUT_DIRECTORY ${GFS_TEST_BIN}
)