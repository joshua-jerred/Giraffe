set(TEST_SRC ${CMAKE_SOURCE_DIR}/tests)

enable_testing()

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.12.1
)
FetchContent_MakeAvailable(googletest)

# gtest based test function ----------------------------------------------------
function(add_gtest_test test_name)
	add_executable(${test_name} ${ARGN})
	target_link_libraries(${test_name} gtest_main)
	add_test(NAME ${test_name} COMMAND ${test_name})
endfunction()

# test-01 - Data Stream --------------------------------------------------------
add_executable(test-01 
	${TEST_SRC}/test-01/test.cpp
	${GFS_SRC}/utilities/data-stream.cpp
)
set_target_properties(test-01 PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${TEST_BIN}/test-01/)
target_link_libraries(test-01 gtest_main)

# test-02 - Configuration ------------------------------------------------------
add_executable(test-02 
	${TEST_SRC}/test-02/test.cpp
	${GFS_SRC}/modules/module-configuration.cpp
	${GFS_SRC}/utilities/data-stream.cpp
)
set_target_properties(test-02 PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${TEST_BIN}/test-02/)
target_link_libraries(test-02 gtest_main)
target_link_libraries(test-02 nlohmann_json::nlohmann_json)
file(COPY "test-02/input-1.json" DESTINATION ${TEST_BIN}/test-02)

# test-03 - Data Module --------------------------------------------------------
add_executable(test-03 
	${TEST_SRC}/test-03/test.cpp
	${GFS_SRC}/modules/module-data.cpp
	${GFS_SRC}/utilities/data-stream.cpp
)
set_target_properties(test-03 PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${TEST_BIN}/test-03/)
target_link_libraries(test-03 gtest_main)
target_link_libraries(test-03 nlohmann_json::nlohmann_json)

# test-04 - Extensions Module --------------------------------------------------
add_executable(test-04
	${TEST_SRC}/test-04/test.cpp
	${GFS_SRC}/modules/module-extensions.cpp

	${GFS_SRC}/extensions/extension.cpp
	${GFS_SRC}/extensions/simulated-extensions.cpp
	${GFS_SRC}/extensions/bmp180.cpp
	${GFS_SRC}/extensions/ds18b20.cpp
	${GFS_SRC}/extensions/samm8q.cpp
	${GFS_SRC}/extensions/bme280.cpp
	${GFS_SRC}/extensions/sysinfo.cpp

	${GFS_SRC}/extension-interface/i2c.cpp
	${GFS_SRC}/extension-interface/one-wire.cpp
	
	${GFS_SRC}/utilities/data-stream.cpp
	${GFS_SRC}/utilities/ubx.cpp
)
target_link_libraries(test-04 i2c)
set_target_properties(test-04 PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${TEST_BIN}/test-04/)
target_link_libraries(test-04 gtest_main)
target_link_libraries(test-04 nlohmann_json::nlohmann_json)

# test-05 - I2C Utility --------------------------------------------------------
add_executable(test-05
	${TEST_SRC}/test-05/test.cpp
	${GFS_SRC}/extension-interface/i2c.cpp
)
target_link_libraries(test-05 i2c)
set_target_properties(test-05 PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${TEST_BIN}/test-05/)
target_link_libraries(test-05 gtest_main)

# test-06 - BMP180 Extension ---------------------------------------------------
add_executable(test-06
	${TEST_SRC}/test-06/test.cpp
	${GFS_SRC}/extensions/bmp180.cpp
	${GFS_SRC}/extensions/extension.cpp
	${GFS_SRC}/utilities/data-stream.cpp
	${GFS_SRC}/extension-interface/i2c.cpp
)
target_link_libraries(test-06 i2c)
set_target_properties(test-06 PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${TEST_BIN}/test-06/)
target_link_libraries(test-06 gtest_main)

