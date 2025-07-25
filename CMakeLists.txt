cmake_minimum_required(VERSION 3.20)

# ----------------- *** START CONFIGURABLE ITEMS *** --------------------------
option(SSTV_ENABLED "Enable SSTV - Requires Magick++" OFF)
option(GFS_HOST_TEST "Enable GFS Host Test" OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS OFF)
# ----------------- ***  END CONFIGURABLE ITEMS  *** --------------------------

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
file(READ ${CMAKE_SOURCE_DIR}/build/version.ini ver_file)
string(REGEX MATCH
  "stage = ([a-z]+)[ \n\r]+clean = [a-zA-Z]+\[ \n\r]+semantic = ([0-9\.]+)(-*[a-z]*-[a-z]*)[ \n\r]+"
  GIRAFFE_SEMANTIC_VERSION
  ${ver_file}
)
set(GIRAFFE_VERSION_STAGE ${CMAKE_MATCH_1})
set(GIRAFFE_VERSION_NUMBER ${CMAKE_MATCH_2})
set(GIRAFFE_SEMANTIC_VERSION "${GIRAFFE_VERSION_NUMBER}${CMAKE_MATCH_3}")

message(STATUS "Giraffe Version Number: ${GIRAFFE_VERSION_NUMBER}")
message(STATUS "Giraffe Stage Label: ${GIRAFFE_STAGE_LABEL}")
message(STATUS "Giraffe Semantic Version: ${GIRAFFE_SEMANTIC_VERSION}")

add_compile_definitions(
  GIRAFFE_SEMANTIC_VERSION="${GIRAFFE_SEMANTIC_VERSION}"
  GIRAFFE_VERSION_STAGE="${GIRAFFE_SEMANTIC_VERSION}"
  GIRAFFE_VERSION_NUMBER="${GIRAFFE_VERSION_NUMBER}"
)
message(STATUS "Giraffe Software Version: ${GIRAFFE_VERSION_NUMBER} \
${GIRAFFE_VERSION_STAGE}")
# -------------------------------

# --- DIRECTORIES --- #
# need to get rid of these
set(THIRD_PARTY_SRC ${CMAKE_SOURCE_DIR}/lib/third_party)


# @todo remove these, let `add_subdirectory` handle it
# and use relative paths in those CMakeLists.txt files
set(GIRAFFE_DAEMON_SRC ${CMAKE_SOURCE_DIR}/src/daemon)
set(GFS_SRC ${CMAKE_SOURCE_DIR}/src/flight_system)
set(GDL_SRC ${CMAKE_SOURCE_DIR}/src/data_link)
set(GFS_BIN ${CMAKE_BINARY_DIR}/bin)
set(GFS_TEST_SRC ${CMAKE_SOURCE_DIR}/tests)
set(GFS_UNIT_TEST_SRC ${GFS_TEST_SRC}/flight_system)
set(GDL_TEST_SRC ${CMAKE_SOURCE_DIR}/tests/data_link)
set(GIRAFFE_DAEMON_TEST_SRC ${CMAKE_SOURCE_DIR}/tests/daemon)

# @todo remove these
set(GFS_BIN ${CMAKE_BINARY_DIR}/bin)
set(GDL_BIN ${CMAKE_BINARY_DIR}/bin)

# @todo clean up
set(GFS_TEST_BIN ${CMAKE_BINARY_DIR}/bin/tests/flight_system)
set(GDL_TEST_BIN ${CMAKE_BINARY_DIR}/bin/tests/data_link)
set(TEST_BIN ${CMAKE_BINARY_DIR}/bin/tests)


# ------------------- #

if(BUILD_FOR_PICO)
  # Must be set before project() is called
  message(STATUS "==>> Building for Pico <<==")
  include("./project/cmake/pico_sdk_import.cmake")
endif()

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
-Wno-float-equal \
-Wno-unused-variable \
-Wswitch-enum \
-Wno-switch-enum \
-Wno-psabi"
)
# ----------------------------


if(BUILD_FOR_PICO)
  add_subdirectory(src/power_board_pico) # Power Board Pico
  add_subdirectory(lib/third_party/no-OS-FatFS-SD-SPI-RPi-Pico/FatFs_SPI build)

else()
  # ----- Third Party Libs -----
  set(THREADS_PREFER_PTHREAD_FLAG ON)
  find_package(Threads REQUIRED) # pthreads
  find_package(Curses REQUIRED) # ncurses
  find_package(CURL REQUIRED) # curl
  add_subdirectory(lib/third_party/sqlite_cpp) # sqlite_cpp
  set(BUILD_GMOCK ON CACHE BOOL "Build Google Mock with Google Test" FORCE)
  add_subdirectory(lib/third_party/googletest) # googletest
  add_subdirectory(lib/third_party/nlohmann_json) # nlohmann_json
  add_subdirectory(lib/third_party/fmt) # fmt

  # ---- Giraffe Components ----
  add_subdirectory(project) # Auto-generated files
  add_subdirectory(src/flight_system) # Flight System
  add_subdirectory(src/common) # Common
  add_subdirectory(src/data_link) # Data Link
  add_subdirectory(${GFS_TEST_SRC}/simulator) # gfs_simulator
  add_subdirectory(lib/SignalEasel) # SignalEasel
  add_subdirectory(lib/Boost_erSeat) # BoosterSeat
  add_subdirectory(lib/bzip2-cpp) # bzip2-cpp
  # ---- Utilities ----
  add_subdirectory(project/eeprom) # EEPROM Utility
  # ----------------------------

  # Unit Tests
  enable_testing()
  add_subdirectory(${GFS_TEST_SRC})

  # Additional CMake Files
  include("project/package/package.cmake")

endif()
