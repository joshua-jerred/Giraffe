cmake_minimum_required(VERSION 3.14)
project(Giraffe VERSION 0.1)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

# ---- Flight Computer ----

add_executable(Giraffe-FC-Alpha
    src/flight-computer/main.cpp
    src/flight-computer/flight-runner.cpp
    src/flight-computer/modules/module-configuration.cpp
    src/flight-computer/modules/module-data.cpp
    src/flight-computer/modules/module-extensions.cpp
    src/flight-computer/modules/module-console.cpp
    src/flight-computer/modules/module-telemetry.cpp
    src/flight-computer/utilities/utility-timer.cpp
    src/flight-computer/utilities/utility-data-stream.cpp
    src/flight-computer/extensions/extension.cpp
    src/flight-computer/extensions/simulated-extensions.cpp
)

file(COPY "src/flight-computer/config.json" DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/bin)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin/data_logs)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin/error_logs)

target_include_directories(Giraffe-FC-Alpha PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/flight-computer
    ${CMAKE_CURRENT_SOURCE_DIR}/src/flight-computer/modules
    ${CMAKE_CURRENT_SOURCE_DIR}/src/flight-computer/utilities
    ${CMAKE_CURRENT_SOURCE_DIR}/src/flight-computer/extensions
)

# ---- Flight Computer Tests ----
enable_testing()

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.12.1
)
FetchContent_MakeAvailable(googletest)

# test-01
add_executable(test-01 
    tests/test-01/test.cpp
    src/flight-computer/modules/module-configuration.cpp
)
set_target_properties( test-01 PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests/test-01/ )
target_link_libraries(test-01 GTest::gtest_main) 
target_include_directories(test-01 PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/flight-computer
    ${CMAKE_CURRENT_SOURCE_DIR}/src/flight-computer/modules
    ${CMAKE_CURRENT_SOURCE_DIR}/src/flight-computer/utilities
)
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)