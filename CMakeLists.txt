cmake_minimum_required(VERSION 3.14)

# ----------------- *** START CONFIGUREABLE ITEMS *** --------------------------
option(SSTV_ENABLED "Enable SSTV - Requires Magick++" OFF) # ON or OFF
option(EXTENSION_DEBUG_PRINT "Enable Debug Printing for Extensions" OFF) # ON or OFF
# ----------------- ***  END CONFIGUREABLE ITEMS  *** --------------------------


# ---- Source Files -----
include(${CMAKE_SOURCE_DIR}/sources.cmake)
# -----------------------

set(CMAKE_EXPORT_COMPILE_COMMANDS OFF)

# Debug Printing
if(EXTENSION_DEBUG_PRINT)
	add_compile_definitions(EXTENSION_DEBUG_PRINT=1)
	message(WARNING "EXTENSION DEBUG PRINTING ENABLED")
else()
	add_compile_definitions(EXTENSION_DEBUG_PRINT=0)
endif()

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
-Wpedantic \
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
find_package(nlohmann_json 3.9.0 REQUIRED)
find_package(Curses REQUIRED)
find_package(CURL REQUIRED) 


# gtest
find_library(GTEST_LIB NAMES gtest)
# ----------------------------

# ---- Giraffe Components ----
# Common
add_subdirectory(${COMMON_SRC})

# Flight System
add_subdirectory(${GFS_SRC})

# Data Link
add_subdirectory(${GDL_SRC})

# External Libraries
add_subdirectory(${LIB_SRC}/MWAV)
add_subdirectory(${LIB_SRC}/Boost_erSeat)
add_subdirectory(${LIB_SRC}/bzip2-cpp)

# Unit Tests
enable_testing()
add_subdirectory(${CMAKE_SOURCE_DIR}/tests)
# ----------------------------
