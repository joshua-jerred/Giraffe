# Unit Tests
add_library(unit_test INTERFACE)
target_include_directories(unit_test INTERFACE "./")
target_link_libraries(unit_test INTERFACE gtest_main gmock_main)

# --- Common ---
file(GLOB common_test_sources "$common/test_*.cpp")
add_executable(unit_test_common ${common_test_sources} ${common_sources})
target_link_libraries(
  unit_test_common
  # internal
  giraffe_common
  unit_test
  # external
  gtest_main
  BoosterSeat
  nlohmann_json::nlohmann_json)
target_include_directories(unit_test_common PRIVATE ${common_include_dirs})
set_target_properties(unit_test_common PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                                                  ${TEST_BIN})

# --- Data Link ---
file(GLOB data_link_test_sources "${GDL_TEST_SRC}/test_*.cpp")
add_executable(gdl_test ${data_link_test_sources})
target_link_libraries(
  gdl_test
  # internal
  giraffe_data_link_core
  giraffe_common
  # ax25_state_machine
  unit_test
  # external
  BoosterSeat)
target_include_directories(gdl_test PRIVATE ${GDL_SRC})
set_target_properties(gdl_test PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                                               ${TEST_BIN})
target_include_directories(
  gdl_test
  PRIVATE
  ${THIRD_PARTY_SRC}/nlohmann_json/single_include
)
file(COPY ${GDL_TEST_SRC}/sstv_image.png DESTINATION ${TEST_BIN})

# --- Flight Software ---
file(GLOB gfs_test_sources "${GFS_UNIT_TEST_SRC}/test_*.cpp")
add_executable(giraffe_flight_system_test ${gfs_test_sources} ${gfs_sources})
target_include_directories(giraffe_flight_system_test PUBLIC
  ${gfs_include_dirs}
  "./flight_system"
)
target_link_libraries(
  giraffe_flight_system_test
  # internal
  giraffe_common
  giraffe_data_link_core
  gfs_flight_runner
  gfs_interface
  gfs_shared_data
  gfs_configuration
  gfs_extensions
  gfs_control
  module_base
  data_module
  console_module
  extension_module
  server_module
  system_module
  telemetry_module
  gfs_utilities
  # external
  unit_test
  gtest_main
  BoosterSeat
  ${CURSES_LIBRARIES}
  ${CURL_LIBRARIES}
  nlohmann_json::nlohmann_json)
set_target_properties(giraffe_flight_system_test PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                                               ${TEST_BIN})
target_compile_definitions(giraffe_flight_system_test PRIVATE
  GFS_UNIT_TEST_BUILD=1
)

# --- Flight Software : On Pi Tests ---
file(GLOB gfs_on_pi_test_sources "${GFS_UNIT_TEST_SRC}/host_tests/test_*.cpp")
add_executable(
  unit_test_on_pi_host "${GFS_UNIT_TEST_SRC}/host_tests/extension_test_framework.cpp"
                     ${gfs_on_pi_test_sources})
target_link_libraries(
  unit_test_on_pi_host
  giraffe_common
  giraffe_debug_assert
  gfs_interface
  gfs_extensions
  gfs_configuration
  gfs_shared_data
  gfs_control
  gfs_utilities
  module_base
  data_module
  BoosterSeat
  unit_test
  gtest_main
  ${CURL_LIBRARIES}
  nlohmann_json::nlohmann_json)
set_target_properties(unit_test_on_pi_host
  PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${TEST_BIN})
target_compile_options(unit_test_on_pi_host PRIVATE -Os)

add_executable(curl_test
  ./flight_system/curl_test.cpp
)
include_directories(curl_test
  ${CURL_INCLUDE_DIRS}
)
target_link_libraries(curl_test
  ${CURL_LIBRARIES}
  giraffe_debug_assert
  giraffe_common
  gfs_configuration
  gfs_shared_data
  gfs_control

  module_base
  data_module

  BoosterSeat
  nlohmann_json::nlohmann_json
)
set_target_properties(
	curl_test PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY ${GFS_TEST_BIN}
)

# ---------------- ctest ----------------
include(GoogleTest)
gtest_discover_tests(giraffe_flight_system_test)
gtest_discover_tests(gdl_test)
gtest_discover_tests(unit_test_common)
# add_test(NAME common_tests COMMAND $<TARGET_FILE:unit_test_common>)
# add_test(NAME flight_system_tests COMMAND giraffe_flight_system_test)
# add_test(NAME data_link_tests COMMAND gdl_test)

# add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND}
                  # DEPENDS ExeName)
