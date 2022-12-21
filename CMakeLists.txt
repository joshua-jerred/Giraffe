cmake_minimum_required(VERSION 3.14)
project(Giraffe VERSION 0.1)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-omit-frame-pointer")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ansi -pedantic -Wall -Wextra -Weffc++ -Wdisabled-optimization -Wfloat-equal")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)

include(FetchContent) # for gtest and nlohmann_json

# nlhomann_json
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)
FetchContent_MakeAvailable(json)

# ---- Flight Computer ----
add_executable(giraffe-flight-software

    src/flight-computer/main.cpp
    src/flight-computer/flight-runner.cpp

    src/flight-computer/modules/module-configuration.cpp
    src/flight-computer/modules/module-data.cpp
    src/flight-computer/modules/module-extensions.cpp
    src/flight-computer/modules/module-console.cpp
    src/flight-computer/modules/module-telemetry.cpp
    src/flight-computer/modules/module-server.cpp

    src/flight-computer/utilities/utility-timer.cpp
    src/flight-computer/utilities/utility-data-stream.cpp
    src/flight-computer/utilities/utility-socket.cpp
    src/flight-computer/utilities/utility-i2c.cpp
    src/flight-computer/utilities/utility-one-wire.cpp
    src/flight-computer/utilities/utility-psk.cpp

    src/flight-computer/extensions/extension.cpp
    src/flight-computer/extensions/simulated-extensions.cpp
    src/flight-computer/extensions/bmp180.cpp
    src/flight-computer/extensions/ds18b20.cpp
)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(giraffe-flight-software PRIVATE Threads::Threads)

target_link_libraries(giraffe-flight-software PUBLIC i2c)

target_link_libraries(giraffe-flight-software PRIVATE nlohmann_json::nlohmann_json)

file(COPY "src/flight-computer/config.json" DESTINATION ${CMAKE_SOURCE_DIR}/bin)
file(COPY "src/flight-computer/web-server" DESTINATION ${CMAKE_SOURCE_DIR}/bin)
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/data_logs)
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/error_logs)
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/telem)

target_include_directories(giraffe-flight-software PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/flight-computer
    ${CMAKE_CURRENT_SOURCE_DIR}/src/flight-computer/modules
    ${CMAKE_CURRENT_SOURCE_DIR}/src/flight-computer/utilities
    ${CMAKE_CURRENT_SOURCE_DIR}/src/flight-computer/extensions
)

# ------------------------------------------------------------------------------
# ---- Flight Computer Tests ---------------------------------------------------
# ------------------------------------------------------------------------------
enable_testing()

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.12.1
)
FetchContent_MakeAvailable(googletest)

# test-01 - Data Stream --------------------------------------------------------
add_executable(test-01 
    tests/test-01/test.cpp
    src/flight-computer/utilities/utility-data-stream.cpp
)
set_target_properties( test-01 PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/tests/test-01/ )
target_include_directories(test-01 PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/flight-computer
    ${CMAKE_CURRENT_SOURCE_DIR}/src/flight-computer/modules
    ${CMAKE_CURRENT_SOURCE_DIR}/src/flight-computer/utilities
)
target_link_libraries(test-01 gtest_main)

# test-02 - Configuration ------------------------------------------------------
add_executable(test-02 
    tests/test-02/test.cpp
    src/flight-computer/modules/module-configuration.cpp
    src/flight-computer/utilities/utility-data-stream.cpp
)
set_target_properties( test-02 PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/tests/test-02/ )
target_include_directories(test-02 PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/flight-computer
    ${CMAKE_CURRENT_SOURCE_DIR}/src/flight-computer/modules
    ${CMAKE_CURRENT_SOURCE_DIR}/src/flight-computer/utilities
)
target_link_libraries(test-02 gtest_main)
target_link_libraries(test-02 nlohmann_json::nlohmann_json)
file(COPY "tests/test-02/input-1.json" DESTINATION ${CMAKE_SOURCE_DIR}/bin/tests/test-02)

# test-03 - Data Module --------------------------------------------------------
add_executable(test-03 
    tests/test-03/test.cpp
    src/flight-computer/modules/module-data.cpp
    src/flight-computer/utilities/utility-data-stream.cpp
)
set_target_properties( test-03 PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/tests/test-03/ )
target_include_directories(test-03 PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/flight-computer
    ${CMAKE_CURRENT_SOURCE_DIR}/src/flight-computer/modules
    ${CMAKE_CURRENT_SOURCE_DIR}/src/flight-computer/utilities
)
target_link_libraries(test-03 gtest_main)

# test-04 - Extensions Module --------------------------------------------------
add_executable(test-04
    tests/test-04/test.cpp
    src/flight-computer/modules/module-extensions.cpp
    src/flight-computer/extensions/extension.cpp
    src/flight-computer/extensions/simulated-extensions.cpp
    src/flight-computer/extensions/bmp180.cpp
    src/flight-computer/extensions/ds18b20.cpp
    src/flight-computer/utilities/utility-data-stream.cpp
    src/flight-computer/utilities/utility-i2c.cpp
    src/flight-computer/utilities/utility-one-wire.cpp
)
target_link_libraries(test-04 i2c)
set_target_properties( test-04 PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/tests/test-04/ )
target_include_directories(test-04 PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/flight-computer
    ${CMAKE_CURRENT_SOURCE_DIR}/src/flight-computer/modules
    ${CMAKE_CURRENT_SOURCE_DIR}/src/flight-computer/utilities
    ${CMAKE_CURRENT_SOURCE_DIR}/src/flight-computer/extensions
)
target_link_libraries(test-04 gtest_main)

# test-05 - I2C Utility --------------------------------------------------------
add_executable(test-05
    tests/test-05/test.cpp
    src/flight-computer/utilities/utility-i2c.cpp
)
target_link_libraries(test-05 i2c)
set_target_properties( test-05 PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/tests/test-05/ )
target_include_directories(test-05 PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/flight-computer/utilities
)
target_link_libraries(test-05 gtest_main)

# test-06 - BMP180 Extension ---------------------------------------------------
add_executable(test-06
    tests/test-06/test.cpp
    src/flight-computer/extensions/bmp180.cpp
    src/flight-computer/extensions/extension.cpp
    src/flight-computer/utilities/utility-data-stream.cpp
    src/flight-computer/utilities/utility-i2c.cpp
)
target_link_libraries(test-06 i2c)
set_target_properties( test-06 PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/tests/test-06/ )
target_include_directories(test-06 PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/flight-computer/utilities
    ${CMAKE_CURRENT_SOURCE_DIR}/src/flight-computer/extensions
)
target_link_libraries(test-06 gtest_main)

# test-07 - DS18B20 Extension --------------------------------------------------
add_executable(test-07
    tests/test-07/test.cpp
    src/flight-computer/extensions/ds18b20.cpp
    src/flight-computer/extensions/extension.cpp
    src/flight-computer/utilities/utility-data-stream.cpp
    src/flight-computer/utilities/utility-one-wire.cpp
)
target_link_libraries(test-07 i2c)
set_target_properties( test-07 PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/tests/test-07/ )
target_include_directories(test-07 PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/flight-computer/utilities
    ${CMAKE_CURRENT_SOURCE_DIR}/src/flight-computer/extensions
)
target_link_libraries(test-07 gtest_main)
# ------------------------------------------------------------------------------
# ------------------------------------------------------------------------------
# ------------------------------------------------------------------------------
# ------------------------------------------------------------------------------

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)