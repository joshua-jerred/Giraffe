cmake_minimum_required(VERSION 3.14)
project(Giraffe VERSION 0.4)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_BUILD_TYPE Debug) # Change to Release for production
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
	-fno-omit-frame-pointer \
	-pedantic \
	-Wall \
	-Wextra \
	-Weffc++ \
	-Wdisabled-optimization \
	-Wfloat-equal \
	-Wno-unused-variable")

include(FetchContent) # for gtest and nlohmann_json

# nlhomann_json
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)
FetchContent_MakeAvailable(json)

set(GFS_SRC  ${CMAKE_SOURCE_DIR}/src/flight-computer)
set(TEST_BIN ${CMAKE_BINARY_DIR}/tests)

include_directories(
	${GFS_SRC}/flight-computer
	${GFS_SRC}/modules
	${GFS_SRC}/utilities
	${GFS_SRC}/extensions
	${GFS_SRC}/extension-interface

	src/MWAV/include
)

# ---- Flight Computer ----
add_executable(giraffe-flight-software
	${GFS_SRC}/main.cpp
	${GFS_SRC}/flight-runner.cpp

	${GFS_SRC}/modules/module.cpp
	${GFS_SRC}/modules/module-configuration.cpp
	${GFS_SRC}/modules/module-data.cpp
	${GFS_SRC}/modules/module-extensions.cpp
	${GFS_SRC}/modules/module-console.cpp
	${GFS_SRC}/modules/module-telemetry.cpp
	${GFS_SRC}/modules/module-server.cpp

	${GFS_SRC}/utilities/timer.cpp
	${GFS_SRC}/utilities/data-stream.cpp
	${GFS_SRC}/utilities/socket.cpp
	${GFS_SRC}/utilities/ubx.cpp

	${GFS_SRC}/extension-interface/i2c.cpp
	${GFS_SRC}/extension-interface/one-wire.cpp

	${GFS_SRC}/extensions/extension.cpp
	${GFS_SRC}/extensions/simulated-extensions.cpp
	${GFS_SRC}/extensions/bmp180.cpp
	${GFS_SRC}/extensions/ds18b20.cpp
	${GFS_SRC}/extensions/samm8q.cpp
	${GFS_SRC}/extensions/bme280.cpp
	${GFS_SRC}/extensions/sysinfo.cpp
	${GFS_SRC}/extensions/max17049.cpp
	${GFS_SRC}/extensions/mcp3021.cpp
	${GFS_SRC}/extensions/sa868.cpp
	${GFS_SRC}/extensions/rpicam.cpp
)
target_link_libraries(giraffe-flight-software PRIVATE MWAVlib)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(giraffe-flight-software PRIVATE Threads::Threads)

target_link_libraries(giraffe-flight-software PUBLIC i2c)
target_link_libraries(giraffe-flight-software PRIVATE nlohmann_json::nlohmann_json)

# Installation
install(TARGETS giraffe-flight-software DESTINATION /opt/giraffe)
install(DIRECTORY ${GFS_SRC}/web-server DESTINATION /opt/giraffe)
install(FILES ${GFS_SRC}/config.json DESTINATION /opt/giraffe)

# Tests
add_subdirectory(tests EXCLUDE_FROM_ALL)
add_subdirectory(src/MWAV EXCLUDE_FROM_ALL)

# CPack
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)