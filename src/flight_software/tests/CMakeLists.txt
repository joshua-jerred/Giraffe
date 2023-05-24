enable_testing()

include_directories(
	${GFS_SRC}/
	${GFS_SRC}/configuration/generated
	${GFS_SRC}/configuration
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
	gfs_01_configuration
)
target_link_libraries(gfs_01_configuration nlohmann_json::nlohmann_json)

add_gtest_test(
	test_02
	gfs_02_gfs_exception
)

add_gtest_test(
	test_03
	gfs_03_data_error_stream
	${data_module_sources}
	${module_sources}
)
target_link_libraries(gfs_03_data_error_stream nlohmann_json::nlohmann_json)

add_gtest_test(
	test_04
	gfs_04_module
	${data_module_sources}
	${module_sources}
)
target_link_libraries(gfs_04_module nlohmann_json::nlohmann_json)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(gfs_04_module Threads::Threads)