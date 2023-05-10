cmake_minimum_required(VERSION 3.14)

# ---- Options ----
# The following options are configurable by the user
option(SSTV_ENABLED "Enable SSTV - Requires Magick++" OFF) # ON or OFF
# -----------------


# ---- Development Variables ----
# Changing these variables can break the build
set(GFS_SRC  ${CMAKE_SOURCE_DIR}/src/flight_software)
set(LIB_SRC ${CMAKE_SOURCE_DIR}/lib)
set(TEST_BIN ${CMAKE_BINARY_DIR}/tests)
set(GIRAFFE_CONFIG_FILE_NAME "gfs-configuration.json")
set(GIRAFFE_BUILD_OUTPUT_DIR ${CMAKE_SOURCE_DIR}/bin)
set(GIRAFFE_INSTALL_DIR /opt/giraffe)
# ---------------------------


# ---- Version File Parseing ----
file(READ ${CMAKE_SOURCE_DIR}/src/version ver_file)
string(
	REGEX MATCH 
	"VERSION_NUMBER <([0-9\.]+)>[\n\r]+STAGE <([A-Za-z ]+)>" 
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


# ----------------------------
# ---- Giraffe Components ----
add_subdirectory(${GFS_SRC}) 							# Giraffe Flight Software
	                                        # Giraffe Ground Software
add_subdirectory(tests EXCLUDE_FROM_ALL)  # Giraffe Tests (Flight and Ground)
# ----------------------------
# ----------------------------


# ---- External Libraries ----
add_subdirectory(${LIB_SRC}/MWAV EXCLUDE_FROM_ALL)
# ----------------------------