cmake_minimum_required(VERSION 3.14)

# ----------------- *** START CONFIGUREABLE ITEMS *** --------------------------
option(SSTV_ENABLED "Enable SSTV - Requires Magick++" OFF) # ON or OFF
# ----------------- ***  END CONFIGUREABLE ITEMS  *** --------------------------


# ---- Source Files/Directories -----
include(${CMAKE_SOURCE_DIR}/directories.cmake)
include(${CMAKE_SOURCE_DIR}/source_list.cmake)
# -----------------------------------

# Clang-Tidy
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# find_program(CLANG_TIDY_EXE NAMES "clang-tidy") 
# set(CLANG_TIDY_CMD "${CLANG_TIDY_EXE}" "-checks=-*,modernize-*")

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
	-Wno-unused-variable \
	-Wswitch-enum \
	-Wno-switch-enum"
)
# ----------------------------


# ----- Third Party Libs -----
# nlhomann_json
include(FetchContent)
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)
FetchContent_MakeAvailable(json)

# ncurses
find_package(Curses REQUIRED)

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
	${COMMON_SRC}/protocol
	${LIB_SRC}/MWAV/include
)
# ----------------------------


# ---- Giraffe Components ----
# Flight Software
add_subdirectory(${GFS_SRC} ./flight_software)

# External Libraries
add_subdirectory(${LIB_SRC}/MWAV)
add_subdirectory(${LIB_SRC}/Boost_erSeat)

# Unit Tests
enable_testing()
add_subdirectory(${CMAKE_SOURCE_DIR}/tests)
# ----------------------------