# test-07 - DS18B20 Extension --------------------------------------------------
add_executable(test-07
	${TEST_SRC}/test-07/test.cpp
	${GFS_SRC}/extensions/ds18b20.cpp
	${GFS_SRC}/extensions/extension.cpp
	${GFS_SRC}/utilities/data-stream.cpp
	${GFS_SRC}/extension-interface/one-wire.cpp
)
target_link_libraries(test-07 i2c)
set_target_properties(test-07 PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${TEST_BIN}/test-07/)
target_link_libraries(test-07 gtest_main)

# test-08 - UBX Utility --------------------------------------------------------
add_executable(test-08
	${TEST_SRC}/test-08/test.cpp
	${GFS_SRC}/extension-interface/i2c.cpp
	${GFS_SRC}/utilities/ubx.cpp
)
target_link_libraries(test-08 i2c)
set_target_properties(test-08 PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${TEST_BIN}/test-08/)
target_link_libraries(test-08 gtest_main)

# test-09 - UBX Manual Test ----------------------------------------------------
add_executable(test-09
	${TEST_SRC}/test-09/test.cpp
	${GFS_SRC}/extension-interface/i2c.cpp
	${GFS_SRC}/utilities/ubx.cpp
)
target_link_libraries(test-09 i2c)
set_target_properties(test-09 PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${TEST_BIN}/test-09/)

# test-10 - BME280 Extension ---------------------------------------------------
add_executable(test-10
	${TEST_SRC}/test-10/test.cpp
	${GFS_SRC}/extensions/bme280.cpp
	${GFS_SRC}/extensions/extension.cpp
	${GFS_SRC}/utilities/data-stream.cpp
	${GFS_SRC}/extension-interface/i2c.cpp
)
target_link_libraries(test-10 i2c)
set_target_properties(test-10 PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${TEST_BIN}/test-10/)
target_link_libraries(test-10 gtest_main)

# test-11 - SYSINFO Extension --------------------------------------------------
add_executable(test-11
	${TEST_SRC}/test-11/test.cpp
	${GFS_SRC}/extensions/sysinfo.cpp
	${GFS_SRC}/extensions/extension.cpp
	${GFS_SRC}/utilities/data-stream.cpp
)
set_target_properties(test-11 PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${TEST_BIN}/test-11/)
target_link_libraries(test-11 gtest_main)

# test-12 - MCP3021 Extension --------------------------------------------------
add_executable(test-12
	${TEST_SRC}/test-12/test.cpp
	${GFS_SRC}/extensions/mcp3021.cpp
	${GFS_SRC}/extensions/extension.cpp
	${GFS_SRC}/utilities/data-stream.cpp
	${GFS_SRC}/extension-interface/i2c.cpp
)
target_link_libraries(test-12 i2c)
set_target_properties(test-12 PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${TEST_BIN}/test-12/)
target_link_libraries(test-12 gtest_main)

# test-13 - Image Tools --------------------------------------------------------
add_executable(test-13
	${TEST_SRC}/test-13/test.cpp
	${GFS_SRC}/utilities/image-tools.cpp
)
set_target_properties(test-13 PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${TEST_BIN}/test-13/)
file(COPY "test-13/test-pattern.png" DESTINATION ${TEST_BIN}/test-13)
file(COPY "test-13/test-image.png" DESTINATION ${TEST_BIN}/test-13)

# ImageMagick
add_definitions(-DMAGICKCORE_QUANTUM_DEPTH=16)
add_definitions(-DMAGICKCORE_HDRI_ENABLE=0)
find_package(ImageMagick COMPONENTS Magick++)
include_directories(${ImageMagick_INCLUDE_DIRS})
target_link_libraries(test-13 ${ImageMagick_LIBRARIES} )

# test-14 - Serial Interface ---------------------------------------------------
add_gtest_test(test-14
	${TEST_SRC}/test-14/test.cpp
	${GFS_SRC}/extension-interface/serial.cpp
)