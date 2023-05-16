cmake_minimum_required(VERSION 3.14)

# ---- Options ----
# The following options are configurable by the user
option(SSTV_ENABLED "Enable SSTV - Requires Magick++" OFF) # ON or OFF
# -----------------


# ---- Development Variables ----
set(COMMON_SRC ${CMAKE_SOURCE_DIR}/src/common)
set(COMMON_BIN ${CMAKE_BINARY_DIR}/common)
set(COMMON_TEST_SRC ${COMMON_SRC}/tests)
set(COMMON_TEST_BIN ${CMAKE_BINARY_DIR}/tests/common)

set(GFS_SRC  ${CMAKE_SOURCE_DIR}/src/flight_software)
set(GFS_BIN ${CMAKE_BINARY_DIR}/flight_software)
set(GFS_TEST_SRC ${GFS_SRC}/tests)
set(GFS_TEST_BIN ${CMAKE_BINARY_DIR}/tests/flight_software)

set(LIB_SRC ${CMAKE_SOURCE_DIR}/lib)
set(GIRAFFE_CONFIG_FILE_NAME "gfs-configuration.json")
set(GIRAFFE_BUILD_OUTPUT_DIR ${CMAKE_SOURCE_DIR}/bin)
set(GIRAFFE_INSTALL_DIR /opt/giraffe)
# ---------------------------


# ---- Version File Parseing ----
file(READ ${CMAKE_SOURCE_DIR}/src/version ver_file)
string(
	REGEX MATCH 
	"VERSION_NUMBER<([0-9\.]+)>[\n\r]+STAGE<([A-Za-z ]+)>" 
	GIRAFFE_VERSION_STAGE 
	${ver_file}
)
set(GIRAFFE_VERSION_NUMBER ${CMAKE_MATCH_1})
set(GIRAFFE_VERSION_STAGE ${CMAKE_MATCH_2})
add_compile_definitions(
	GIRAFFE_VERSION_NUMBER="${GIRAFFE_VERSION_NUMBER}"
	GIRAFFE_VERSION_STAGE="${GIRAFFE_VERSION_STAGE}"
)
message(STATUS "Giraffe Software Version: ${GIRAFFE_VERSION_STAGE} \
${GIRAFFE_VERSION_NUMBER}")
# -------------------------------


# ---- Project ----
project(
	Giraffe 
	VERSION ${GIRAFFE_VERSION_NUMBER}
	DESCRIPTION "A Unified High Altitude Flight Observation System. \
		With homogeneous software and hardware, both on the ground and in the air."
	HOMEPAGE_URL "https://giraffe.joshuajer.red/"
)
# -----------------


# ---- g++ Configuration ----
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
	-fno-omit-frame-pointer \
	-pedantic \
	-Wall \
	-Wextra \
	-Weffc++ \
	-Wdisabled-optimization \
	-Wfloat-equal \
	-Wno-unused-variable"
)
# ----------------------------


# ----- Third Party Libs -----
# nlhomann_json
include(FetchContent)
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)
FetchContent_MakeAvailable(json)

# gtest
FetchContent_Declare(
	googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.12.1
	)
FetchContent_MakeAvailable(googletest)
include(GoogleTest)
# ----------------------------


# ----- Common Includes ------
include_directories(
	${COMMON_SRC}/

	${GFS_SRC}/modules
	${GFS_SRC}/modules/configuration

	${GFS_SRC}/utilities
	${GFS_SRC}/extensions
	${GFS_SRC}/interface
	${GFS_SRC}/radios

	${LIB_SRC}/MWAV/include
)
# ----------------------------


# ---- Giraffe Components ----
add_subdirectory(${GFS_SRC} ./flight_software)

# ---- External Libraries ----
add_subdirectory(${LIB_SRC}/MWAV ./lib)
# ----------------------------


# -------- Unit Tests --------
add_subdirectory(${GFS_TEST_SRC} ${GFS_TEST_BIN})
add_subdirectory(${COMMON_TEST_SRC} ${COMMON_TEST_BIN})
# ----------------------------
