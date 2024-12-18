# =*========GIRAFFE========*=
# A Unified Flight Command and Control System
# https://github.com/joshua-jerred/Giraffe
# https://giraffe.joshuajer.red/
# =*=======================*=
#
# @file   install.cmake
#
# =*=======================*=
# @author     Joshua Jerred (https://joshuajer.red)
# @date       2024-12-17
# @copyright  2024 (license to be defined)

# --- CONFIGURATION --- #
# set(GIRAFFE_INSTALL_DIR "/bin/giraffe" CACHE STRING "Giraffe Install Directory - Ground Station")
# --------------------- #

# --- 'giraffe' target: Builds all primary Giraffe targets --- #
add_custom_target(giraffe
COMMENT "[recommended] Build all primary Giraffe targets"
DEPENDS
  giraffe_flight_system
  giraffe_command_line_interface
)

# --- 'install' target: Installs the 'giraffe' target components --- #
# message(STATUS "Giraffe Install Directory: ${GIRAFFE_INSTALL_DIR}")

# install(DIRECTORY DESTINATION ${GIRAFFE_INSTALL_DIR})

# install(PROGRAMS
#   "${CMAKE_SOURCE_DIR}/src/scripts/install.sh"
# )