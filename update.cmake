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

# add_custom_target(giraffe_ground_station
#   COMMENT "Package Giraffe Ground Station"
#   COMMAND ${CMAKE_COMMAND} -E copy
#                 ${CMAKE_SOURCE_DIR}/src/ground_station
#                 ${CMAKE_CURRENT_BINARY_DIR}/ground_station
#   # COMMAND bash "${CMAKE_SOURCE_DIR}/src/scripts/package_ground_station.sh"
# )


add_custom_target(giraffe
COMMENT "[recommended] Build all primary Giraffe targets"
DEPENDS
  giraffe_flight_system
  giraffe_command_line_interface
  giraffe_data_link_server
  # giraffe_ground_station
  COMMAND echo ""
)

add_custom_target(giraffe_update
  DEPENDS giraffe

  COMMAND rm -rf "${CMAKE_CURRENT_BINARY_DIR}/bin/ground_station.tar.gz"
  COMMAND rm -rf "${CMAKE_CURRENT_BINARY_DIR}/ground_station"
  COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_SOURCE_DIR}/src/ground_station
                ${CMAKE_CURRENT_BINARY_DIR}/ground_station
  COMMAND ${CMAKE_COMMAND} -E copy
    "${CMAKE_CURRENT_BINARY_DIR}/../version.json"
    "${CMAKE_CURRENT_BINARY_DIR}/bin/version.json"
  COMMAND bash "${CMAKE_SOURCE_DIR}/src/scripts/package_giraffe.sh"
  # COMMAND ${CMAKE_COMMAND} -E copy
    # "${CMAKE_CURRENT_BINARY_DIR}/bin/giraffe.tar.gz"
    # "${CMAKE_CURRENT_BINARY_DIR}/bin/giraffe-update-${GIRAFFE_VERSION_NUMBER}.tar.gz"
)

# --- 'install' target: Installs the 'giraffe' target components --- #
# message(STATUS "Giraffe Install Directory: ${GIRAFFE_INSTALL_DIR}")

# install(DIRECTORY DESTINATION ${GIRAFFE_INSTALL_DIR})

# install(PROGRAMS
#   "${CMAKE_SOURCE_DIR}/src/scripts/install.sh"
# )