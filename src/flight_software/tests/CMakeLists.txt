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

add_gtest_test(
	test_01
	gfs_01_configuration_validators
	${GFS_SRC}/modules/configuration/validators.cpp
)

add_gtest_test(
	test_02
	gfs_02_configuration_class
	${GFS_SRC}/modules/configuration/configuration.cpp
	${GFS_SRC}/modules/configuration/validators.cpp
)

add_gtest_test(
	test_03
	gfs_03_configuration_file
	${GFS_SRC}/modules/configuration/configuration.cpp
	${GFS_SRC}/modules/configuration/validators.cpp
	${GFS_SRC}/modules/configuration/file.cpp
)
target_link_libraries(gfs_03_configuration_file nlohmann_json::nlohmann_json)

#configure_file(${GFS_TEST_SRC}/test_01/01_valid_config.yaml ${GFS_TEST_BIN}/01_valid_config.yaml)
#configure_file(${GFS_TEST_SRC}/test_01/01_empty_config.yaml ${GFS_TEST_BIN}/01_empty_config.yaml)
#configure_file(${GFS_TEST_SRC}/test_01/01_invalid_config.yaml ${GFS_TEST_BIN}/01_invalid_config.yaml)


#add_executable(test-01-configuration
#	${TEST_SRC}/test-01/test.cpp
#	${GFS_SRC}/modules/configuration/configuration.cpp
#)
#set_target_properties(test-01 PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${TEST_BIN}/test-01/)
#target_link_libraries(test-01 gtest_main)