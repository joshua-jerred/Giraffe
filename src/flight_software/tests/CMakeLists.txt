set(TEST_SRC ${CMAKE_SOURCE_DIR}/tests)

enable_testing()

# gtest based test function ----------------------------------------------------
function(add_gtest_test test_id test_name)
	add_executable(${test_name}
	${GFS_TEST_SRC}/${test_id}/test.cpp
		${ARGN}
	)
	target_link_libraries(${test_name} gtest_main)
	add_test(NAME ${test_name} COMMAND ${test_name})
	gtest_discover_tests(${test_name})
endfunction()


# test-01 - Configuration ------------------------------------------------------
add_gtest_test(
	test_01
	01_configuration_definition
)
target_link_libraries(01_configuration_definition yaml-cpp)
#configure_file()

#add_executable(test-01-configuration
#	${TEST_SRC}/test-01/test.cpp
#	${GFS_SRC}/modules/configuration/configuration.cpp
#)
#set_target_properties(test-01 PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${TEST_BIN}/test-01/)
#target_link_libraries(test-01 gtest_main)