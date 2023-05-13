set(TEST_SRC ${CMAKE_SOURCE_DIR}/tests)

enable_testing()

# gtest based test function ----------------------------------------------------
function(add_gtest_test test_id test_name)
	add_executable(${test_name}
	${COMMON_TEST_SRC}/${test_id}/test.cpp
		${ARGN}
	)
	target_link_libraries(${test_name} gtest_main)
	add_test(NAME ${test_name} COMMAND ${test_name})
	gtest_discover_tests(${test_name})
endfunction()


# test-01 - Configuration ------------------------------------------------------
add_gtest_test(
	test_01
	common_01_configuration_scheme
)
target_link_libraries(common_01_configuration_scheme nlohmann_json::nlohmann_json)
configure_file(${COMMON_SRC}/gfs_configuration_metadata.json ${COMMON_TEST_BIN}/gfs_configuration_metadata.json)