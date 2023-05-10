set(TEST_SRC ${CMAKE_SOURCE_DIR}/tests)

enable_testing()

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.12.1
)
FetchContent_MakeAvailable(googletest)

include_directories(
	${GFS_SRC}/flight-computer
	${GFS_SRC}/modules
	${GFS_SRC}/utilities
	${GFS_SRC}/extensions
	${GFS_SRC}/interface
	${GFS_SRC}/radios

	${LIB_SRC}/MWAV/include
)

# gtest based test function ----------------------------------------------------
function(add_gtest_test test_id test_name)
	add_executable(${test_name}
	${GFS_TEST_SRC}/${test_id}/test.cpp
		${ARGN}
	)
	target_link_libraries(${test_name} gtest_main)
	add_test(NAME ${test_name} COMMAND ${test_name})
endfunction()

# test-01 - Configuration ------------------------------------------------------
add_gtest_test(
	test_01
	01_configuration
	${GFS_SRC}/modules/configuration/configuration.cpp
)

#add_executable(test-01-configuration
#	${TEST_SRC}/test-01/test.cpp
#	${GFS_SRC}/modules/configuration/configuration.cpp
#)
#set_target_properties(test-01 PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${TEST_BIN}/test-01/)
#target_link_libraries(test-01 gtest_main)