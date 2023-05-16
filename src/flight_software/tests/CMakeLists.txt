enable_testing()

include_directories(
	${GFS_SRC}/modules
	${GFS_SRC}/modules/configuration
	${GFS_SRC}/modules/data
)


# gtest based test function ----------------------------------------------------
function(add_gtest_test test_id test_name)
	add_executable(${test_name}
	${GFS_TEST_SRC}/${test_id}/test.cpp
		${ARGN}
	)
	target_link_libraries(${test_name} gtest_main)
	add_test(NAME ${test_name} COMMAND ${test_name})
	gtest_discover_tests(${test_name})

	file(MAKE_DIRECTORY ${GFS_TEST_BIN}/${test_id})
	set_target_properties(${test_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${GFS_TEST_BIN}/${test_id})
endfunction()

include(${GFS_SRC}/sourcelist.CMake)

add_gtest_test(
	test_01
	gfs_01_configuration_validators
	${configuration_module_sources}
)
target_link_libraries(gfs_01_configuration_validators nlohmann_json::nlohmann_json)

add_gtest_test(
	test_02
	gfs_02_configuration_class
	${configuration_module_sources}
)
target_link_libraries(gfs_02_configuration_class nlohmann_json::nlohmann_json)


add_gtest_test(
	test_03
	gfs_03_configuration_json
	${configuration_module_sources}
)
target_link_libraries(gfs_03_configuration_json nlohmann_json::nlohmann_json)
configure_file(${COMMON_SRC}/gfs_configuration_metadata.json ${GFS_TEST_BIN}/test_03/gfs_configuration_metadata.json)

add_gtest_test(
	test_04
	gfs_04_configuration_file
	${configuration_module_sources}
	${data_module_sources}
)
target_link_libraries(gfs_04_configuration_file nlohmann_json::nlohmann_json)
configure_file(${COMMON_SRC}/gfs_configuration_metadata.json ${GFS_TEST_BIN}/test_04/gfs_configuration_metadata.json)

add_gtest_test(
	test_05
	gfs_05_gfs_exception
)

add_gtest_test(
	test_06
	gfs_06_data_error_stream
	${data_module_sources}
)

add_gtest_test(
	test_07
	gfs_07_module
	${data_module_sources}
	${module_sources}
)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(gfs_07_module Threads::Threads)