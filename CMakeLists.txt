cmake_minimum_required(VERSION 3.2)

# ----------------- *** START CONFIGURABLE ITEMS *** --------------------------
option(SSTV_ENABLED "Enable SSTV - Requires Magick++" OFF)
option(EXTENSION_DEBUG_PRINT "Enable Debug Printing for Extensions" OFF)
option(GFS_HOST_TEST "Enable GFS Host Test" OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS OFF)
# ----------------- ***  END CONFIGURABLE ITEMS  *** --------------------------

# --- DIRECTORIES --- # - need to get rid of these
set(COMMON_TEST_SRC ${CMAKE_SOURCE_DIR}/tests/common)
set(COMMON_TEST_BIN ${CMAKE_BINARY_DIR}/bin/tests/common)

set(THIRD_PARTY_SRC ${CMAKE_SOURCE_DIR}/lib/third_party)

set(GFS_SRC  ${CMAKE_SOURCE_DIR}/src/flight_system)
set(GFS_BIN ${CMAKE_BINARY_DIR}/bin)
set(GFS_TEST_SRC ${CMAKE_SOURCE_DIR}/tests/flight_system)
set(GFS_TEST_BIN ${CMAKE_BINARY_DIR}/bin/tests/flight_system)

set(GDL_SRC ${CMAKE_SOURCE_DIR}/src/data_link)
set(GDL_BIN ${CMAKE_BINARY_DIR}/bin)
set(GDL_TEST_SRC ${CMAKE_SOURCE_DIR}/tests/data_link)
set(GDL_TEST_BIN ${CMAKE_BINARY_DIR}/bin/tests/data_link)

set(TEST_BIN ${CMAKE_BINARY_DIR}/bin/tests)
# ------------------- #

# Debug Printing
if(EXTENSION_DEBUG_PRINT)
  add_compile_definitions(EXTENSION_DEBUG_PRINT=1)
  message(WARNING "EXTENSION DEBUG PRINTING ENABLED")
else()
  add_compile_definitions(EXTENSION_DEBUG_PRINT=0)
endif()

# Raspberry Pi Build
# if($ENV{RASPBERRY_PI} EQUAL 1)
#   message(STATUS "Raspberry Pi Build: Enabled")
#   add_compile_definitions(RASPBERRY_PI=1)
# else()
#   message(STATUS "Raspberry Pi Build: Disabled")
# endif()

# ---- Version Files ----
execute_process(
  COMMAND ${CMAKE_SOURCE_DIR}/project/generate_version_files.py
  RESULT_VARIABLE output
  OUTPUT_VARIABLE version_output
  TIMEOUT 3
)
if(NOT output EQUAL 0)
  message(
    FATAL_ERROR
    "Failed to generate version files"
    ${version_output}
    ${output})
endif()
file(READ ${CMAKE_SOURCE_DIR}/version.ini ver_file)
string(
  REGEX MATCH
  "version = ([0-9\.]+)[ \n\r]+stage = ([a-zA-Z]+)"
  GIRAFFE_VERSION_STAGE
  ${ver_file}
)
set(GIRAFFE_VERSION_NUMBER ${CMAKE_MATCH_1})
set(GIRAFFE_VERSION_STAGE ${CMAKE_MATCH_2})
add_compile_definitions(
  GIRAFFE_VERSION_NUMBER="${GIRAFFE_VERSION_NUMBER}"
  GIRAFFE_VERSION_STAGE="${GIRAFFE_VERSION_STAGE}"
)
message(STATUS "Giraffe Software Version: ${GIRAFFE_VERSION_NUMBER} \
${GIRAFFE_VERSION_STAGE}")
# -------------------------------


# ---- Project ----
project(Giraffe
  VERSION ${GIRAFFE_VERSION_NUMBER}
  DESCRIPTION "A Unified High Altitude Flight Observation System. \
    With homogeneous software and hardware, both on the ground and in the air."
  HOMEPAGE_URL "https://giraffe.joshuajer.red/")
# -----------------


# ---- g++ Configuration ----
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
-fno-omit-frame-pointer \
-Wpedantic \
-Wall \
-Wextra \
-Wdisabled-optimization \
-Wfloat-equal \
-Wno-unused-variable \
-Wswitch-enum \
-Wno-switch-enum \
-Wno-psabi"
)
# ----------------------------


# ----- Third Party Libs -----
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)                  # pthreads
find_package(Curses REQUIRED)                   # ncurses
find_package(CURL REQUIRED)                     # curl
add_subdirectory(lib/third_party/sqlite_cpp)    # sqlite_cpp
add_subdirectory(lib/third_party/nlohmann_json) # nlohmann_json
add_subdirectory(lib/third_party/googletest)    # googletest
add_subdirectory(lib/third_party/fmt)           # fmt
# ----------------------------

# add_definitions(-DDONT_ADD_SIGNAL_EASEL_BOOSTER_SEAT)
# set(DONT_ADD_SIGNAL_EASEL_BOOSTER_SEAT 1)

# ---- Giraffe Components ----
add_subdirectory(project)                  # Auto-generated files
add_subdirectory(src/common)               # Common
add_subdirectory(src/flight_system)        # Flight System
add_subdirectory(src/data_link)            # Data Link
add_subdirectory(lib/SignalEasel)          # SignalEasel
add_subdirectory(lib/Boost_erSeat)         # BoosterSeat
add_subdirectory(lib/bzip2-cpp)            # bzip2-cpp
# ---- Utilities ----
add_subdirectory(project/eeprom)           # EEPROM Utility
# ----------------------------

# Unit Tests
enable_testing()
add_subdirectory(${CMAKE_SOURCE_DIR}/tests)
